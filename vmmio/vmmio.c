/*
Alec Stobbs
Discord: @PotatoeComrade

4/13/2023


*/

#include "vmmio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"


/** \brief VMMIO function template
 *  \ingroup VMMIO_H
 *
 *  Allows swift updates to all function structures
 *
 * \param val 8-bit value sent to register
 */
#define VMMIO_FUNC_T(x) void x(uint8_t* val)

struct repeating_timer vmmio_timer;

uint16_t pwm_get_chan_level(uint slice_num, uint chan);
void pwm_step_level(uint8_t pin, uint16_t target);


// Blank filler function for unassigned registers
VMMIO_FUNC_T(f_default){
    DEBUG_PRINTF("Blank VMMIO Register!: ",0);
    DEBUG_PRINTF("0x%02X\r\n", *val);
}

// Some config value idk yet
VMMIO_FUNC_T(sys_config){

}

// A global rate(speed) for all servos to move at
VMMIO_FUNC_T(sys_rate){
    bool cancelled = cancel_repeating_timer(&vmmio_timer);
    DEBUG_PRINTF("Servo Timer Cancelled: %d\r\n", cancelled);
    if (cancelled){
        add_repeating_timer_us(*val*4, irq_vmmio_driver, NULL, &vmmio_timer);
        vmmio_reg_vals[SYS_RATE_ADDR] = *val;
    }
}

VMMIO_FUNC_T(sys_state){

}

// Takes a reg address as val and sends its current value to master through i2c
VMMIO_FUNC_T(sys_i2c_reg){
    
}

// * * * * * * * * * * * * * * * //

// Some servo devices //
// Servo devices expect signed input [-100, 100]
// disregard data outside that range
VMMIO_FUNC_T(srv_claw){
    int8_t temp = *val;
    if (temp < -100 || temp > 100) {
        DEBUG_PRINTF("Value Out of Bounds!: %d\r\n", temp);
        return;
    }
    // Shift range to [0, 200]
    vmmio_reg_vals[SRV_CLAW_ADDR] = (uint8_t)(temp+100);
    DEBUG_PRINTF("Claw Servo Moved: %d\r\n", temp);
}

VMMIO_FUNC_T(srv_torp){
    int8_t temp = *val;
    if (temp < -100 || temp > 100) {
        DEBUG_PRINTF("Value Out of Bounds!: %d\r\n", temp);
        return;
    }
    // Shift range to [0, 200]
    vmmio_reg_vals[SRV_TORP_ADDR] = (uint8_t)(temp+100);
    DEBUG_PRINTF("Torpedo Servo Moved: %d\r\n", temp);
}

// * * * * * * * * * * * * * * * //

// Some pwm devices //
// LED devices expect unsigned input [0x00, 0xFF]
VMMIO_FUNC_T(led_r){
    vmmio_reg_vals[LED_R_ADDR] = *val;
    DEBUG_PRINTF("LED R Changed: %d\r\n", *val);
}

VMMIO_FUNC_T(led_g){
    vmmio_reg_vals[LED_G_ADDR] = *val;
    DEBUG_PRINTF("LED G Changed: %d\r\n", *val);
}

VMMIO_FUNC_T(led_b){
    vmmio_reg_vals[LED_B_ADDR] = *val;
    DEBUG_PRINTF("LED B Changed: %d\r\n", *val);
}


vmmio_func_list_t vmmio_list[256] = {
    [0 ... 0xFF]        =   f_default,      // initialize all members to f_default
    [SYS_REG_BASE]      =   sys_config,     // 0x00
    [SYS_RATE_ADDR]     =   sys_rate,       // 0x01
    [SRV_CLAW_ADDR]     =   srv_claw,       // 0x10
    [SRV_TORP_ADDR]     =   srv_torp,       // 0x11
    [LED_R_ADDR]        =   led_r,          // 0x50
    [LED_G_ADDR]        =   led_g,          // 0x51
    [LED_B_ADDR]        =   led_b           // 0x52
};


uint8_t vmmio_reg_vals[256] = {
    // Initial conditions of reg values. Undefined = 0
    [0 ... 0xFF]        =   0,
    [SYS_RATE_ADDR]     =   SYS_RATE_DEF,
    [SRV_CLAW_ADDR]     =   SRV_CLAW_DEF,
    [SRV_TORP_ADDR]     =   SRV_TORP_DEF,
    [LED_R_ADDR]        =   LED_R_DEF,
    [LED_G_ADDR]        =   LED_G_DEF,
    [LED_B_ADDR]        =   LED_B_DEF
};

/****** Timer Driven Interrupt For Syncronous Servo Control ******/

/** general pwm formula **/
#define SERVO_RANGE         200u                                    // [-100, 100]
#define SERVO_MIN           3600u                                   // TOP val for .0011 s
#define SERVO_MAX           6218u                                   // TOP val for .0019
#define SERVO_FRAC          ((SERVO_MAX - SERVO_MIN) / SERVO_RANGE) // conversion fraction to scale i2c packets up to pwm range
#define INTP_SERVO(v)       ((SERVO_FRAC * v) + SERVO_MIN)             // interolate 8bit range to 16bit range for servos

#define LED_RANGE           256u                                    // [0, 255]
#define LED_MIN             0u
#define LED_MAX             65040u
#define LED_FRAC            ((LED_MAX - LED_MIN) / LED_RANGE)       // conversion fraction to scale i2c packets up to pwm range
#define INTP_LED(v)         ((LED_FRAC * v) + LED_MIN)             // interolate 8bit range to 16bit range for LEDs
/** end **/

