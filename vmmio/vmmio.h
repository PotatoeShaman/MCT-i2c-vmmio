/*
Alec Stobbs
Discord: @PotatoeComrade

4/13/2023


*/

#ifndef VMMIO_H
#define VMMIO_H

#include <stdio.h>
#include "pico/stdlib.h"


#include "../rp2040_nodeConstants.h"


typedef void (*vmmio_func_list_t)(uint8_t* val);

// Array of function pointers: Virtual Registers
extern vmmio_func_list_t vmmio_list[256];

// array that keeps the current state of all registers
extern uint8_t vmmio_reg_vals[256];

void setup_vmmio(void);
bool irq_vmmio_driver(struct repeating_timer *t);

/* Functions dont need need to be globally visable, only need the vmmio_list
#define VMMIO_FUNC_T(x) void x(uint8_t val)
VMMIO_FUNC_T(f_blank);
VMMIO_FUNC_T(sys_config);
VMMMIO_FUNC_T(sys_rate);
VMMIO_FUNC_T(srv_1);
VMMIO_FUNC_T(pwm_1);
*/

// VMMIO_H //
#endif