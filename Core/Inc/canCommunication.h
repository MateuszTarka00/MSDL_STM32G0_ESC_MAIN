/*
 * canCommunication.h
 *
 *  Created on: Feb 5, 2026
 *      Author: mateo
 */

#ifndef INC_CANCOMMUNICATION_H_
#define INC_CANCOMMUNICATION_H_

#include "main.h"

#define MASTER_ID						0x00
#define BOTTOM_CONTROL_BOARD			0x01

//action list
#define RECOGNITION_ACTION				0x0000
#define HEART_BIT_ACTION				0x0001
#define PARAMETR_SET_ACTION				0x0002
#define PARAMETR_GET_ACTION				0x0003

#define RECOGNITION_ACTION_SIZE			0x02

#define DEVICES_MAX_NUMBER				16

#define HEARTBIT_ACTION_STRUCTURE_LEN 	4
#define PARAMETER_ACTION_GET_TX_LEN		6
#define PARAMETER_ACTION_GET_RX_LEN		8
#define PARAMETER_ACTION_SET_TX_LEN		8
#define PARAMETER_ACTION_SET_RX_LEN		8

#define REQEST_QUEUE_SIZE  				8
#define PARAMETER_TIMEOUT_MS 			100

typedef struct{
	uint16_t deviceID;
	bool deviceConnected;
	bool deviceAlive;
}CanDevice;

extern CanDevice canDevices[DEVICES_MAX_NUMBER];

typedef struct{
	uint16_t actionID;
}RecognitionActionStructureTx;

typedef struct{
	uint16_t actionID;
}RecognitionActionStructureRx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
}HeartBitEmergencyActionStructureTx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
}HeartBitActionStructureRx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
	uint16_t value;
}ParameterActionSetStructureTx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
	uint16_t ack;
}ParameterActionSetStructureRx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
}ParameterActionGetStructureTx;

typedef struct{
	uint16_t actionID;
	uint16_t deviceID;
	uint16_t parameterID;
	uint16_t value;
}ParameterActionGetStructureRx;

typedef struct
{
    uint8_t  nodeId;
    uint16_t parameterId;
    uint32_t timestamp;
} CanRequestStructure; //todo create parameters structures

void FDCAN_Send(uint16_t id, uint8_t *data, uint8_t len);
void recognitionActionTx(void);
void recognitionActionRx(uint16_t id, RecognitionActionStructureRx *data);
void heartBitEmergencyActionTx(void);
void heartBitActionRx(uint16_t id, HeartBitActionStructureRx *data, uint8_t len);
void parameterSetActionTx(uint16_t parameterID, uint16_t deviceID, uint16_t value);
void parameterSetActionRx(uint16_t id, ParameterActionSetStructureRx *data, uint8_t len);
void parameterGetActionTx(uint16_t parameterID, uint16_t deviceID);
void parameterGetActionRx(uint16_t id, ParameterActionGetStructureRx *data, uint8_t len);
void initHeartBitTimers(void);


#endif /* INC_CANCOMMUNICATION_H_ */
