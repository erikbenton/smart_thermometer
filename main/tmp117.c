#include "tmp117.h"
#include "bsp.h"
#include "driver/i2c_master.h"
#include "string.h"

static i2c_master_dev_handle_t i2c_tmp117_dev_handle;

static uint8_t const write_buffer_to_read[1] = {0x00};
static uint8_t read_buffer[2];
static float const conversion_factor = 0.0078125f;

void TMP117_init(void)
{
    i2c_device_config_t i2c_tmp117_config = {
        .device_address = I2C_TMP117_ADDRESS,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = I2C_TMP117_FREQ_HZ};

    BSP_i2c_master_add_device(&i2c_tmp117_config, &i2c_tmp117_dev_handle);
}

esp_err_t TMP117_read_temperature_celsius(float *temperature)
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
        // I2C is MSB - need to shift 0th byte to be LSB
        int16_t temp = (int16_t)(read_buffer[0] << 8) | (int16_t)read_buffer[1];
        *temperature = (float)temp * conversion_factor;
    }

    return err_res;
}