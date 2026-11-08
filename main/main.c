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

static TemperatureRecorder temperature_recorder;
static Active *AO_temperature_recorder = &(temperature_recorder.super);

void read_tmp117_temperature(TimerHandle_t xTimer)
{
    static Event const report_temp_evt = {REPORT_TEMP_SIG};
    Active_post(AO_temperature_recorder, &report_temp_evt);
}

void app_main(void)
{
    BSP_init(AO_temperature_recorder);

    TemperatureRecorder_ctor(&temperature_recorder);
    Active_start(&temperature_recorder.super,
                 "Temp AO",
                 1,
                 temperature_recorder_q,
                 sizeof(temperature_recorder_q) / sizeof(temperature_recorder_q[0]),
                 temperature_recorder_stack,
                 sizeof(temperature_recorder_stack) / sizeof(temperature_recorder_stack[0]));

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
