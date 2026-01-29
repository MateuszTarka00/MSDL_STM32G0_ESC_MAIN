/*
 * flash.h
 *
 *  Created on: 23 paź 2025
 *      Author: mateo
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

typedef struct
{
	uint32_t engine;
	uint32_t handrail;
	uint32_t step;
}Flash_RotationsPerMinute;

extern Flash_RotationsPerMinute flash_RotationsPerMinute;

void Flash_ErasePage(uint32_t pageIndex);
void Flash_WriteStruct(uint32_t pageIndex, const Flash_RotationsPerMinute *data);
void Flash_ReadStruct(uint32_t pageIndex, Flash_RotationsPerMinute *data);
uint32_t Flash_GetPageAddress(uint32_t pageIndex);
uint8_t checkStructEmpty(const Flash_RotationsPerMinute *data);


#endif /* INC_FLASH_H_ */
