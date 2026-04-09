/*
 * canCommunication.c
 *
 *  Created on: Feb 5, 2026
 *      Author: mateo
 */

#include "canCommunication.h"
#include "fdcan.h"
#include "string.h"
#include "softwareTimer_ms.h"
#include "FreeRTOS.h"
#include "parameters.h"
#include "mainForm.h"

#define WAIT_FOR_MAPPING_MS	5000

static volatile uint8_t myID = MASTER_ID;

static uint32_t FDCAN_BytesToDLC(uint8_t len);
static uint8_t devicesNumber = 0;

static CanDevice canDevices[DEVICES_MAX_NUMBER];
static bool humanDownStairs = FALSE;
static uint8_t bottomSafetyCircuit = 0;

QueueHandle_t canRxQueue;

void CAN_InitRTOS(void)
{
    canRxQueue = xQueueCreate(16, sizeof(CAN_Message_t));
}

void FDCAN_Send(uint16_t id, uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_BytesToDLC(len);
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(1)); // yield CPU
    }

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data);
}

static uint32_t FDCAN_BytesToDLC(uint8_t len)
{
    switch (len)
    {
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        case 8: return FDCAN_DLC_BYTES_8;
        default: return FDCAN_DLC_BYTES_8; // safety fallback
    }
}

static uint8_t FDCAN_DLCToBytes(uint32_t dlc)
{
    switch (dlc)
    {
        case FDCAN_DLC_BYTES_0: return 0;
        case FDCAN_DLC_BYTES_1: return 1;
        case FDCAN_DLC_BYTES_2: return 2;
        case FDCAN_DLC_BYTES_3: return 3;
        case FDCAN_DLC_BYTES_4: return 4;
        case FDCAN_DLC_BYTES_5: return 5;
        case FDCAN_DLC_BYTES_6: return 6;
        case FDCAN_DLC_BYTES_7: return 7;
        case FDCAN_DLC_BYTES_8: return 8;
        default: return 8; // safety fallback
    }
}

//Send message to all modules to get acknowlage connected modules
void recognitionActionTx(void)
{
	RecognitionActionStructureTx message;
	message.actionID = RECOGNITION_ACTION;
	message.destination = ALL_OBJECTS_ID;

	uint16_t arbitration = (myID<<8) + (ACTION<<ACTION_EVENT_BIT) + (REQUEST<<RQ_RS_BIT);

	FDCAN_Send(arbitration, (uint8_t *)&message, RECOGNITION_ACTION_SIZE);
}

void errorEventTx(uint8_t value)
{
	eventStructureTx message;
	message.eventID = ERROR_EVENT << 8;
	message.value = value;
	message.destination = DISPLAY_ID;

	uint16_t arbitration = (myID<<8) + (EVENT<<ACTION_EVENT_BIT) + (REQUEST<<RQ_RS_BIT);

	FDCAN_Send(arbitration, (uint8_t *)&message, EVENT_SIZE);
}

void workTypeEventTx(uint8_t value)
{
	eventStructureTx message;
	message.eventID = WORK_TYPE_EVENT << 8;
	message.value = value;
	message.destination = DISPLAY_ID;

	uint16_t arbitration = (myID<<8) + (EVENT<<ACTION_EVENT_BIT) + (REQUEST<<RQ_RS_BIT);

	FDCAN_Send(arbitration, (uint8_t *)&message, EVENT_SIZE);
}

void recognitionActionRx(uint16_t id)
{
	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceID == id)
		{
			canDevices[i].deviceAlive = TRUE;
			canDevices[i].lastHeartBeat = xTaskGetTickCount();
			return;
		}
	}

	canDevices[devicesNumber].deviceID = id;
	canDevices[devicesNumber].lastHeartBeat = xTaskGetTickCount();
	canDevices[devicesNumber].deviceAlive = TRUE;
	devicesNumber++;
}

void processEvent(uint16_t eventId, uint8_t value)
{
	switch(eventId)
	{
	case BOTTOM_SENSOR_EVENT:
		humanDownStairs = value;
		break;
	case BOTTOM_SAFETY_CIRCUIT_EVENT:
		bottomSafetyCircuit = value;
		break;
	default:
		break;
	}
}

void heartBitEmergencyActionTx(void)
{
	HeartBitEmergencyActionStructureTx message;
	message.actionID = HEART_BIT_ACTION;

	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceAlive == FALSE)
		{
			message.destination = canDevices[i].deviceID;
			FDCAN_Send(myID, (uint8_t *) &message, HEARTBIT_ACTION_STRUCTURE_LEN);
		}
	}
}

void parameterSetActionTx(uint16_t parameterID, uint16_t deviceID, uint16_t value)
{
	ParameterActionSetStructureTx message;
	message.actionID = PARAMETR_SET_ACTION;
	message.destination = deviceID;
	message.parameterID = parameterID;
	message.value = value;

	FDCAN_Send(myID, (uint8_t *) &message, PARAMETER_ACTION_SET_TX_LEN);
}

void parameterSetActionRx(uint16_t id, ParameterActionSetStructureRx *data, uint8_t len)
{
	//todo add parameter ack set callback
}

void parameterGetActionTx(uint16_t parameterID, uint16_t deviceID)
{
	ParameterActionGetStructureTx message;
	message.actionID = PARAMETR_GET_ACTION;
	message.destination = deviceID;
	message.parameterID = parameterID;

	FDCAN_Send(myID, (uint8_t *) &message, PARAMETER_ACTION_GET_TX_LEN);

}

