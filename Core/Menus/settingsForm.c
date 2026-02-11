/*
 * settingsForm.c
 *
 *  Created on: 10 lut 2026
 *      Author: mateo
 */

#include "settingsForm.h"
#include "fonts.h"
#include "st7789.h"
#include "string.h"

#define HEADER_TO_FIRST_ITEM_PIXELS	41
#define ITEM_TO_ITEM_PIXELS			7

void enterSubMenu(void *param)
{
	currentMenu = param;
	ST7789_Fill_Color(WHITE);
	currentMenu->currentItem = 0;
	menuEnterFunction();
}

void callItemFunction(void)
{
	if(currentMenu->menuItems[currentMenu->currentItem].menuFunction != 0)
	{
		currentMenu->menuItems[currentMenu->currentItem].menuFunction(currentMenu->menuItems[currentMenu->currentItem].param);
	}
}

void backToParentMenu(void)
{
	if(currentMenu->parentMenu != 0)
	{
		currentMenu = currentMenu->parentMenu;
		ST7789_Fill_Color(WHITE);
		menuEnterFunction();
	}
}

MenuItem timesParameterMenuItems[] =
{
    { .name = "Czas luzownika",				.menuFunction = 0, .param = 0 },
    { .name = "Czas lancucha",	  			.menuFunction = 0, .param = 0 },
    { .name = "Czas stycznikow",   			.menuFunction = 0, .param = 0 },
    { .name = "Czas pracy szybko",	  		.menuFunction = 0, .param = 0 },
    { .name = "Czas pracy wolno",       	.menuFunction = 0, .param = 0 },
    { .name = "Czas gwiazda/trojkat",      	.menuFunction = 0, .param = 0 },
};

MenuFormat timesParameterMenu =
{
	.header = "Parametry czas",
	.menuItems = timesParameterMenuItems,   // ← pointer to first MenuItem
	.itemsNumber = sizeof(timesParameterMenuItems) / sizeof(timesParameterMenuItems[0]),
	.currentItem = 0,
	.parentMenu = &settingsMenu
};

MenuItem settingsMenuItems[] =
{
    { .name = "Parametry czas"	,			.menuFunction = enterSubMenu, .param = &timesParameterMenu},
    { .name = "Kontrola",	  				.menuFunction = 0, .param = 0 },
    { .name = "Sterowanie",        			.menuFunction = 0, .param = 0 },
    { .name = "Dziennik zdarzen",	  		.menuFunction = 0, .param = 0 },
    { .name = "Jezyk",       				.menuFunction = 0, .param = 0 },
    { .name = "Ustawienia fabryczne",      	.menuFunction = 0, .param = 0 },
};

MenuFormat settingsMenu =
{
    .header = "Ustawienia",
    .menuItems = settingsMenuItems,   // ← pointer to first MenuItem
    .itemsNumber = sizeof(settingsMenuItems) / sizeof(settingsMenuItems[0]),
    .currentItem = 0,
	.parentMenu = 0
};


MenuFormat *currentMenu = &settingsMenu;

uint16_t centerString(FontDef font, const char *str)
{
	//size of string in pixels
	uint16_t Xstart = font.width * strlen(str);

	if(Xstart > ST7789_WIDTH)
	{
		return 0;
	}

	Xstart = ST7789_WIDTH - Xstart; //number of empty pixels
	Xstart /= 2; //devided by 2 to get x start to center a string

	return Xstart;
}

uint16_t calculateItemStartHeight(uint8_t itemNumber, FontDef font)
{
	return (itemNumber * (font.height + ITEM_TO_ITEM_PIXELS)) + HEADER_TO_FIRST_ITEM_PIXELS;
}

void goToNextItem(void)
{
	if(currentMenu->currentItem == (currentMenu->itemsNumber - 1))
	{
		return;
	}

	ST7789_WriteString(2, calculateItemStartHeight(currentMenu->currentItem, Font_11x18), currentMenu->menuItems[currentMenu->currentItem].name, Font_11x18, BLACK, WHITE);
	currentMenu->currentItem++;
	ST7789_WriteString(2, calculateItemStartHeight(currentMenu->currentItem, Font_11x18), currentMenu->menuItems[currentMenu->currentItem].name, Font_11x18, WHITE, BLACK);
}

void goToPreviousItem(void)
{
	if(currentMenu->currentItem == 0)
	{
		return;
	}

	ST7789_WriteString(2, calculateItemStartHeight(currentMenu->currentItem, Font_11x18), currentMenu->menuItems[currentMenu->currentItem].name, Font_11x18, BLACK, WHITE);
	currentMenu->currentItem--;
	ST7789_WriteString(2, calculateItemStartHeight(currentMenu->currentItem, Font_11x18), currentMenu->menuItems[currentMenu->currentItem].name, Font_11x18, WHITE, BLACK);
}

void menuEnterFunction(void)
{
	ST7789_WriteString(centerString(Font_16x26, currentMenu->header), 5, currentMenu->header, Font_16x26, BLACK, WHITE);

	for(uint8_t i = 0; i < currentMenu->itemsNumber; i++)
	{
		if(i == currentMenu->currentItem)
		{
			ST7789_WriteString(2, calculateItemStartHeight(i, Font_11x18), currentMenu->menuItems[i].name, Font_11x18, WHITE, BLACK);
		}
		else
		{
			ST7789_WriteString(2, calculateItemStartHeight(i, Font_11x18), currentMenu->menuItems[i].name, Font_11x18, BLACK, WHITE);
		}
	}
}

void settingsMenuTask(void)
{

}
