/*
 * canCommunication.h
 *
 *  Created on: Feb 5, 2026
 *      Author: mateo
 */

#ifndef INC_CANCOMMUNICATION_H_
#define INC_CANCOMMUNICATION_H_

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#define MASTER_ID						0x00
#define BOTTOM_CONTROL_BOARD_ID			0x01

//action list
#define RECOGNITION_ACTION				0x0000
#define HEART_BIT_ACTION				0x0001
#define PARAMETR_SET_ACTION				0x0002
#define PARAMETR_GET_ACTION				0x0003

#define BOTTOM_SENSOR_EVENT				0x0000
#define BOTTOM_SAFETY_CIRCUIT_EVENT		0x0001

#define RECOGNITION_ACTION_SIZE			0x03

#define DEVICES_MAX_NUMBER				16

#define HEARTBIT_ACTION_STRUCTURE_LEN 	4
#define PARAMETER_ACTION_GET_TX_LEN		6
#define PARAMETER_ACTION_GET_RX_LEN		8
#define PARAMETER_ACTION_SET_TX_LEN		8
#define PARAMETER_ACTION_SET_RX_LEN		8

#define REQEST_QUEUE_SIZE  				8
#define PARAMETER_TIMEOUT_MS 			100

#define HEARTBIT_TIMER_LOST_TIME_MS			2000
#define HEARTBIT_TIMER_EMERGANCY_TIME_MS	100

#define ACTION_EVENT_BIT					0
#define RQ_RS_BIT							1

#define ACTION								1
#define EVENT								0

#define REQUEST								0
#define RESPONSE							1

#define ALL_OBJECTS_ID						255
#define HEARTBIT_PERIOD_MS					1000

#define LAST_HEARTBEAT_TICK					3000

extern QueueHandle_t canRxQueue;

typedef struct
{
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
} CAN_Message_t;

typedef struct{
	uint16_t deviceID;
	bool deviceAlive;
	uint32_t lastHeartBeat;
}CanDevice;

typedef struct{
	uint16_t actionID;
	uint8_t  destination;
}RecognitionActionStructureTx;

typedef struct{
	uint16_t actionID;
	uint8_t  destination;
}HeartBitEmergencyActionStructureTx;

typedef struct{
	uint16_t actionID;
	uint8_t  destination;
}HeartBitActionStructureRx;

typedef struct{
	uint16_t actionID;
	uint16_t parameterID;
	uint16_t value;
	uint8_t  destination;
}ParameterActionSetStructureTx;

typedef struct{
	uint16_t actionID;
	uint16_t parameterID;
	uint16_t ack;
	uint8_t  destination;
}ParameterActionSetStructureRx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
	uint8_t  destination;
}ParameterActionGetStructureTx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
	uint16_t value;
	uint8_t  destination;
}ParameterActionGetStructureRx;

typedef struct
{
    uint8_t  nodeId;
    uint16_t parameterId;
    uint32_t timestamp;
} CanRequestStructure; //todo create parameters structures

void CAN_InitRTOS(void);
void FDCAN_Send(uint16_t id, uint8_t *data, uint8_t len);
void recognitionActionTx(void);
void recognitionActionRx(uint16_t id);
void heartBitEmergencyActionTx(void);
void heartBitActionRx(uint16_t id, HeartBitActionStructureRx *data, uint8_t len);
void parameterSetActionTx(uint16_t parameterID, uint16_t deviceID, uint16_t value);
void parameterSetActionRx(uint16_t id, ParameterActionSetStructureRx *data, uint8_t len);
void parameterGetActionTx(uint16_t parameterID, uint16_t deviceID);
void parameterGetActionRx(uint16_t id, ParameterActionGetStructureRx *data, uint8_t len);
void initHeartBitTimers(void);
void heartBitSubTask(void);
bool getBootomBoardAlive(void);
void processMessage(CAN_Message_t *msg);
bool getHumanDown(void);
bool getSafetyCircuitBottom(void);


#endif /* INC_CANCOMMUNICATION_H_ */
