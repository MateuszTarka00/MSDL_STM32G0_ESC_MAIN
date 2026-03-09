/*
 * suppCpuCommunication.h
 *
 *  Created on: 27 lut 2026
 *      Author: mateo
 */

#ifndef INC_SUPPCPUCOMMUNICATION_H_
#define INC_SUPPCPUCOMMUNICATION_H_

#include "main.h"

extern bool cpu2Alive;

void setFactoryReset(void);
void setTeachFast(bool onOff);
void setTeachSlow(bool onOff);
bool getSpeedReady(void);
bool getRotationState(void);
bool getStandState(void);
bool getSafetyStateSupp(void);
void setSlowSpeed(bool onOff);
void setFastSpeed(bool onOff);
void setTeachOutput(bool onOff);
void setTeachingEnd(bool onOff);
bool getAckK2(void);
void setHardStop(bool onOff);
void checkCpu2Alive(void);
#endif /* INC_SUPPCPUCOMMUNICATION_H_ */
