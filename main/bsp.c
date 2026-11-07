#include "bsp.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

static i2c_master_bus_handle_t i2c_bus_handle;
static i2c_master_bus_config_t i2c_bus_config;

void BSP_i2c_master_init(void)
{
    i2c_bus_config.i2c_port = I2C_BUS_PORT;
    i2c_bus_config.sda_io_num = I2C_BUS_SDA_PIN;
    i2c_bus_config.scl_io_num = I2C_BUS_SCL_PIN;
    i2c_bus_config.clk_source = I2C_BUS_CLK_SRC;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
}

void BSP_init(void)
{
    BSP_i2c_master_init();
}

esp_err_t BSP_i2c_master_add_device(i2c_device_config_t *i2c_device_config, i2c_master_dev_handle_t *i2c_device_handle)
{
    esp_err_t res = i2c_master_bus_add_device(i2c_bus_handle, i2c_device_config, i2c_device_handle);

    return res;
}