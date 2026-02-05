/*
 * canCommunication.c
 *
 *  Created on: Feb 5, 2026
 *      Author: mateo
 */

#include "canCommunication.h"
#include "fdcan.h"
#include "string.h"

static volatile uint16_t myID = MASTER_ID;

static uint32_t FDCAN_BytesToDLC(uint8_t len);
static uint8_t devicesNumber = 0;

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

    /* Wait until TX FIFO has free space */
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0);

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

	FDCAN_Send(myID, (uint8_t *)&message, RECOGNITION_ACTION_SIZE);
}

void recognitionActionRx(uint16_t id, RecognitionActionStructureRx *data)
{
	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[0].deviceID == id)
		{
			//TODO write two devices error excepction
		}
	}

	canDevices[devicesNumber].deviceID = id;
	canDevices[devicesNumber].deviceConnected = TRUE;
	canDevices[devicesNumber].deviceAlive = TRUE;
}

void heartBitEmergencyActionTx(void)
{
	HeartBitEmergencyActionStructureTx message;
	message.actionID = HEART_BIT_ACTION;

	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceAlive == FALSE)
		{
			message.deviceID = canDevices[i].deviceID;
			FDCAN_Send(myID, (uint8_t *) &message, HEARTBIT_ACTION_STRUCTURE_LEN);
		}
	}
}

void heartBitActionRx(uint16_t id, HeartBitActionStructureRx *data, uint8_t len)
{
	for(uint8_t i = 0; i < devicesNumber; i++)
	{
		if(canDevices[i].deviceID == id)
		{
			canDevices[i].deviceConnected = TRUE;
			canDevices[i].deviceAlive = TRUE;
			//TODO add heartbit emergency timer restart
			return;
		}
	}
}

void parameterSetActionTx(uint16_t parameterID, uint16_t deviceID, uint16_t value)
{
	ParameterActionSetStructureTx message;
	message.actionID = PARAMETR_SET_ACTION;
	message.deviceID = deviceID;
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
	message.deviceID = deviceID;
	message.parameterID = parameterID;

	FDCAN_Send(myID, (uint8_t *) &message, PARAMETER_ACTION_GET_TX_LEN);

}

void parameterGetActionRx(uint16_t id, ParameterActionGetStructureRx *data, uint8_t len)
{
	//TODO add parameter set value
}
