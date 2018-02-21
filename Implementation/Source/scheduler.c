/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "debug.h"
#include "scheduler.h"
#include "scheduler_config.h"

/************************************************************************/
/* Internal defines, enums, structs, types                              */
/************************************************************************/

typedef struct internalTask_struct_t
{
	task_struct_t task;
	bool isActive;
	u16 countdown;
}internalTask_struct_t;

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

volatile u16 u16_milliseconds;
volatile bool b_millisecondPassed;
u8 u8_maxActiveTasks;
internalTask_struct_t task_table[SCHEDULER_MAX_NO_OF_TASKS];
timer_struct_t s_timer;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void scheduler_tick()
{
	 b_millisecondPassed = TRUE;
	 u16_milliseconds++;
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void scheduler_init(timer_struct_t s_schedulerTimer)
{
	u8_maxActiveTasks = 0;
	u16_milliseconds = 0;
	b_millisecondPassed = FALSE;
	
	s_timer.frequency = s_schedulerTimer.frequency;
	s_timer.peripheral = s_schedulerTimer.peripheral;
	
	timer_init(s_timer);
	timer_attachInterrupt(s_timer, OVERFLOW, scheduler_tick);
	timer_enableInterrupt(s_timer, OVERFLOW);
}

void scheduler_start()
{
	timer_start(s_timer);
}

void scheduler_stop()
{
	timer_stop(s_timer);
}

void scheduler_createTask(task_struct_t s_task)
{
	task_table[u8_maxActiveTasks].task.period = s_task.period;
	task_table[u8_maxActiveTasks].task.function = s_task.function;
	task_table[u8_maxActiveTasks].countdown = s_task.period;
	u8_maxActiveTasks++;
}

void scheduler_destroyTask(task_struct_t s_task)
{
	u8 i, j;
	for (i = 0; i < u8_maxActiveTasks; i++)
		if (task_table[i].task.function == s_task.function)
		{
			for (j = i; j < u8_maxActiveTasks - 1; j++)
			{
				task_table[j].task.period =  task_table[j + 1].task.period;
				task_table[j].task.function = task_table[j + 1].task.function;
			}
			u8_maxActiveTasks--;
			break;
		}
}

void scheduler_enableTask(task_struct_t s_task)
{
	u8 i;
	
	for (i = 0; i < u8_maxActiveTasks; i++)
		if (task_table[i].task.function == s_task.function)
		{
			task_table[i].isActive = TRUE;
			break;
		}
}

void scheduler_disableTask(task_struct_t s_task)
{
	u8 i;
	
	for (i = 0; i < u8_maxActiveTasks; i++)
		if (task_table[i].task.function == s_task.function)
		{
			task_table[i].isActive = FALSE;
			break;
		}
}

void scheduler_loop()
{
	u8 i;
	if (b_millisecondPassed == TRUE)
	{
		for (i = 0; i < u8_maxActiveTasks; i++)
		{
			task_table[i].countdown--;
			if (task_table[i].countdown == 0 && task_table[i].isActive)
			{
				task_table[i].task.function();
				task_table[i].countdown = task_table[i].task.period;
			}
		}
		b_millisecondPassed = FALSE;
	}
}