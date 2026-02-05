/*
 * canCommunication.h
 *
 *  Created on: Feb 5, 2026
 *      Author: mateo
 */

#ifndef INC_CANCOMMUNICATION_H_
#define INC_CANCOMMUNICATION_H_

#include "main.h"

#define MASTER_ID				0x00
#define BOTTOM_CONTROL_BOARD	0x01

//action list
#define RECOGNITION_ACTION		0x0000
#define HEART_BIT_ACTION		0x0001

#define RECOGNITION_ACTION_SIZE	0x02

typedef struct{
	uint8_t deviceID;
	bool deviceConnected;
	bool deviceAlive;
}CanDevice;

typedef struct{
	uint16_t actionID;
	uint8_t data[6];
}RecognitionActionTX;

void FDCAN_Send(uint16_t id, uint8_t *data, uint8_t len);
void recognitionActionTX(void);


#endif /* INC_CANCOMMUNICATION_H_ */
