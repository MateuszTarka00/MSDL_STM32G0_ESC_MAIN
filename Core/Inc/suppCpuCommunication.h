/*
 * suppCpuCommunication.h
 *
 *  Created on: 27 lut 2026
 *      Author: mateo
 */

#ifndef INC_SUPPCPUCOMMUNICATION_H_
#define INC_SUPPCPUCOMMUNICATION_H_

#include "main.h"

void setFactoryReset(void);
void setTeachFast(bool onOff);
bool getSpeedReady(void);
bool getRotationState(void);
bool getStandState(void);
bool getSafetyState(void);
void setSlowSpeed(bool onOff);
void setFastSpeed(bool onOff);
void setTeachOutput(bool onOff);
#endif /* INC_SUPPCPUCOMMUNICATION_H_ */
