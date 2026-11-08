#include "temperature_recorder.h"
#include "active_object.h"
#include "bsp.h"
#include "tmp117.h"
#include "esp_log.h"

void record_temperature(TemperatureRecorder *const me)
{
    TMP117_read_temperature_celsius(&(me->current_temp_celsius));
}

void reset_recordings(TemperatureRecorder *const me)
{
    me->current_temp_celsius = 0.0f;
}

void TemperatureRecorder_ctor(TemperatureRecorder *const me)
{
    Active_ctor(&(me->super), (StateHandler)TemperatureRecorder_initial);
    TimeEvent_ctor(&(me->timeEvt), TIMEOUT_SIG, &(me->super));
}

State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e)
{
    TMP117_init();
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