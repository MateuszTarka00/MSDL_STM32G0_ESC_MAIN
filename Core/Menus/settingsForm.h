/*
 * settingsForm.h
 *
 *  Created on: 10 lut 2026
 *      Author: mateo
 */

#ifndef MENUS_SETTINGSFORM_H_
#define MENUS_SETTINGSFORM_H_

#include "main.h"
#include "parameters.h"

#define NUMBER_OF_TIMERS	6

typedef enum
{
	SCROLL_MENU,
	PARAMETER_ADJUSTMENT,
}MenuType;

typedef struct
{
	char *name;
	void (*menuFunction)(void *param);
	void *param;
}MenuItem;

typedef struct
{
	char *header;
	MenuItem *menuItems;
	uint8_t itemsNumber;
	uint8_t currentItem;
	void *parentMenu;
	MenuType menuType;
}MenuFormat;

extern MenuFormat *currentSettingsMenu;
extern MenuFormat settingsMenu;
extern MenuFormat parametersAdjustmentMenu;
extern Parameter *currentParameter;

void enterSettingsMenu(void);

#endif /* MENUS_SETTINGSFORM_H_ */
