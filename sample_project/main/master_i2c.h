// this file is the header file for the master i2c comfiguration
// this is specifically for communicating with the xm-125 over I2C

#ifndef MASTER_I2C
#define MASTER_I2C
#define i2c_scl_pin 22
#define i2c_sda_pin 21


extern i2c_master_bus_config_t i2c_mst_config;
extern i2c_device_config_t dev_cfg;


#endif 