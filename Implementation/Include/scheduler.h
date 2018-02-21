/*
 * scheduler.h
 *
 * Created: 10/27/2017 6:42:07 PM
 *  Author: Phoenix Ars Technica
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "timer.h"

typedef struct task_struct_t
{
	u16 period;
	void (*function)(void);
}task_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void scheduler_init(timer_struct_t s_timer);
void scheduler_start();
void scheduler_stop();
void scheduler_createTask(task_struct_t s_task);
void scheduler_destroyTask(task_struct_t s_task);
void scheduler_enableTask(task_struct_t s_task);
void scheduler_disableTask(task_struct_t s_task);
void scheduler_loop();

#endif /* SCHEDULER_H_ */