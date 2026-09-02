#include "radar.h"
#include "driver/i2c_master.h"

#define MAX_POLLING_ATTEMPTS 50
#define TIMEOUT_VAL 1000
//                             which bus     which address you want to write to        how many bytes that address is ,         where you want to put the data being read from,   how many bytes your reading, how long to wait for response 
//  i2c_master_transmit_receive(dev_handle,target_address,sizeof(target_address),received_array,4,1000);

esp_err_t radar_init(i2c_master_dev_handle_t dev_handle){

    esp_err_t error;
    general_register_t reg = DETECTOR_STATUS_REGISTER;
    uint8_t reg_address[2] = {
        (reg >> 8) & 0xFF,
        reg & 0xFF
    };
    uint8_t received_array[4];

    

    // read detector register
    error = i2c_read_write(dev_handle,reg_address,sizeof(reg_address),received_array,4,TIMEOUT_VAL);
    if(error != ESP_OK){
        return error;
    }
    // converting the received data into a uint 32
    uint32_t received_register = {
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    };

    // verify that Busy Nor Error bits are set 
    if((received_register & BUSY)||(received_register & DETECTOR_ERROR)){
        return ESP_FAIL;
    }

    // write start register
    // #define RADAR_START 0x01F4 // 500 mm
    general_register_t target_register = START_REGISTER;
    uint8_t target_address[2] = {
        (target_register >> 8) & 0xFF,
        target_register & 0xFF
    };
    uint8_t start_distance[4] = {
        (RADAR_START>>24)&0xFF,
        (RADAR_START>>16)&0xFF,
        (RADAR_START>>8)&0xFF,
        (RADAR_START>>0)&0xFF
    } ;
    uint8_t start_write[6];

    start_write[0]=target_address[0];
    start_write[1]=target_address[1];
    start_write[2]=start_distance[0];
    start_write[3]=start_distance[1];
    start_write[4]=start_distance[2];
    start_write[5]=start_distance[3];

    error = i2c_master_transmit(dev_handle, start_write, sizeof(start_write), -1);
    if(error != ESP_OK){
        return error;
    }

    // write end register
    // #define RADAR_END 0x3A98 //15000mm
    target_register = END_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;
    

    uint8_t end_distance[4] = {
        (RADAR_END>>24)&0xFF,
        (RADAR_END>>16)&0xFF,
        (RADAR_END>>8)&0xFF,
        (RADAR_END>>0)&0xFF
    } ;
    uint8_t end_write[6];

    end_write[0]=target_address[0];
    end_write[1]=target_address[1];
    end_write[2]=end_distance[0];
    end_write[3]=end_distance[1];
    end_write[4]=end_distance[2];
    end_write[5]=end_distance[3];

    error = i2c_master_transmit(dev_handle, end_write, sizeof(end_write), -1);
    if(error != ESP_OK){
        return error;
    }

    //  Write APPLY CONFIG AND CALIBRATE to Command register.
    target_register = COMMAND_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;
    

    uint8_t radar_command[4] = {
        ((uint32_t)APPLY_CONFIG_AND_CALIBRATE>>24)&0xFF,
        ((uint32_t)APPLY_CONFIG_AND_CALIBRATE>>16)&0xFF,
        ((uint32_t)APPLY_CONFIG_AND_CALIBRATE>>8)&0xFF,
        ((uint32_t)APPLY_CONFIG_AND_CALIBRATE>>0)&0xFF
    } ;
    uint8_t command_write[6];

    command_write[0]=target_address[0];
    command_write[1]=target_address[1];
    command_write[2]=radar_command[0];
    command_write[3]=radar_command[1];
    command_write[4]=radar_command[2];
    command_write[5]=radar_command[3];

    error = i2c_master_transmit(dev_handle, command_write, sizeof(command_write), -1);
    if(error != ESP_OK){
        return error;
    }

    // poll detector status until busy bit is cleared
    target_register = DETECTOR_STATUS_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;

    error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
    if(error != ESP_OK){
        return error;
    }
    received_register = 
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    ;
    int current_polls = 0; 
    while ((received_register & BUSY)&&(current_polls < MAX_POLLING_ATTEMPTS)){
        error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
        if(error != ESP_OK){
            return error;
        }
        received_register = 
            ((uint32_t)received_array[0] << 24) |
            ((uint32_t)received_array[1] << 16) |
            ((uint32_t)received_array[2] << 8)  |
            ((uint32_t)received_array[3])
        ;
        vTaskDelay(pdMS_TO_TICKS(50)); 
        current_polls++;
    }
    if((received_register & BUSY)){
        return ESP_ERR_TIMEOUT;
    }
    // verify no error bits are set in the detector status register
    target_register = DETECTOR_STATUS_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;
    error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
    if(error != ESP_OK){
        return error;
    }
    received_register = 
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    ;
    if((received_register & DETECTOR_ERROR)){
        return ESP_FAIL;
    }

    // now can write measure distance to command register!
    return ESP_OK ;
}

