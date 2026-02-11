/*
 * settingsForm.h
 *
 *  Created on: 10 lut 2026
 *      Author: mateo
 */

#ifndef MENUS_SETTINGSFORM_H_
#define MENUS_SETTINGSFORM_H_

#include "main.h"

#define NUMBER_OF_TIMERS	6

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
}MenuFormat;

extern MenuFormat *currentMenu;
extern MenuFormat settingsMenu;

void menuEnterFunction(void);
void goToNextItem(void);
void goToPreviousItem(void);
void callItemFunction(void);
void backToParentMenu(void);


#endif /* MENUS_SETTINGSFORM_H_ */
