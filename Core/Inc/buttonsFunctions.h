/*
 * buttonsFunctions.h
 *
 *  Created on: 9 lut 2026
 *      Author: mateo
 */

#ifndef INC_BUTTONSFUNCTIONS_H_
#define INC_BUTTONSFUNCTIONS_H_

#include "main.h"
#include "FreeRTOS.h"

#define DEBOUNCE_TIME_MS 20

typedef struct
{
    TickType_t lastTick;
    uint8_t pendingPress;
    uint8_t pendingRelease;
} ButtonDebounce_t;

extern void (*upButtonFunction)(void *);
extern void (*downButtonFunction)(void *);
extern void (*okButtonFunction)(void *);
extern void (*escButtonFunction)(void *);

void buttonsSubTask(void);

#endif /* INC_BUTTONSFUNCTIONS_H_ */
