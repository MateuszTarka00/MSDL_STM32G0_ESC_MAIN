/*
 * flash.c
 *
 *  Created on: 23 paź 2025
 *      Author: mateo
 */

#include "flash.h"
#include "softwareReset.h"
#include <string.h>

#define FLASH_PAGE 			200
#define FLASH_BASE_ADDR     0x08000000U
#define FLASH_MAGIC         0xDEADBEEFUL
#define FLASH_DATA_SIZE \
    (sizeof(Flash_parametersToSave) - sizeof(uint32_t))

Flash_parametersToSave flash_parametersToSave = {};

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

void Flash_WriteStruct(uint32_t pageIndex, const Flash_parametersToSave *data)
{
    HAL_FLASH_Unlock();

    uint32_t address = Flash_GetPageAddress(pageIndex);
    const uint8_t *src = (const uint8_t *)data;

    for (uint32_t i = 0; i < sizeof(Flash_parametersToSave); i += 8) {
        uint64_t dword = 0xFFFFFFFFFFFFFFFFULL;
        memcpy(&dword, src + i, (sizeof(Flash_parametersToSave) - i >= 8) ? 8 : (sizeof(Flash_parametersToSave) - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + i, dword) != HAL_OK) {
            // Handle error
            break;
        }
    }

    HAL_FLASH_Lock();
}

void Flash_ReadStruct(uint32_t pageIndex, Flash_parametersToSave *data)
{
    memcpy(data, (const void *)Flash_GetPageAddress(pageIndex), sizeof(Flash_parametersToSave));
}

uint8_t checkStructEmpty(const Flash_parametersToSave *data)
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

uint32_t crc32_compute(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320UL;
            else
                crc >>= 1;
        }
    }

    return ~crc;
}

void flash_parametersSave(void)
{
	flash_parametersToSave.magic = FLASH_MAGIC;

	flash_parametersToSave.crc = crc32_compute(
                      (uint8_t*)&flash_parametersToSave,
                      FLASH_DATA_SIZE);

	Flash_ErasePage(FLASH_PAGE);
	Flash_WriteStruct(FLASH_PAGE, &flash_parametersToSave);
}

bool flash_loadParameters(void)
{
	Flash_parametersToSave *flash_ptr =
	        (Flash_parametersToSave*)Flash_GetPageAddress(FLASH_PAGE);

	if (flash_ptr->magic != FLASH_MAGIC)
	        return FALSE;

    uint32_t crc_calc = crc32_compute(
                            (uint8_t*)flash_ptr,
                            FLASH_DATA_SIZE);

    if (crc_calc != flash_ptr->crc)
        return 0;

    memcpy(&flash_parametersToSave,
           flash_ptr,
           sizeof(Flash_parametersToSave));

    return 1;
}

void flash_factoryReset(void)
{
	Flash_ErasePage(FLASH_PAGE);
	softwareReset();
}