/** \brief Initialize the VMMIO timer, pwm devices, and default values
 *  \ingroup TBD
 *
 *  Setup repeating timer with SYS_RATE_DEF period
 *
 */
void setup_vmmio(void){
    // initialize a default rate(speed) value in a sys reg
    // use sys rate as timer interrupt period
    // setup timer based on some math from picture
    // setup interrupt for timer that has handler irq_vmmio_driver()
    // make lower priority

    // Initialize a timer to periodically update the pwm value of all devices
    add_repeating_timer_us(SYS_RATE_DEF*4, irq_vmmio_driver, NULL, &vmmio_timer);

    // setup all pwm devices here
    gpio_set_function(SRV_CLAW_PIN  , GPIO_FUNC_PWM);
    gpio_set_function(SRV_TORP_PIN  , GPIO_FUNC_PWM);
    gpio_set_function(LED_R_PIN     , GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN     , GPIO_FUNC_PWM);
    gpio_set_function(LED_B_PIN     , GPIO_FUNC_PWM);

    // Claw and Torpedo are on the same slice because they operate at the same freq
    uint32_t claw_torp_slice = pwm_gpio_to_slice_num(SRV_CLAW_PIN);
    // LED R & G channels are on the same slice
    uint32_t led_rg_slice = pwm_gpio_to_slice_num(LED_R_PIN);
    // LED B channel is on a different slice
    uint32_t led_b_slice = pwm_gpio_to_slice_num(LED_G_PIN);

    // servo pwm math
    /*50.0 Hz target = .020 s, 125MHz input clk = 8E-9 s

        0.020 = 8E-9 * div * (65535 + 1)

        div = 38.14697 ~= 38.1875 -> 38 int, 3 frac

        0.020 = 8E-9 * 38.1875 * (TOP + 1)

        TOP = 65465.44845 ~= 65465

        T = 8E-9 * 38.1875 * (65465 + 1)

        T = 0.01999986 s
    */

    // RGB LED pwm math
    /*250.0 Hz target = .004 s, 125MHz input clk = 8E-9 s

        0.0004 = 8E-9 * div * (65535 + 1)

        div = 7.629 ~= 7.6875 -> 7 int, 11 frac

        0.004 = 8E-9 * 7.6875 * (TOP + 1)

        TOP = 65040.65 ~= 65040

        T = 8E-9 * 7.6875 * (65040 + 1)

        T = 0.00399996 s
    */

    pwm_set_wrap(claw_torp_slice, 65465u);
    pwm_set_wrap(led_rg_slice, 65040u);
    pwm_set_wrap(led_b_slice, 65040u);
    pwm_set_clkdiv_int_frac(claw_torp_slice, 38u, 3u);
    pwm_set_clkdiv_int_frac(led_rg_slice, 7u, 11u);
    pwm_set_clkdiv_int_frac(led_b_slice, 7u, 11u);

    //enables slice
    pwm_set_enabled(claw_torp_slice, 1u);
    pwm_set_enabled(led_rg_slice, 1u);
    pwm_set_enabled(led_b_slice, 1u);

    // loop through vmmio_list and call all functions with corresponding vmmio_reg_val
    for (uint16_t i = 0; i <= 0xFF; ++i){
        if (vmmio_list[i] == f_default){continue;}
        vmmio_list[i](&vmmio_reg_vals[i]);
    }
}

/** \brief Callback function for pwm update timer
 *  \ingroup TBD
 *
 *  Compares each pwm level against the vmmio_reg_val and
 *  incriments the level by 1 until it equals the vmmio_reg_val
 *
 */
bool irq_vmmio_driver(struct repeating_timer *t){
    // if case for each pwm device to incriment their compare val until it matches new vmmio_reg_val

    // claw
    pwm_step_level(SRV_CLAW_PIN, INTP_SERVO(vmmio_reg_vals[SRV_CLAW_ADDR]));

    // torpedo
    pwm_step_level(SRV_TORP_PIN, INTP_SERVO(vmmio_reg_vals[SRV_TORP_ADDR]));

    // RGB led
    pwm_step_level(LED_R_PIN, INTP_LED(vmmio_reg_vals[LED_R_ADDR]));
    pwm_step_level(LED_G_PIN, INTP_LED(vmmio_reg_vals[LED_G_ADDR]));
    pwm_step_level(LED_B_PIN, INTP_LED(vmmio_reg_vals[LED_B_ADDR]));

    return true;
}

/** \brief Get the current PWM counter compare value for one channel
 *  \ingroup hardware_pwm
 *
 * Get the value of the PWM counter compare value, for either channel A or channel B.
 *
 * \param slice_num PWM slice number
 * \param chan Which channel to read. 0 for A, 1 for B.
 * 
 * \return Current PWM counter compare value for one channel
 */
uint16_t pwm_get_chan_level(uint slice_num, uint chan){
    check_slice_num_param(slice_num);
    return (uint16_t)((pwm_hw->slice[slice_num].cc >> (chan ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB)) & 0xFFFFu);
}

/** \brief Increment the compare value of a given pin by 1 toward a target
 *  \ingroup hardware_pwm
 *
 * Step the value of PWM compare value by 1 toward a given target value
 *
 * \param pin PWM device pin
 * \param target Value to increment compare level toward
 * 
 */
void pwm_step_level(uint8_t pin, uint16_t target){
    uint16_t level = pwm_get_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin));
    int16_t diff = target - level;
    if (diff){
        pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), level + (diff<0 ? -1 : 1));

        #ifdef DEBUG_MODE
            if (diff == 1 || diff == -1){
                printf("Pin %d reached target: %d\r\n", pin, target);
            }
        #endif
    }
}