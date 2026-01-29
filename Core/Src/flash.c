/*
 * flash.c
 *
 *  Created on: 23 paź 2025
 *      Author: mateo
 */

#include "flash.h"
#include <string.h>

#define FLASH_BASE_ADDR     0x08000000U

Flash_RotationsPerMinute flash_RotationsPerMinute = {};

// Calculate page start address
uint32_t Flash_GetPageAddress(uint32_t pageIndex)
{
    return FLASH_BASE_ADDR + (pageIndex * FLASH_PAGE_SIZE);
}

void Flash_ErasePage(uint32_t pageIndex)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInit = {0};
    uint32_t pageError = 0;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Page = pageIndex;
    eraseInit.NbPages = 1;

    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        // Handle error
        HAL_FLASH_Lock();
        return;
    }

    HAL_FLASH_Lock();
}

void Flash_WriteStruct(uint32_t pageIndex, const Flash_RotationsPerMinute *data)
{
    HAL_FLASH_Unlock();

    uint32_t address = Flash_GetPageAddress(pageIndex);
    const uint8_t *src = (const uint8_t *)data;

    for (uint32_t i = 0; i < sizeof(Flash_RotationsPerMinute); i += 8) {
        uint64_t dword = 0xFFFFFFFFFFFFFFFFULL;
        memcpy(&dword, src + i, (sizeof(Flash_RotationsPerMinute) - i >= 8) ? 8 : (sizeof(Flash_RotationsPerMinute) - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + i, dword) != HAL_OK) {
            // Handle error
            break;
        }
    }

    HAL_FLASH_Lock();
}

void Flash_ReadStruct(uint32_t pageIndex, Flash_RotationsPerMinute *data)
{
    memcpy(data, (const void *)Flash_GetPageAddress(pageIndex), sizeof(Flash_RotationsPerMinute));
}

uint8_t checkStructEmpty(const Flash_RotationsPerMinute *data)
{
	const uint8_t *src = (const uint8_t *)data;

	for(uint16_t i = 0; i < sizeof(*data); ++i)
	{
		if(*src != 0xFF)
		{
			return 0;
		}

		src++;
	}
	return 1;
}



