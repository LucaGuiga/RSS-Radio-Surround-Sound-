// this file is the header file for the master i2c comfiguration
// this is specifically for communicating with the xm-125 over I2C

#ifndef MASTER_I2C_H
#define MASTER_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#define glitch_ignore_count 7
#define xm125_address 0x52 // this is the slave (xm-125) address
#define scl_frequency 100000 //replace this with what it needs to be
#define i2c_port_0 I2C_NUM_0
#define i2c_scl_pin 22
#define i2c_sda_pin 21

// a wrapper for i2c_master_bus_config
// initializes the i2c bus
// returns esp_ok if it was inited properly, otherwise error code
esp_err_t i2c_master_init(i2c_clock_source_t clk_src,i2c_port_num_t  i2c_port,int scl_pin_num, int sda_pin_num, uint8_t glitch_ignore_cnt, bool internal_pullup,i2c_master_bus_handle_t* bus_handle); 


// wrapper for i2c_device_config
// initializes the target for the i2c bus
// returns esp_ok if it was inited properly, otherwise error code
// for target_addr_legnth use the espressif i2c length constants
esp_err_t i2c_target_init(i2c_addr_bit_len_t  target_addr_length, uint16_t target_address,uint32_t scl_freq,i2c_master_dev_handle_t* dev_handle ,i2c_master_bus_handle_t bus_handle );

// wrapper for i2c_master_transmit_recieve
esp_err_t i2c_read_write(i2c_master_dev_handle_t dev_handle, uint8_t* reg_address, size_t reg_address_size,uint8_t* recieved_data,size_t read_size, int timeout_value );



#endif 