// alarm.h 
// for setting an alarm to know when to communicate with xm-125 over i2c
// using timer group 0 timer 0

/*
inits
    should auto reload
set alarm
starts timer
stop timer (pause)

isr
    isr stuff happens in the .c 
get alarm status 
set alarm status 
*/
#ifndef ALARM_H
#define ALARM_H
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// alarm period is 1 second
#define ALARM_PERIOD 500000*2 

// call init -> enable -> start 


// timer init

// initializes the timer and the alarm for Group 0 timer 0
// creates timer, allows alarms, allows callbakcs(ISR)  
esp_err_t alarm_0_init();

// starts the timer for group 0 timer 0 
// starts timer 0 
// call in pairs with alarm_0_stop
esp_err_t alarm_0_start();

// stops the timer for group 0 timer 0
// stios timer 0
// call in pairs with alarm_0_start
esp_err_t alarm_0_stop();

// enables tiemr 0 
esp_err_t alarm_0_enable();

// disables tiemr 0
esp_err_t alarm_0_disable();

// sets the alarm0 flag 
void set_alarm0_flag(bool status);

// gets the alarm0 flag
bool get_alarm0_flag();

#endif