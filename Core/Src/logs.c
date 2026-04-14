/*
 * logs.c
 *
 *  Created on: Mar 17, 2026
 *      Author: mtarka
 */

#include "logs.h"
#include "flash.h"
#include "string.h"

#define LOGS_FLASH_PAGE 			254
#define FLASH_DATA_SIZE 			MAXIMUM_LOGS_NUMBER

uint8_t actualLogs[MAXIMUM_LOGS_NUMBER];

void addLog(uint8_t log)
{
	for (int i = MAXIMUM_LOGS_NUMBER - 1; i > 0; i--)
	{
	    actualLogs[i] = actualLogs[i - 1];
	}
	actualLogs[0] = log;
	Flash_ErasePage(LOGS_FLASH_PAGE);
	Flash_WriteLogs(LOGS_FLASH_PAGE, actualLogs);
}

void clearLogs(void)
{
	Flash_ErasePage(LOGS_FLASH_PAGE);
	memset(actualLogs, 0xFF, MAXIMUM_LOGS_NUMBER);
}

void Flash_WriteLogs(uint32_t pageIndex, uint8_t *data)
{
    HAL_FLASH_Unlock();

    uint32_t address = Flash_GetPageAddress(pageIndex);
    const uint8_t *src = (const uint8_t *)data;

    for (uint32_t i = 0; i < FLASH_DATA_SIZE; i += 8) {
        uint64_t dword = 0xFFFFFFFFFFFFFFFFULL;
        memcpy(&dword, src + i, (FLASH_DATA_SIZE - i >= 8) ? 8 : (FLASH_DATA_SIZE - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + i, dword) != HAL_OK) {
            // Handle error
            break;
        }
    }

    HAL_FLASH_Lock();
}

void initializeLogs(void)
{
	uint8_t *Flash_actualLogs =
	        (uint8_t*)Flash_GetPageAddress(LOGS_FLASH_PAGE);


    memcpy(actualLogs,
    		Flash_actualLogs,
			MAXIMUM_LOGS_NUMBER);
}
