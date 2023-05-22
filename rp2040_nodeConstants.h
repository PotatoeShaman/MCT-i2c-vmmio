/*
Alec Stobbs
Discord: @PotatoeComrade

4/13/2023


*/

#ifndef RP2040_NODECONSTANTS_H
#define RP2040_NODECONSTANTS_H


///////////////////////////////////
//      USB Serial Debugging
///////////////////////////////////
#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_PRINTF(x,y) printf(x,y)

#define BLINK_LED \
    gpio_put(25, true); \
    busy_wait_us(100000); \
    gpio_put(25, false); \
    busy_wait_us(1000000); \
    gpio_put(25, true); \
    busy_wait_us(1000000); \
    gpio_put(25, false); \
    busy_wait_us(100000);

#else
#define DEBUG_PRINTF(x) do { } while(0)
#define BLINK_LED(x) do { } while(0)
#endif

///////////////////////////////////
//       I2C Configuration
///////////////////////////////////
#define MASTER_ADDR     0x00                // T4 address
#define I2C_ADDR        0x0A                // Device address
#define FAST_I2C_FREQ   400000              // Fast i2c 400kHz
#define I2C_SDA         2                   // Data line
#define I2C_SCL         3                   // Clk line

/////////////////////////////////// 
//         Virtual MMIO 
/////////////////////////////////// 
#define SYS_REG_BASE    0x00                // Base address for system config
#define SRV_REG_BASE    0x10                // Base address for servo devices
#define PWM_REG_BASE    0x50                // Base address for standard pwm devices
#define RES_1_REG_BASE  0x70                // Base address for [reserved1]
#define RES_2_REG_BASE  0xAF                // Base address for [reserved2]

#define SYS_RATE_ADDR   (SYS_REG_BASE+1u)   // Servo Rate address
#define SRV_CLAW_ADDR   (SRV_REG_BASE   )   // Claw Servo address
#define SRV_TORP_ADDR   (SRV_REG_BASE+1u)   // Torpedo Servo address
#define LED_R_ADDR      (PWM_REG_BASE   )   // RGB LED R address
#define LED_G_ADDR      (LED_R_ADDR+1u  )   // RGB LED G address
#define LED_B_ADDR      (LED_G_ADDR+1u  )   // RGB LED B address

#define SYS_RATE_DEF    71u                 // Default Servo Rate | Sets timer interrupt frequency | us = 4*val to scale [0, 1000] 
#define SRV_CLAW_DEF    0x00                // Default Claw Servo position. Will also be used as safety position
#define SRV_TORP_DEF    0x00                // Default Torpedo Servo position. WIll also be used as safety position
#define LED_R_DEF       0x00                // Default RGB LED R value
#define LED_G_DEF       0x00                // Default RGB LED G value
#define LED_B_DEF       0x00                // Default RGB LED B value

#define SRV_CLAW_PIN    15                  // GPIO pin of Claw Servo PWM
#define SRV_TORP_PIN    14                  // GPIO pin of Torpedo Servo PWM
#define LED_R_PIN       4                   // GPIO pins of RGB LED R value
#define LED_G_PIN       5                   // GPIO pins of RGB LED G value
#define LED_B_PIN       6                   // GPIO pins of RGB LED B value

///////////////////////////////////
//              MISC
///////////////////////////////////
#define LED_PIN 25

///////////////////////////////////
//  State Machine States, OA_STATE
///////////////////////////////////
/*    0   Hard Kill
 *    1   RES
 *    2   Soft Kill
 *    3   Normal operation, no all clear needed to start movement again after an emergency input
 *    4   All Good, clear to run
 *    5
 *    6
 *    7
 */
 #define HARD_KILL_STATE    0
 // State 1 Reserved
 #define SOFT_KILL_STATE    2
 #define TEST_IA_STATE      3
 #define ALL_GOOD_STATE     4

// RP2040_NODECONSTANTS_H //
 #endif