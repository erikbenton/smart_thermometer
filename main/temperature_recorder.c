#include "temperature_recorder.h"
#include "active_object.h"
#include "bsp.h"
#include "tmp117.h"
#include "esp_log.h"

void TemperatureRecorder_ctor(TemperatureRecorder *const me)
{
    Active_ctor(&(me->super), (StateHandler)TemperatureRecorder_initial);
    TimeEvent_ctor(&(me->timeEvt), TIMEOUT_SIG, &(me->super));
}

State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e)
{
    TMP117_init();
    return TRANSITION(TemperatureRecorder_wait4Running);
}

State TemperatureRecorder_wait4Running(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        BSP_ledGreenOn();
        BSP_ledBlueOff();
        status = HANDLED_STATUS;
        break;

    case EXIT_SIG:
        BSP_ledGreenOff();
        status = HANDLED_STATUS;
        break;

    case START_RECORDING_SIG:
        status = TRANSITION(TemperatureRecorder_recording);
        break;

    default:
        status = SUPER(Hsm_top);
        break;
    }

    return status;
}

State TemperatureRecorder_running(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        // turn on led
        status = HANDLED_STATUS;
        break;

    case REPORT_TEMP_SIG:
        status = TRANSITION(TemperatureRecorder_report);
        break;

    case TIMEOUT_SIG:
        status = TRANSITION(TemperatureRecorder_recording);
        break;

    default:
        status = SUPER(Hsm_top);
        break;
    }

    return status;
}

State TemperatureRecorder_recording(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        BSP_ledBlueOn();
        //ESP_LOGI("TEMP AO", "Recording");
        TMP117_read_temperature_celsius(&(me->current_temp_celsius));
        status = TRANSITION(TemperatureRecorder_idle);
        break;

    default:
        status = SUPER(TemperatureRecorder_running);
        break;
    }

    return status;
}

State TemperatureRecorder_idle(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        //ESP_LOGI("TEMP AO", "Idle");
        TimeEvent_arm(&(me->timeEvt), pdMS_TO_TICKS(RECORDING_TIME_PERIOD_MS), 0U);
        status = HANDLED_STATUS;
        break;

    default:
        status = SUPER(TemperatureRecorder_running);
        break;
    }

    return status;
}

State TemperatureRecorder_report(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        // run report_cb function
        ESP_LOGI("TEMP AO", "Temp: %.2f F", (me->current_temp_celsius) * 1.8 + 32);
        status = HANDLED_STATUS;
        break;

    default:
        status = SUPER(TemperatureRecorder_running);
        break;
    }

    return status;
}

State TemperatureRecorder_reset(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        // reset all of the variables
        status = TRANSITION(TemperatureRecorder_wait4Running);
        break;

    default:
        status = SUPER(TemperatureRecorder_running);
        break;
    }

    return status;
}