void parameterGetActionRx(uint16_t id, ParameterActionGetStructureRx *data, uint8_t len)
{
	//TODO add parameter set value
}

static void sendEmergencyHeartBitTimerHandler(void *param)
{
	HeartBitEmergencyActionStructureTx message;

	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceAlive == FALSE)
		{
			message.actionID = HEART_BIT_ACTION;
			message.destination = canDevices[i].deviceID;

			FDCAN_Send(myID, (uint8_t *) &message, HEARTBIT_ACTION_STRUCTURE_LEN);
		}
	}
}

void heartBitSubTask(void)
{
	static uint32_t ticksTmp = 0;
	uint32_t ticksNow = xTaskGetTickCount();

	if((ticksNow - HEARTBIT_PERIOD_MS) >= ticksTmp && ticksNow > HEARTBIT_PERIOD_MS)
	{
		recognitionActionTx();
		ticksTmp = xTaskGetTickCount();
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];
    CAN_Message_t msg;
    BaseType_t hpw = pdFALSE;

    HAL_FDCAN_GetRxMessage(
        hfdcan,
        FDCAN_RX_FIFO0,
        &rxHeader,
		msg.data
    );

    msg.id = rxHeader.Identifier;
    msg.len = rxHeader.DataLength >> 16;

    if(msg.data[msg.len - 1] == myID || msg.data[msg.len - 1] == ALL_OBJECTS_ID) //Check if it is for me
    {
        xQueueSendFromISR(
            canRxQueue,
            &msg,
			hpw
        );

        portYIELD_FROM_ISR(hpw);
    }
}

void checkHeartBeatStatusSubTask(void)
{
	uint32_t ticksNow = xTaskGetTickCount();
	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if((ticksNow - canDevices[i].lastHeartBeat) > LAST_HEARTBEAT_TICK)
		{
			canDevices[i].deviceAlive = FALSE;

			if(workWithoutBottom.value == FALSE)
			{
				addRemoveError(BOTTOM_DEAD, TRUE);
				openInformationForm();
			}
		}
	}

	if(!devicesNumber)
	{
		if(ticksNow > WAIT_FOR_MAPPING_MS)
		{
			addRemoveError(BOTTOM_DEAD, TRUE);
			openInformationForm();
		}
	}
}

bool getBootomBoardAlive(void)
{
	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceID == BOTTOM_CONTROL_BOARD_ID)
		{
			return canDevices[i].deviceAlive;
		}
	}

	return FALSE;
}

void processMessage(CAN_Message_t *msg)
{
	if(msg->id & (ACTION<<ACTION_EVENT_BIT))
	{
		if(msg->id & (RESPONSE<<RQ_RS_BIT))
		{
			//Response
			uint16_t actionID = (msg->data[0] << 8) + msg->data[1];
			switch(actionID)
			{
				case RECOGNITION_ACTION:
					uint8_t sourceID = msg->id >> 8;
					recognitionActionRx(sourceID);
					break;
				default:
					uint8_t dupa = 0;
					dupa++;
					break;

			}
		}
		else
		{
			//Request

		}
	}
	else
	{
		//Event
		uint16_t eventID = (msg->data[0] << 8) + msg->data[1];
		uint8_t value = msg->data[2];
		processEvent(eventID, value);
	}
}

bool getHumanDown(void)
{
	return humanDownStairs;
}

bool getSafetyCircuitBottom(void)
{
	return bottomSafetyCircuit;
}

static void setCanLedOK(bool onOFF)
{
	HAL_GPIO_WritePin(CAN_OK_GPIO_Port, CAN_OK_Pin, onOFF);
}

static void setCanLedFault(bool onOFF)
{
	HAL_GPIO_WritePin(CAN_FAULT_GPIO_Port, CAN_FAULT_Pin, onOFF);
}

void CAN_UpdateLEDs(void)
{
    static uint32_t lastBlink = 0;
    static uint8_t blinkState = 0;

    uint32_t now = HAL_GetTick();

    FDCAN_ProtocolStatusTypeDef status;
    HAL_FDCAN_GetProtocolStatus(&hfdcan2, &status);

    /* --- BUS OFF --- */
    if (status.BusOff)
    {
    	setCanLedOK(FALSE);
    	setCanLedFault(TRUE);
        return;
    }

    /* --- ERROR PASSIVE / WARNING --- */
    if (status.ErrorPassive || status.Warning)
    {
        if ((now - lastBlink) > 500)
        {
            lastBlink = now;
            blinkState ^= 1;
        }

        if (blinkState)
        {
        	setCanLedOK(TRUE);
        	setCanLedFault(TRUE);
        }
        else
        {
        	setCanLedOK(FALSE);
        	setCanLedFault(FALSE);
        }
        return;
    }

    /* --- ERROR ACTIVE (NORMAL OPERATION) --- */
    setCanLedFault(FALSE);

    /* Blink green if traffic present */
    for(uint8_t i = 0; i < devicesNumber; i++)
    {
    	if(canDevices[i].deviceAlive != TRUE)
    	{
    	    if ((now - lastBlink) > 200)
    	    {
    	        lastBlink = now;
    	        blinkState ^= 1;
    	    }

    	    if (blinkState)
    	    	setCanLedOK(TRUE);
    	    else
    	    	setCanLedOK(FALSE);

    	    return;
    	}

    }

    setCanLedOK(TRUE);

}


