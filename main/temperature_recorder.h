#ifndef _TEMPERATURE_RECORDER_H_
#define _TEMPERATURE_RECORDER_H_

#include "active_object.h"

#define RECORDING_TIME_PERIOD_MS 200

typedef struct TemperatureRecorder
{
    Active super;
    TimeEvent timeEvt;
    float filtered_temp_celsius;
    float raw_temp_celsius;
    float filter_alpha;
    bool is_filter_started;
} TemperatureRecorder;

typedef void (*report_temperature_cb)(TemperatureRecorder *const me);

typedef struct ReportTemperatureEvent
{
    Event super;
    report_temperature_cb report_cb;
} ReportTemperatureEvent;

void TemperatureRecorder_ctor(TemperatureRecorder *const me);
State TemperatureRecorder_initial(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_wait4Start(TemperatureRecorder *const me, Event const *const e);
State TemperatureRecorder_recording(TemperatureRecorder *const me, Event const *const e);
typedef State (*TemperatureRecorderAction)(TemperatureRecorder *const me, Event const *const e);

#endif