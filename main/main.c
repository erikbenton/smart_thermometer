#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "bsp.h"
#include "tmp117.h"
#include "temperature_recorder.h"
#include "active_object.h"


static TimerHandle_t tmp117_timer;

static TemperatureRecorder temperature_recorder;
static Event *temperature_recorder_q[10];
static StackType_t temperature_recorder_stack[configMINIMAL_STACK_SIZE];

void read_tmp117_temperature(TimerHandle_t xTimer)
{
    float temp_celsius;

    esp_err_t res = TMP117_read_temperature_celsius(&temp_celsius);

    if (res == ESP_OK)
    {
        ESP_LOGI("TMP", "Temp: %.2f F", temp_celsius * 1.8 + 32);
    }
    else
    {
        ESP_LOGI("TMP", "TMP117 Error");
    }
}

void app_main(void)
{
    BSP_init();

    TMP117_init();

    tmp117_timer = xTimerCreate(
        "TMP117",
        pdMS_TO_TICKS(1000),
        pdTRUE,
        (void *)0,
        read_tmp117_temperature);

    if (tmp117_timer == NULL)
    {
        ESP_LOGE("APP", "TMP117 Failed to init");
    }

    xTimerStart(tmp117_timer, portMAX_DELAY);
}
