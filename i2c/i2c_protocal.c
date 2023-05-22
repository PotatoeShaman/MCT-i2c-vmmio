
/*
Alec Stobbs
Discord: @PotatoeComrade

4/17/2023


*/
#include "i2c_protocal.h"

///////////////////// FIGURE OUT EXTERN SITUATION FOR VMMIO_LIST AND I2C_BUFFER //////////////////////////////////
uint8_t i2c_buffer[I2C_BUFFER_SIZE] = {0};

/** \brief Initialize the I2C in slave mode
 *  \ingroup TBD
 *
 *  Setup GPIO pins, i2c SDA and DCL.
 *  Fast i2c, slave mode.
 *
 * \param i2c i2c object (i2c0 or i2c1)
 */
void setup_i2c(i2c_inst_t* i2c){
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    i2c_init(i2c, FAST_I2C_FREQ);
    // Device is a peripheral(slave)
    i2c_set_slave_mode(i2c, 1, I2C_ADDR);

    /****************************/
    // General Interrupt for i2c events
    irq_set_exclusive_handler(I2C1_IRQ, irq_i2c_func);
    // Enable RD_REQ & RX_DONE mask
    i2c1->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);
    // Setup i2c interrupt vector
    irq_set_enabled(I2C1_IRQ, 1);
    /****************************/

    printf("i2c Setup Complete\n");
}

/** \brief Primary I2C interrupt for any enabled events
 *  \ingroup TBD
 *
 * I2C interrupt when recieve is done
 */
void irq_i2c_func(void){

    printf("Got something!\r\n");

    uint32_t status = i2c1->hw->intr_stat;

    // if RX_FULL
    if(status & I2C_IC_INTR_STAT_R_RX_FULL_BITS){
        irq_i2c_rx_done();
    }
    // if RD_REQ
    else if(status & I2C_IC_INTR_MASK_M_RD_REQ_BITS){
        irq_i2c_rd_req();
    }
}

/** \brief Sub I2C interrupt when receive is done
 *  \ingroup TBD
 *
 * I2C interrupt when recieve is done
 */
void irq_i2c_rx_done(void){
    // Returns num bytes recieved
    size_t nbytes = i2c_get_read_available(i2c1);
    
    // read nbytes from FIFO
    for(uint8_t i = 0; i<nbytes && i<I2C_BUFFER_SIZE; ++i){
        i2c_buffer[i] = i2c_read_byte_raw(i2c1);

        //printf("%u: 0x%02X, ", i, i2c_buffer[i]);
    }

    //printf("\r\n");

    // call reg function and pass val
    // 16 bit variant
    //vmmio_list[i2c_buffer[0]]((uint16_t*)&i2c_buffer[1]);
    vmmio_list[i2c_buffer[0]](&i2c_buffer[1]);

    // save received val in reg_vals
    // Currently passed responsibility to each vmmio func,
    // so that disregarding data is possible
    // vmmio_reg_vals[i2c_buffer[0]] = i2c_buffer[1];
}

/** \brief Sub I2C interrupt when master sends a read request
 *  \ingroup TBD
 *
 * I2C interrupt when master sends a read request
 */
void irq_i2c_rd_req(void){
    // Returns num bytes recieved
    size_t nbytes = i2c_get_read_available(i2c1);

    // read nbytes from FIFO
    for(uint8_t i = 0; i<nbytes && i<I2C_BUFFER_SIZE; ++i){
        i2c_buffer[i] = i2c_read_byte_raw(i2c1);
    }

    // wait for nbytes of space in FIFO
    while(i2c_get_write_available(i2c1)<nbytes);

    // write nbytes directly to FIFO
    for(uint8_t i = 0; i<nbytes && i<I2C_BUFFER_SIZE; ++i) {
        i2c_write_byte_raw(i2c1, vmmio_reg_vals[i2c_buffer[i]]);
    }
}

/** \brief Read byte(s) from specified register.
 *  \ingroup TBD
 *
 * Read byte(s) from specified register.
 * If nbytes > 1 read consecutive registers.
 *
 * \param i2c i2c object (i2c0 or i2c1)
 * \param addr 7-bit address of device to send to
 * \param reg 8-bit address of register to send to
 * \param buf Pointer to data to send
 * \param nbytes Length of data in bytes to send
 * 
 * \return Number of bytes written. -2 if addr does not ACK(nowledge)
 */
int i2c_write_reg(  i2c_inst_t*     i2c, 
                    const uint8_t   addr, 
                    const uint8_t   reg, 
                    uint8_t*        buf,
                    const uint8_t   nbytes) {

    int num_bytes_written = 0;
    uint8_t msg[nbytes + 1];

    // Check to make sure caller is sending 1 or more bytes
    if (nbytes < 1) {
        return 0;
    }

    // Append register address to front of data packet
    msg[0] = reg;
    for (uint8_t i = 0; i < nbytes; ++i) {
        msg[i + 1] = buf[i];
    }

    // Write msg to register(s) over I2C
    num_bytes_written = i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

    // Will return -2 if addr does not ACK(nowledge)
    return num_bytes_written;
}

/** \brief Read byte(s) from specified register.
 *  \ingroup TBD
 *
 * Read byte(s) from specified register.
 * If nbytes > 1 read consecutive registers.
 *
 * \param i2c i2c object (i2c0 or i2c1)
 * \param addr 7-bit address of device to read from
 * \param reg 8-bit address of register to read from
 * \param buf Pointer to receive buffer
 * \param nbytes Length of data in bytes to read
 * 
 * \return Number of bytes read
 */
int i2c_read_reg(   i2c_inst_t*     i2c,
                    const uint8_t   addr,
                    const uint8_t   reg,
                    uint8_t*        buf,
                    const uint8_t   nbytes) {

    int num_bytes_read = 0;

    // Check to make sure caller is asking for 1 or more bytes
    if (nbytes < 1) {
        return 0;
    }

    // Read data from register(s) over I2C
    i2c_write_blocking(i2c, addr, &reg, 1, true);
    num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);

    return num_bytes_read;
}
