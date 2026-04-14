/*
 * logs.h
 *
 *  Created on: Mar 17, 2026
 *      Author: mtarka
 */

#ifndef INC_LOGS_H_
#define INC_LOGS_H_

#include "main.h"

#define MAXIMUM_LOGS_NUMBER	10

extern uint8_t actualLogs[MAXIMUM_LOGS_NUMBER];

void addLog(uint8_t log);
void clearLogs(void);
void Flash_WriteLogs(uint32_t pageIndex, uint8_t *data);
void saveLogs(void);
void initializeLogs(void);

#endif /* INC_LOGS_H_ */
