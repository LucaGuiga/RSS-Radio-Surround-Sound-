// below is for configuring how to use the esp-32 as a master 
// taken from espressif website
#include "driver/i2c_master.h"
#include "stdio.h"
#include "esp_err.h"
#include "master_i2c.h"


#define glitch_ignore_count 7
#define xm125_address 0x52 // this is the slave (xm-125) address
#define scl_frequency 100000 //replace this with what it needs to be
#define i2c_port_0 I2C_NUM_0



// configuration function
i2c_master_bus_config_t i2c_mst_config = { 
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = i2c_port_0,
    .scl_io_num = i2c_scl_pin,
    .sda_io_num = i2c_sda_pin,
    .glitch_ignore_cnt = glitch_ignore_count,
    .flags.enable_internal_pullup = true,
};
/* 
//should be in the main to throw an error 
i2c_master_bus_handle_t bus_handle;
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
*/

// target device 
i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7, //seems to be right
    .device_address = xm125_address,
    .scl_speed_hz = scl_frequency,
};

/* 
//should be in the main to throw an error 
i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
*/

/*
i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle)); // which bus and which address you;re trying to talk to
uint8_t buf[20] = {0x20}; //0b0100000, the specific register of the slave you want to talk to 
uint8_t buffer[2]; //holdes 2 bytes
ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, buf, sizeof(buf), buffer, 2, -1));
parameters in order which i2c device, buf pointer of buffers you want to write, sizeof(buf) number of bytes to write from buf, buffer where received bytes should live, 2 number of bytes to read, -1 timeout value
*/

#define master_test_bench
#ifdef master_test_bench

void app_main(void)
{
    printf("Starting I2C master test bench\n");
    printf("initializing i2c master bus\n");
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    printf("adding slave device\n");
    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    printf("inited and added a slave device\n");

    printf("trying to read/write data");

    printf("starting read/write sequence\n"); //with no device it shouldnt say anything after this or it should throw an error
    uint8_t reg_address[2] = {0x00,0x20} ;// the specifi register of the .device you want to talk to 
    uint8_t recieved_data[4]; // a home for the data you recieved 
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, reg_address, sizeof(reg_address), recieved_data, 4, 1000));
}

#endif