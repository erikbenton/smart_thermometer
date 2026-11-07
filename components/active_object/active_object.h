#ifndef _ACTIVE_OBJECT_H_
#define _ACTIVE_OBJECT_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* ----------- Event Facilities ----------- */

typedef uint16_t Signal;

enum ReservedSignals
{
    INIT_SIG,  /* Initialization signal */
    ENTRY_SIG, /* Trigger for the entry action */
    EXIT_SIG,  /* Trigger for the exit action */
    USER_SIG   /* User defined signal */
};

typedef struct
{
    Signal sig; /* event signal */
} Event;

/* ----------- Hierarchical State Machine Facilities ----------- */

typedef struct Hsm Hsm;

typedef enum STATUS
{
    TRAN_STATUS,
    HANDLED_STATUS,
    IGNORED_STATUS,
    INIT_STATUS,
    SUPER_STATUS
} State;

typedef State (*StateHandler)(Hsm *const me, Event const *const e);

struct Hsm
{
    StateHandler state; /* the "state variable "*/
    StateHandler temp;  /* for handling super state access */
};

#define TRANSITION(target_) ((((Hsm *)me)->state) = (StateHandler)(target_), TRAN_STATUS)
#define SUPER(super_) ((((Hsm *)me)->temp) = (StateHandler)(super_), SUPER_STATUS)

void Hsm_ctor(Hsm *const me, StateHandler initial);
void Hsm_init(Hsm *const me, Event const *const e);
void Hsm_dispatch(Hsm *const me, Event const *const e);
State Hsm_top(Hsm *const me, Event const *const e);

/* ----------- Active Object Facilities ----------- */

typedef struct Active Active;

typedef void (*DispatchHandler)(Active *const me, Event const *const e);

struct Active
{
    Hsm super; /* finite state machine */

    TaskHandle_t thread;    /* private thread */
    StaticTask_t thread_cb; /* callback for the task */

    QueueHandle_t queue;    /* private queue */
    StaticQueue_t queue_cb; /* callback for the queue*/
};

void Active_ctor(Active *const me, StateHandler initial);
void Active_start(Active *const me,
                  const char *const object_name,
                  UBaseType_t priority,
                  Event **queue_storage,
                  size_t queue_length,
                  StackType_t *stack_storage,
                  size_t stack_depth);
void Active_post(Active *const me, Event const *const e);
void Active_postFromISR(Active *const me, Event const *const e, BaseType_t *pxHigherPriorityWasTaken);

/* ----------- Time Event Facilities ----------- */

typedef struct
{
    Event super;
    Active *act;
    uint32_t timeout;
    uint32_t interval;
} TimeEvent;

void TimeEvent_ctor(TimeEvent *const me, Signal sig, Active *act);
void TimeEvent_arm(TimeEvent *const me, uint32_t timeout, uint32_t interval);
void TimeEvent_disarm(TimeEvent *const me);

void TimeEvent_tick(void);

#endif