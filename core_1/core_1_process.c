/*
Alec Stobbs
Discord: @PotatoeComrade

4/17/2023


*/

#include "core_1_process.h"

/** \brief Core 1 main | PWM Driver
 *  \ingroup TBD
 *
 *  Core_0 sends [reg] and [val], obtained from Master over I2C, to Core_1.
 */
void core_1_entry(){
    while(1){
        tight_loop_contents();
    }
}
