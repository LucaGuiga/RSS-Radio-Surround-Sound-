// header file for radar

// using the xm-125
#ifndef RADAR_H
#define RADAR_H
#include <stdint.h>
#include "esp_err.h"
#include "master_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

// values of read can be stored in a circular buffer in main

typedef enum {
    APPLY_CONFIG_AND_CALIBRATE = 1,
    MEASURE_DISTANCE = 2,
    APPLY_CONFIGURATION = 3,
    CALIBRATE = 4,
    RECALIBRATE = 5,
    ENABLE_UART_LOGS = 32,
    DISABLE_UART_LOGS = 33,
    LOG_CONFIGURATION= 34,
    RESET_MODULE = 1381192737
} radar_command_t;


typedef enum {
    DISTANCE_0 = 0x0011,
    DISTANCE_1 =0x0012,
    DISTANCE_2 = 0x0013,
    DISTANCE_3 =0x0014,
    DISTANCE_4 =0x0015,
    DISTANCE_5 =0x0016,
    DISTANCE_6 =0x0017,
    DISTANCE_7 =0x0018,
    DISTANCE_8 =0x0019,
    DISTANCE_9 =0x001a,
} distance_register_t;

typedef enum {
    STRENGTH_0 = 0x001b,
    STRENGTH_1 = 0x001c,
    STRENGTH_2 = 0x001d,
    STRENGTH_3 = 0x001e,
    STRENGTH_4 = 0x001f,
    STRENGTH_5 = 0x0020,
    STRENGTH_6 = 0x0021,
    STRENGTH_7 = 0x0022,
    STRENGTH_8 = 0x0023,
    STRENGTH_9 = 0x0024,

} strength_register_t;

// registers 
typedef enum {
    COMMAND_REGISTER = 0x0100,
    DETECTOR_STATUS_REGISTER = 0x0003,
    START_REGISTER = 0x0040,
    END_REGISTER = 0x0041,
} general_register_t;

typedef enum { //more can be added as we need them 
    DETECTOR_ERROR=0x10000000,
    BUSY=0x80000000
} detector_status_t;

#define RADAR_START 0x01F4 // 500 mm
#define RADAR_END 0x3A98 //15000mm


// initializes radar module --
// only does radar init stuff, the 
// makes sure that detector status reg is not busy and not error 
// writes start and end registers 
// write "COMMAND_REGISTER " w/ APPLY_CONFIG_AND_CALIBRATE
// poll DETECTOR_STATUS_REGISTER make sure no error bits 
// returns final result of i2c 
esp_err_t radar_init(i2c_master_dev_handle_t dev_handle);

// sends radar commands to radar over i2c to measure distance
// returns final result of i2c 
esp_err_t radar_measure_distance(i2c_master_dev_handle_t dev_handle);

// gets the distane from distance number 
// writes the distane from distance register number to distance 
// returns the result of i2c
esp_err_t radar_read_distance(i2c_master_dev_handle_t dev_handle,distance_register_t distance_reg, uint32_t* distance);

// gets the stren from strenfth number 
// writes the distane from strength register number to strenght 
// returns the result of i2c
esp_err_t radar_read_strength(i2c_master_dev_handle_t dev_handle,strength_register_t strength_reg, int32_t* strength);

// returns result of i2c 
// writes the status of radar to status
esp_err_t check_detector_status(i2c_master_dev_handle_t dev_handle,uint32_t* status);

#endif