/*
 * scheduler.c
 *
 * Created: 10/1/2017 6:28:42 PM
 *  Author: Phoenix Ars Technica
 */ 
/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/
#include "scheduler.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

volatile bool timer_flag;
static u8 task_number;
static u16 task_timers[MAX_NO_OF_TASKS];
static task_struct_t task_table[MAX_NO_OF_TASKS];


/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/
void FlagUpdate ()
{
	 timer_flag =TRUE;
}
/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/
void scheduler_init(timer_struct_t s_Timer,timer_interruptType_enum_t interrupt)
{
	task_number = 0;
	timer_flag = FALSE;
	timer_init(s_Timer);
	timer_attachInterrupt(s_Timer,interrupt,FlagUpdate);
	timer_enableInterrupt(s_Timer,interrupt);
	timer_start(s_Timer);
}
void activate_task(u8 taskId,u16 task_timer)
{
	task_table[taskId].timerValue = task_timer;
	task_timers[taskId] =task_table[taskId].timerValue;
}
void deactivate_task(u8 taskId)
{
	task_timers[taskId] = DEACTIVATE;
}
void scheduler ()
{
	u8 task = 0u;
	if (timer_flag == TRUE)
	{
		timer_flag = FALSE;
		while(task < task_number)
		{
			if (task_timers[task] != 0)
			{
				task_timers[task] --;
				if (task_timers[task] == 0)
				{
					task_table[task].pf_task();
					task_timers[task]= task_table[task].timerValue;
				}
			}
			task++;
		}
	}
	else 
	{
		
	}
}
void create_task (taskIdType taskId, pf_TaskHandler pf_taskH )
{
	task_table[taskId].timerValue = DEACTIVATE;
	task_table[taskId].pf_task = pf_taskH;
	task_number++;
}
void destroy_task (taskIdType taskId)
{
	if (taskId < task_number - 1)
	{
		task_table[taskId].timerValue =  task_table[taskId + 1u].timerValue;
		task_table[taskId].pf_task = task_table[taskId +1u].pf_task;
	}
	
	task_number--;
}