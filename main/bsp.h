#ifndef _BSP_H_
#define _BSP_H_

#include "driver/i2c_master.h"
#include "esp_err.h"

void BSP_init(void);

/* -------------------- I2C Facilities -------------------- */

#define I2C_BUS_PORT I2C_NUM_0
#define I2C_BUS_SDA_PIN 21
#define I2C_BUS_SCL_PIN 22
#define I2C_BUS_CLK_SRC I2C_CLK_SRC_DEFAULT

esp_err_t BSP_i2c_master_add_device(i2c_device_config_t *i2c_device_config,
                                    i2c_master_dev_handle_t *i2c_device_handle);

#endif