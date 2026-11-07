#include "temperature_recorder.h"
#include "active_object.h"
#include "bsp.h"
#include "tmp117.h"

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
        break;

    default:
        status = TRANSITION(Hsm_top);
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
        break;

    default:
        status = TRANSITION(Hsm_top);
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
        break;

    default:
        status = TRANSITION(Hsm_top);
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
        break;

    default:
        status = TRANSITION(Hsm_top);
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
        break;

    default:
        status = TRANSITION(Hsm_top);
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
        break;

    default:
        status = TRANSITION(Hsm_top);
        break;
    }

    return status;
}