// sends radar commands to radar over i2c to measure distance
// returns final result of i2c 
esp_err_t radar_measure_distance(i2c_master_dev_handle_t dev_handle){
    //  Write MEASURE DISTANCE to Command register.
    esp_err_t error;
    uint8_t received_array[4];
    uint32_t received_register;
    uint8_t target_address[2];

    general_register_t target_register = COMMAND_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;
    

    uint8_t radar_command[4] = {
        ((uint32_t)MEASURE_DISTANCE>>24)&0xFF,
        ((uint32_t)MEASURE_DISTANCE>>16)&0xFF,
        ((uint32_t)MEASURE_DISTANCE>>8)&0xFF,
        ((uint32_t)MEASURE_DISTANCE>>0)&0xFF
    } ;
    uint8_t command_write[6];

    command_write[0]=target_address[0];
    command_write[1]=target_address[1];
    command_write[2]=radar_command[0];
    command_write[3]=radar_command[1];
    command_write[4]=radar_command[2];
    command_write[5]=radar_command[3];

    error = i2c_master_transmit(dev_handle, command_write, sizeof(command_write), -1);
    if(error != ESP_OK){
        return error;
    }

    // • Poll Detector Status until Busy bit is cleared.
    target_register = DETECTOR_STATUS_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;

    error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
    if(error != ESP_OK){
        return error;
    }
    received_register = 
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    ;
    int current_polls = 0; 
    while ((received_register & BUSY)&&(current_polls < MAX_POLLING_ATTEMPTS)){
        error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
        if(error != ESP_OK){
            return error;
        }
        received_register = 
            ((uint32_t)received_array[0] << 24) |
            ((uint32_t)received_array[1] << 16) |
            ((uint32_t)received_array[2] << 8)  |
            ((uint32_t)received_array[3])
        ;
        vTaskDelay(pdMS_TO_TICKS(50)); 
        current_polls++;
    }
    if(received_register & BUSY){
        return ESP_ERR_TIMEOUT;
    }


    // • Verify that no Error bits are set in the Detector Status register.
    target_register = DETECTOR_STATUS_REGISTER;
    target_address[0] = (target_register >> 8) & 0xFF;
    target_address[1] = target_register & 0xFF;
    error = i2c_master_transmit_receive(dev_handle, target_address, sizeof(target_address), received_array, sizeof(received_array), TIMEOUT_VAL);
    if(error != ESP_OK){
        return error;
    }
    received_register = 
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    ;
    if((received_register & DETECTOR_ERROR)){
        return ESP_FAIL;
    }
    return ESP_OK ;
}


esp_err_t radar_read_distance(i2c_master_dev_handle_t dev_handle,distance_register_t distance_reg, uint32_t* distance){
    esp_err_t error; 
    uint8_t target_address[2];
    uint8_t received_array[4];
    target_address[0] = (distance_reg>>8)&0xFF;
    target_address[1] = (distance_reg>>0)&0xFF;

    error = i2c_master_transmit_receive(dev_handle,target_address,sizeof(target_address),received_array,4,TIMEOUT_VAL);
    if(error != ESP_OK){
        return error; 
    }
    uint32_t received_register = {
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    };
    *distance = received_register; 

    return error;

}

esp_err_t radar_read_strength(i2c_master_dev_handle_t dev_handle,strength_register_t strength_reg, int32_t* strength){
    esp_err_t error; 
    uint8_t target_address[2];
    uint8_t received_array[4];
    target_address[0] = (strength_reg>>8)&0xFF;
    target_address[1] = (strength_reg>>0)&0xFF;

    error = i2c_master_transmit_receive(dev_handle,target_address,sizeof(target_address),received_array,sizeof(received_array),TIMEOUT_VAL);
    if(error != ESP_OK){
        return error; 
    }
    int32_t received_register = {
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    };
    *strength = received_register; 

    return error;
}

esp_err_t check_detector_status(i2c_master_dev_handle_t dev_handle,uint32_t* status){
    esp_err_t error; 
    uint8_t target_address[2];
    uint8_t received_array[4];
    target_address[0] = (DETECTOR_STATUS_REGISTER>>8)&0xFF;
    target_address[1] = (DETECTOR_STATUS_REGISTER>>0)&0xFF;

    error = i2c_master_transmit_receive(dev_handle,target_address,sizeof(target_address),received_array,sizeof(received_array),TIMEOUT_VAL);
    if(error != ESP_OK){
        return error; 
    }
    uint32_t received_register = {
        ((uint32_t)received_array[0] << 24) |
        ((uint32_t)received_array[1] << 16) |
        ((uint32_t)received_array[2] << 8)  |
        ((uint32_t)received_array[3])
    };
    *status = received_register; 

    return error;
}

