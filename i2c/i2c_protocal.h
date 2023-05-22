/*
Alec Stobbs
Discord: @PotatoeComrade

4/17/2023


*/

#ifndef I2C_PROTOCAL_H
#define I2C_PROTOCAL_H

#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"
#include "hardware/irq.h"

#include "../rp2040_nodeConstants.h"
#include "../vmmio/vmmio.h"

#define I2C_BUFFER_SIZE 2


extern uint8_t i2c_buffer[I2C_BUFFER_SIZE];

void setup_i2c(i2c_inst_t*);
int i2c_write_reg(  i2c_inst_t*,
                    const uint8_t,
                    const uint8_t,
                    uint8_t*,
                    const uint8_t
);
int i2c_read_reg(   i2c_inst_t*, 
                    const uint8_t, 
                    const uint8_t, 
                    uint8_t*,
                    const uint8_t
);
void irq_i2c_func(void);
void irq_i2c_rx_done(void);
void irq_i2c_rd_req(void);

// I2C_PROTOCAL_H //
#endif