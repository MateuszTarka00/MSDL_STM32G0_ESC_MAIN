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
}Flash_RotationsPerSecond;

typedef struct
{
	uint32_t parameterLooserTime;
	uint32_t parameterEngineTime;
	uint32_t parameterContactorTime;
	uint32_t parameterFastTime;
	uint32_t parameterSlowTime;
	uint32_t parameterStarTriangleTime;
	uint8_t parameterEngineControl;
	uint8_t parameterAutoStop;
	uint8_t parameterReleasing;
	uint8_t parameterTrafficDirectionSignals;
	uint8_t parameterLightning;
}Flash_settingsValues;

typedef struct __attribute__((packed))
{
	uint32_t magic;
	Flash_settingsValues flash_settingsValues;
	Flash_RotationsPerSecond flash_RotationsPerMinuteSlow;
	Flash_RotationsPerSecond flash_RotationsPerMinuteFast;
	uint32_t crc;
}Flash_parametersToSave;

extern Flash_parametersToSave flash_parametersToSave;

void Flash_ErasePage(uint32_t pageIndex);
void Flash_WriteStruct(uint32_t pageIndex, const Flash_parametersToSave *data);
void Flash_ReadStruct(uint32_t pageIndex, Flash_parametersToSave *data);
uint32_t Flash_GetPageAddress(uint32_t pageIndex);
uint8_t checkStructEmpty(const Flash_parametersToSave *data);
uint32_t crc32_compute(const uint8_t *data, uint32_t length);
void flash_parametersSave(void);
bool flash_loadParameters(void);
void flash_factoryReset(void);

#endif /* INC_FLASH_H_ */
