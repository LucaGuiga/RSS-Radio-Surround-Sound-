

// alarm.c
#include "stdio.h"
#include "esp_err.h"
#include "alarm.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



static gptimer_handle_t gptimer_0 = NULL;

static volatile bool alarm_0_sample_flag = false;

// call init -> enable -> start 

// isr stuff
static bool alarm_0_ISR(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    alarm_0_sample_flag = true;
    return false;

}

gptimer_event_callbacks_t cbs = {

    .on_alarm = alarm_0_ISR, // Call the user callback function when the alarm event occurs

};

esp_err_t alarm_0_init(){
    // enables the clock
    gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_APB, // Select the default clock source
    .direction = GPTIMER_COUNT_UP,      // Counting direction is up
    .resolution_hz = 1 * 1000 * 1000,   // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond
    };

    // enables the alarm
    gptimer_alarm_config_t alarm_config = {
    .reload_count = 0,      // When the alarm event occurs, the timer will automatically reload to 0
    .alarm_count = ALARM_PERIOD, // Set the actual alarm period, since the resolution is 1us, 1000000 represents 1s
    .flags.auto_reload_on_alarm = true, // Enable auto-reload function
    };

    esp_err_t error;

    error = gptimer_new_timer(&timer_config, &gptimer_0);
    if (error != ESP_OK){
        return error;
    }
    error = gptimer_set_alarm_action(gptimer_0, &alarm_config);
    if(error != ESP_OK){
        return error;
    }

    return gptimer_register_event_callbacks(gptimer_0, &cbs, NULL);
}

// starts the timer for group 0 timer 0 
esp_err_t alarm_0_start(){
    return gptimer_start(gptimer_0);
}

// stops the timer for group 0 timer 0
esp_err_t alarm_0_stop(){
    return gptimer_stop(gptimer_0);
}

esp_err_t alarm_0_enable(){
    return gptimer_enable(gptimer_0);
}

esp_err_t alarm_0_disable(){
    return gptimer_disable(gptimer_0);
}

void set_alarm0_flag(bool status){
    alarm_0_sample_flag = status;
}

bool get_alarm0_flag(){
    return alarm_0_sample_flag;
}




// Register timer event callback functions, allowing user context to be carried

// ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_0, &cbs, NULL));

// // Enable the timer
// ESP_ERROR_CHECK(gptimer_enable(gptimer_0));
// // Start the timer
// ESP_ERROR_CHECK(gptimer_start(gptimer_0));

#define alarm_test_bench
#ifdef alarm_test_bench


void app_main(void)
{
    printf("Starting alarm test bench\n");

    printf("initializaing alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_init());

    printf("enabling alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_enable());

    printf("starting alarm 0\n");
    ESP_ERROR_CHECK(alarm_0_start());

    uint32_t count = 0;
    while (count != 60){
        if(get_alarm0_flag()==true){
            printf("alarm_flag is true\n");
            count++;
            set_alarm0_flag(false);
        }
        // pdMS_TO_TICKS converts ms to ticks so vTaskDelay can block the current state for 
        // vTaskDelay block this code for x amt ticks
        vTaskDelay(pdMS_TO_TICKS(500)); 

    }

    printf("done counting to 60!\n");


    
    
    
}

#endif