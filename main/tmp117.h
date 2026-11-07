#ifndef _TMP117_H_
#define _TMP117_H_

#include "esp_err.h"

#define I2C_TMP117_ADDRESS 0x48
#define I2C_TMP117_FREQ_HZ 400000

void TMP117_init(void);
esp_err_t TMP117_read_temperature_celsius(float *temperature);

#endif