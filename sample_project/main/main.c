#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "alarm.h"
#include "circular_buffer.h"
#include "master_i2c.h"

void app_main(void)
{
    printf("Starting main loop \n");

    // i2c initialization
    i2c_master_bus_handle_t radar_handle;
    ESP_ERROR_CHECK(i2c_master_init(
        I2C_CLK_SRC_DEFAULT,
        i2c_port_0,
        i2c_scl_pin,
        i2c_sda_pin,
        glitch_ignore_count,
        true,
        &radar_handle));
    printf("adding slave device\n");

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_target_init(I2C_ADDR_BIT_LEN_7,xm125_address,scl_frequency,&dev_handle,radar_handle));
    printf("inited and added a slave device\n");

    // circular_buffer_initialization
    CircularBuffer radar_circular_buffer;
    cb_init(&radar_circular_buffer);

    // alarm initialization 
    printf("initializaing alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_init());
    printf("enabling alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_enable());
    printf("starting alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_start());

    uint32_t count = 0;
    uint8_t reg_address[2] = {0x00,0x20} ;// the specifi register of the .device you want to talk to 
    uint8_t recieved_data[4]; // a home for the data you recieved 
    size_t read_size = 4;
    //below will probably definitely fail if not connected 
    ESP_ERROR_CHECK(i2c_read_write(dev_handle,reg_address,sizeof(reg_address), recieved_data,read_size,1000)); //not &reg_address or &received_data because decays to pointer already
    // ESP_ERROR_CHECK(i2c_master_transm
    while (1){
        if(get_alarm0_flag()==true){
            
            set_alarm0_flag(false);
        }
        // pdMS_TO_TICKS converts ms to ticks so vTaskDelay can block the current state for 
        // vTaskDelay block this code for x amt ticks
        vTaskDelay(pdMS_TO_TICKS(500)); 

    }

    printf("done counting to 60!\n");
}
