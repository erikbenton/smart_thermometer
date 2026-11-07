#ifndef _TEMPERATURE_RECORDER_H_
#define _TEMPERATURE_RECORDER_H_

#include "active_object.h"

typedef struct TemperatureRecorder TemperatureRecorder;

struct TemperatureRecorder
{
    Active super;
    TimeEvent timeEvt;
    float current_temp_celsius;
};

void TemperatureRecorder_ctor(TemperatureRecorder *const me);
State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_wait4Running(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_running(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_recording(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_idle(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_report(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_reset(TemperatureRecorder *const me, Event const *const e);
typedef State (*TemperatureRecorderAction)(TemperatureRecorder *const me, Event const *const e);

#endif