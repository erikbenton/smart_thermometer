#ifndef _TEMPERATURE_RECORDER_H_
#define _TEMPERATURE_RECORDER_H_

#include "active_object.h"

#define RECORDING_TIME_PERIOD_MS 500

typedef struct TemperatureRecorder
{
    Active super;
    TimeEvent timeEvt;
    float current_temp_celsius;
} TemperatureRecorder;

typedef struct ReportTemperatureEvent
{
    Event super;
    void (*report_cb)(TemperatureRecorder *const me);
} ReportTemperatureEvent;

void TemperatureRecorder_ctor(TemperatureRecorder *const me);
State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_wait4Start(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_recording(TemperatureRecorder *const me, Event const *const e);
typedef State (*TemperatureRecorderAction)(TemperatureRecorder *const me, Event const *const e);

#endif