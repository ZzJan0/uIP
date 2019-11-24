#ifndef _TASKS_H
#define _TASKS_H

#include "main.h"

typedef struct _Task_St
{
	uint8_t En;
	uint8_t Run;
	uint8_t Load;
	uint16_t Timer;
	uint16_t ltvTime;
	void (*Task_Hook)(void);
	
}Task_St;

typedef enum _TASK_LIST
{
	TIME_BASE_10MS,
	TIME_BASE_100MS,
	TIME_BASE_1S,
	
	TASK_MAX
}TASK_LIST;

extern Task_St G_Task_St[TASK_MAX];

void Task_Remarks(void);
void Task_Process(void);
void Time_Base_10ms(void);
void Time_Base_100ms(void);
void Time_Base_1s(void);

#endif

