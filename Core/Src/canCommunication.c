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

void recognitionActionTX(void)
{
	RecognitionActionTX recognitionActionTX;

	recognitionActionTX.actionID = RECOGNITION_ACTION;
	memset(recognitionActionTX.data, 0, sizeof(recognitionActionTX.data));

	FDCAN_Send(myID, recognitionActionTX.data, RECOGNITION_ACTION_SIZE);

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
