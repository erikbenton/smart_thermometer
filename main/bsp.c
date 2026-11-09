#include "bsp.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "soc/gpio_periph.h"
#include "hal/gpio_ll.h"
#include "active_object.h"

static Active *temperature_recorder;

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

void init_leds(void)
{
    // Config GPIO for LED pins
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(RED_LED, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(BLUE_LED, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(GREEN_LED, GPIO_PULLDOWN_ONLY);
}

void init_switch(void)
{
    // Config GPIO for switches
    gpio_set_direction(PIN_SW1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_SW1, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(PIN_SW2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_SW2, GPIO_PULLDOWN_ONLY);
}

void BSP_init(Active *temp_recorder)
{
    init_leds();
    init_switch();
    BSP_i2c_master_init();
    temperature_recorder = temp_recorder;
}

esp_err_t BSP_i2c_master_add_device(i2c_device_config_t *i2c_device_config, i2c_master_dev_handle_t *i2c_device_handle)
{
    esp_err_t res = i2c_master_bus_add_device(i2c_bus_handle, i2c_device_config, i2c_device_handle);

    return res;
}

void BSP_ledRedOn(void)
{
    gpio_set_level(RED_LED, LED_ON);
}

void BSP_ledRedOff(void)
{
    gpio_set_level(RED_LED, LED_OFF);
}

void BSP_ledBlueOn(void)
{
    gpio_set_level(BLUE_LED, LED_ON);
}

void BSP_ledBlueOff(void)
{
    gpio_set_level(BLUE_LED, LED_OFF);
}

void BSP_ledGreenOn(void)
{
    gpio_set_level(GREEN_LED, LED_ON);
}

void BSP_ledGreenOff(void)
{
    gpio_set_level(GREEN_LED, LED_OFF);
}

// Must enable FreeRTOS tick hook in order to use: configUSE_TICK_HOOK
void vApplicationTickHook(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    static struct ButtonsDebouncing
    {
        uint32_t depressed;
        uint32_t previous;
    } buttons = {0U, 0U};
    uint32_t current;
    uint32_t tmp;

    TimeEvent_tick(); /* process AO Time Events */

    // current = (uint32_t)gpio_get_level(PIN_SW1) << PIN_SW1;
    current = GPIO.in;                                 /* read level of 0-31 GPIO pins */
    tmp = buttons.depressed;                           /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous = current;                        /* update the history */
    tmp ^= buttons.depressed;                          /* changed debounced depressed */
    if ((tmp & SW1) != 0U)
    { /* debounced SW1 state changed? */
        if ((buttons.depressed & SW1) != 0U)
        { /* is SW1 depressed? */
            /* post the "button-pressed" event */
            static Event const start_recording_evt = {START_RECORDING_SIG};
            Active_postFromISR(temperature_recorder, &start_recording_evt, &xHigherPriorityTaskWoken);
        }
        else
        { /* the button is released */
        }
    }
    if ((tmp & SW2) != 0U)
    { /* debounced SW2 state changed? */
        if ((buttons.depressed & SW2) != 0U)
        { /* is SW2 depressed? */
            /* post the "button-pressed" event */
            static Event const stop_recording_evt = {STOP_RECORDING_SIG};
            Active_postFromISR(temperature_recorder, &stop_recording_evt, &xHigherPriorityTaskWoken);
        }
        else
        { /* the button is released */
        }
    }
}