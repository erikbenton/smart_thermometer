#ifndef _BSP_H_
#define _BSP_H_

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "active_object.h"

void BSP_init(Active *temp_recorder);

/* -------------------- Switch Facilities -------------------- */

#define PIN_SW1 18
#define SW1 1U << PIN_SW1
#define PIN_SW2 19
#define SW2 1U << PIN_SW2

/* -------------------- LED Facilities -------------------- */

#define RED_LED 27
#define BLUE_LED 26
#define GREEN_LED 25

#define LED_ON 1U
#define LED_OFF 0U

void BSP_ledRedOn(void);
void BSP_ledRedOff(void);
void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);
void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);

/* -------------------- Events -------------------- */

enum EventSignals
{
    START_RECORDING_SIG = USER_SIG,
    TIMEOUT_SIG,
    REPORT_TEMP_SIG,
    RESET_RECORDING_SIG,
    STOP_RECORDING_SIG,
    /* ... */
    MAX_SIG
};

/* -------------------- I2C Facilities -------------------- */

#define I2C_BUS_PORT I2C_NUM_0
#define I2C_BUS_SDA_PIN 21
#define I2C_BUS_SCL_PIN 22
#define I2C_BUS_CLK_SRC I2C_CLK_SRC_DEFAULT

esp_err_t BSP_i2c_master_add_device(i2c_device_config_t *i2c_device_config,
                                    i2c_master_dev_handle_t *i2c_device_handle);

#endif