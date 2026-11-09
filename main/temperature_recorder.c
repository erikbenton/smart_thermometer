#include "temperature_recorder.h"
#include "active_object.h"
#include "bsp.h"
#include "tmp117.h"
#include "esp_log.h"

// exponential moving average filter
static void ema_filter_temperature(TemperatureRecorder *const me, float temperature)
{
    if (me->is_filter_started)
    {
        me->filtered_temp_celsius = me->filter_alpha * temperature + (1.0f - me->filter_alpha) * me->filtered_temp_celsius;
    }
    else
    {
        me->is_filter_started = true;
        me->filtered_temp_celsius = temperature;
    }
}

static void record_temperature(TemperatureRecorder *const me)
{
    float temperature;
    TMP117_read_temperature_celsius(&temperature);
    ema_filter_temperature(me, temperature);
    me->raw_temp_celsius = temperature;
}

static void reset_recordings(TemperatureRecorder *const me)
{
    me->is_filter_started = false;
    me->filtered_temp_celsius = 0.0f;
    me->raw_temp_celsius = 0.0f;
}

void TemperatureRecorder_ctor(TemperatureRecorder *const me)
{
    Active_ctor(&(me->super), (StateHandler)TemperatureRecorder_initial);
    TimeEvent_ctor(&(me->timeEvt), TIMEOUT_SIG, &(me->super));
}

State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e)
{
    TMP117_init();
    me->is_filter_started = false;
    me->filter_alpha = 0.5;
    return TRANSITION(TemperatureRecorder_wait4Start);
}

State TemperatureRecorder_wait4Start(TemperatureRecorder *const me, Event const *const e)
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

State TemperatureRecorder_recording(TemperatureRecorder *const me, Event const *const e)
{
    State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        BSP_ledBlueOn();
        // ESP_LOGI("TEMP AO", "Recording");
        __fallthrough;

    case TIMEOUT_SIG:
        record_temperature(me);
        TimeEvent_arm(&(me->timeEvt), pdMS_TO_TICKS(RECORDING_TIME_PERIOD_MS), 0U);
        status = HANDLED_STATUS;
        break;

    case EXIT_SIG:
        BSP_ledBlueOff();
        reset_recordings(me);
        status = HANDLED_STATUS;
        break;

    case REPORT_TEMP_SIG:
        ReportTemperatureEvent *const report_evt = (ReportTemperatureEvent *const)e;
        if (report_evt->report_cb != NULL)
        {
            report_evt->report_cb(me);
        }
        status = HANDLED_STATUS;
        break;

    case STOP_RECORDING_SIG:
        reset_recordings(me);
        status = TRANSITION(TemperatureRecorder_wait4Start);
        break;

    default:
        status = SUPER(Hsm_top);
        break;
    }

    return status;
}