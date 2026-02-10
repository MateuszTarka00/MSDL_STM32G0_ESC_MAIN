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

MenuItem settingsMenuItems[] =
{
    { .name = "Czas trybu szybkiego",		.menuFunction = 0, .param = 0 },
    { .name = "Czas rozpedu",  				.menuFunction = 0, .param = 0 },
    { .name = "Kierunek ruchu",        		.menuFunction = 0, .param = 0 },
    { .name = "Tolerancja obrotow",  .menuFunction = 0, .param = 0 },
    { .name = "Jezyk",       				.menuFunction = 0, .param = 0 },
    { .name = "Ustawienia fabryczne",      		.menuFunction = 0, .param = 0 },
};

SettingsMenu settingsMenu =
{
    .header = "Ustawienia",
    .menuItems = settingsMenuItems,   // ← pointer to first MenuItem
    .itemsNumber = sizeof(settingsMenuItems) / sizeof(settingsMenuItems[0]),
    .currentItem = 0
};

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
	if(settingsMenu.currentItem == (settingsMenu.itemsNumber - 1))
	{
		return;
	}

	ST7789_WriteString(2, calculateItemStartHeight(settingsMenu.currentItem, Font_11x18), settingsMenu.menuItems[settingsMenu.currentItem].name, Font_11x18, BLACK, WHITE);
	settingsMenu.currentItem++;
	ST7789_WriteString(2, calculateItemStartHeight(settingsMenu.currentItem, Font_11x18), settingsMenu.menuItems[settingsMenu.currentItem].name, Font_11x18, WHITE, BLACK);
}

void goToPreviousItem(void)
{
	if(settingsMenu.currentItem == 0)
	{
		return;
	}

	ST7789_WriteString(2, calculateItemStartHeight(settingsMenu.currentItem, Font_11x18), settingsMenu.menuItems[settingsMenu.currentItem].name, Font_11x18, BLACK, WHITE);
	settingsMenu.currentItem--;
	ST7789_WriteString(2, calculateItemStartHeight(settingsMenu.currentItem, Font_11x18), settingsMenu.menuItems[settingsMenu.currentItem].name, Font_11x18, WHITE, BLACK);
}

void settingsEnterFunction(void)
{
	ST7789_WriteString(centerString(Font_16x26, settingsMenu.header), 5, settingsMenu.header, Font_16x26, BLACK, WHITE);
	ST7789_WriteString(2, calculateItemStartHeight(0, Font_11x18), settingsMenu.menuItems[0].name, Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, calculateItemStartHeight(1, Font_11x18), settingsMenu.menuItems[1].name, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, calculateItemStartHeight(2, Font_11x18), settingsMenu.menuItems[2].name, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, calculateItemStartHeight(3, Font_11x18), settingsMenu.menuItems[3].name, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, calculateItemStartHeight(4, Font_11x18), settingsMenu.menuItems[4].name, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, calculateItemStartHeight(5, Font_11x18), settingsMenu.menuItems[5].name, Font_11x18, BLACK, WHITE);
}

void settingsMenuTask(void)
{

}
