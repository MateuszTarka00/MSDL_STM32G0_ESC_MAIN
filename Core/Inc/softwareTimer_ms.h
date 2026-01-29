/*
 * softwareTimer_ms.h
 *
 *  Created on: 13 paź 2025
 *      Author: mateo
 */

#ifndef INC_SOFTWARETIMER_MS_H_
#define INC_SOFTWARETIMER_MS_H_

#include "main.h"

typedef struct
{
	uint32_t period;
	void (*callback)(void *);
	uint8_t start;
	uint8_t repeat;
	uint32_t ticks;
	void * param;
}SoftwareTimerHandler;

void initSoftwareTimer(SoftwareTimerHandler * timer, uint32_t period, void * callback, uint8_t repeat, void *param);
void deInitSoftwareTimer(SoftwareTimerHandler * timer);
void startSoftwareTimer(SoftwareTimerHandler * timer);
void stopSoftwareTimer(SoftwareTimerHandler * timer);
void timersHandler(void);

#endif /* INC_SOFTWARETIMER_MS_H_ */
