/*
Alec Stobbs
Discord: @PotatoeComrade

4/13/2023


*/

#include <stdio.h>
#include "pico/stdlib.h"


#include "rp2040_nodeConstants.h"
#include "i2c/i2c_protocal.h"
#include "vmmio/vmmio.h"
#include "core_1/core_1_process.h"


// i2c standard transmission
//  Device Addr        Register Addr		         Val
// [RP2040 Address] , [“Register” (Servo) Number] , [Value]

// i2c 2-byte transmission structure
//     Register Addr[23:16]		Val[15:8]   Rate[7:0]
// [“Register” (Servo) Number] , [Value] , [Move Rate]


/** \brief Core 0 main | I2C Communication
 *  \ingroup TBD
 *
 *  Core_0 sends [reg] and [val], obtained from Master over I2C, to Core_1.
 */
int main(){             
    #ifdef DEBUG_MODE
        // Enable UART
        stdio_init_all();
        // Enable LED
        gpio_init(25);
        gpio_set_dir(25, GPIO_OUT);
    #endif

    // Start up core 1
    //multicore_launch_core1(core_1_entry);


    // Setup i2c1 on pins 2 & 3
    setup_i2c(i2c1);

    // Setup VMMIO timer and pwm devices
    setup_vmmio();

    while(1){
        //tight_loop_contents();
        //busy_wait_us(1000000);
        printf("Looping!\r\n");
        BLINK_LED
    }
}


