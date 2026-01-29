/*
 * softwareTimer_ms.c
 *
 *  Created on: 13 paź 2025
 *      Author: mateo
 */

#include "softwareTimer_ms.h"
#include <stdlib.h>

#define SOFTWARE_TIMER_LIST_CAPACITY	16

typedef struct SoftwareTimersObject
{
	SoftwareTimerHandler * timerHandler;
	struct SoftwareTimersObject * nextObject;

}SoftwareTimersObject;

SoftwareTimersObject *timersObjectsList = 0;

void initSoftwareTimer(SoftwareTimerHandler * timer, uint32_t period, void * callback, uint8_t repeat, void *param)
{
	SoftwareTimersObject *current = timersObjectsList;

    while (current != NULL)
    {
        if (current->timerHandler == timer)
        {
            return;
        }
        current = current->nextObject;
    }

	timer->callback = callback;
	timer->period = period;
	timer->repeat = repeat;
	timer->ticks = 0;
	timer->param = param;

	SoftwareTimersObject *timerObject = (SoftwareTimersObject *)malloc(sizeof(SoftwareTimersObject));
	timerObject->timerHandler = timer;
	timerObject->nextObject = 0;

	if(timersObjectsList == 0)
	{
		timersObjectsList = timerObject;
		timersObjectsList->nextObject = 0;
	}
	else
	{
		SoftwareTimersObject *temp = timersObjectsList;

		while(temp->nextObject != 0)
		{
			temp = temp->nextObject;
		}

		temp->nextObject = timerObject;
	}

}

void deInitSoftwareTimer(SoftwareTimerHandler * timer)
{
    if (timer == 0 || timersObjectsList == 0)
        return;

	timer->callback = 0;
	timer->period = 0;
	timer->repeat = 0;
	timer->ticks = 0;

	SoftwareTimersObject *temp = timersObjectsList;
	SoftwareTimersObject *prev = 0;

	while(temp != 0 && temp->timerHandler != timer)
	{
		prev = temp;
		temp = temp->nextObject;
	}

	if(temp == 0) return;

	if(prev == 0)
	{
		timersObjectsList = temp->nextObject;
	}
	else
	{
		prev->nextObject = temp->nextObject;
	}

	free(temp);

}

void startSoftwareTimer(SoftwareTimerHandler * timer)
{
	timer->start = 1;
	timer->ticks = 0;
}

void stopSoftwareTimer(SoftwareTimerHandler * timer)
{
	timer->start = 0;
	timer->ticks = 0;
}

void timersHandler(void)
{
	SoftwareTimersObject *temp = timersObjectsList;

	while(temp != 0)
	{
		if(temp->timerHandler->start)
		{
			temp->timerHandler->ticks++;

			if(temp->timerHandler->ticks == temp->timerHandler->period)
			{
//				((void (*)(void))temp->timerHandler->callback)(temp->timerHandler->param);
				temp->timerHandler->callback(temp->timerHandler->param);

				if(!temp->timerHandler->repeat)
				{
					temp->timerHandler->start = 0;
				}

				temp->timerHandler->ticks = 0;
			}
		}

		temp = temp->nextObject;
	}
}
