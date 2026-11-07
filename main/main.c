#include <stdio.h>
#include "string.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_log.h"

#define I2C_BUS_PORT I2C_NUM_0
#define I2C_BUS_SDA_PIN 21
#define I2C_BUS_SCL_PIN 22
#define I2C_BUS_CLK_SRC I2C_CLK_SRC_DEFAULT

#define I2C_TMP117_ADDRESS 0x48
#define I2C_TMP117_FREQ_HZ 400000

static i2c_master_bus_handle_t i2c_bus_handle;
static i2c_master_bus_config_t i2c_bus_config;
static i2c_master_dev_handle_t i2c_tmp117_dev_handle;
uint8_t write_buffer_to_read[1] = {0x00};
uint8_t read_buffer[2];
float conversion_factor = 0.0078125;

static TimerHandle_t tmp117_timer;

void init_i2c_tmp117(void)
{
    i2c_bus_config.i2c_port = I2C_BUS_PORT;
    i2c_bus_config.sda_io_num = I2C_BUS_SDA_PIN;
    i2c_bus_config.scl_io_num = I2C_BUS_SCL_PIN;
    i2c_bus_config.clk_source = I2C_BUS_CLK_SRC;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));

    i2c_device_config_t i2c_tmp117_config = {
        .device_address = I2C_TMP117_ADDRESS,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = I2C_TMP117_FREQ_HZ};

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(i2c_bus_handle, &i2c_tmp117_config, &i2c_tmp117_dev_handle));
}

void read_tmp117_temperature(TimerHandle_t xTimer)
{
    memset(read_buffer, 0, sizeof(read_buffer));
    esp_err_t err_res = i2c_master_transmit_receive(
        i2c_tmp117_dev_handle,
        write_buffer_to_read,
        sizeof(write_buffer_to_read),
        read_buffer,
        2,
        100);

    if (err_res == ESP_OK)
    {
        int16_t temp = (int16_t)(read_buffer[0] << 8) | (int16_t)read_buffer[1];
        float temp_celsius = (float)temp * conversion_factor;
        ESP_LOGI("TMP", "Temp: %.2f F", temp_celsius * 1.8 + 32);
    }
    else
    {
        ESP_LOGI("TMP", "TMP117 Error");
    }
}

void app_main(void)
{
    init_i2c_tmp117();

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
