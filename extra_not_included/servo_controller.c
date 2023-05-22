#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <stdint.h>

#define ADC_STEPS   ((1 << 12) - 1)
#define STEP_RANGE   200 // [-100, 100]
#define PWM_PIN     2 // D8 is P2
#define POT_PIN     29 //D3 is P29
#define MIN_VAL     3600u //TOP val for .0011 s
#define MAX_VAL     6218u //TOP val for .0019 s
#define POT_FRAC    ((MAX_VAL - MIN_VAL) / RANGE)

#define FIND_COMPARE(v) ((POT_FRAC * v) + MIN_VAL)

int main(){
    /* 
        50.0 Hz target = .020 s, 125MHz input clk = 8E-9 s

        0.020 = 8E-9 * div * (65535 + 1)

        div = 38.14697 ~= 38.1875 -> 38 int, 3 frac

        0.020 = 8E-9 * 38.1875 * (TOP + 1)

        TOP = 65465.44845 ~= 65465

        T = 8E-9 * 38.1875 * (65465 + 1)

        T = 0.01999986 s


        Min: 1100 us = .0011 s, Max: 1900 us = .0019 s

        T hi= .0011 s

        .0011 = 8E-9 * 38.1875 * (TOP + 1)

        TOP = 3599.654664 ~= 3600

        T hi = 8E-9 * 38.1875 * (3600 + 1) = 0.001100106s 


        T hi = .0019 s

        .0019 = 8E-9 * 38.1875 * (Top + 1)

        TOP = 6218.3126 ~= 6218

        T hi = 8E-9 * 38.1875 * (6218 + 1) = 0.001899905 s

    */

    stdio_init_all(); //Serial monitor

    //Potentiometer setup
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(3);


    // GPIO pwm output
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    //sets up slice
    uint32_t slice = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slice, 65465u);
    pwm_set_clkdiv_int_frac(slice, 38u, 3u);

    //enables slice
    pwm_set_enabled(slice, 1);

    while(1){
        // reads potentiometer
        uint16_t pos = adc_read();
        // Chan 0 == A, Chan 1 == B; P2's channel is A
        //Sets potentiometer using function 
        pwm_set_chan_level(slice, 0, FIND_COMPARE(pos));

        /
        printf("Pot position = %d; TOP val = %d \n", pos, FIND_COMPARE(pos)); //Serial monitor
        sleep_ms(100);
        */
    }
}