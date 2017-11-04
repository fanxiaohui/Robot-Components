/*
 * scheduler.h
 *
 * Created: 10/27/2017 6:42:07 PM
 *  Author: Phoenix Ars Technica
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "timer.h"

#define DEACTIVATE		0
#define MAX_NO_OF_TASKS 10
typedef void (*pf_TaskHandler)(void);
typedef u8 taskIdType;

typedef struct task_struct_t
{
	u16 timerValue;
	pf_TaskHandler pf_task;
}task_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/
extern void scheduler_init(timer_struct_t s_Timer, timer_interruptType_enum_t interrupt);
extern void activate_task(u8 taskId,u16 task_timer);
extern void deactivate_task(u8 taskId);
extern void scheduler (void);
extern void create_task (taskIdType taskId, pf_TaskHandler pf_taskH );
extern void destroy_task (taskIdType taskId);
#endif /* SCHEDULER_H_ */