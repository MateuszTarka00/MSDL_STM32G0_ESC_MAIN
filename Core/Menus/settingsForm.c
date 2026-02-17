/*
 * settingsForm.c
 *
 *  Created on: 10 lut 2026
 *      Author: mateo
 */
//TODO poprawic wyswietlanie tytulow
#include "settingsForm.h"
#include "fonts.h"
#include "st7789.h"
#include "string.h"
#include "flash.h"

#define HEADER_TO_FIRST_ITEM_PIXELS	41
#define ITEM_TO_ITEM_PIXELS			7
#define PARAMETER_HEIGHT			153

#define TRIANGLE_WIDTH				20
#define TRIANGLE_HEIGHT				26
#define TRIANGLE_START_HEIGHT		10

#define PARAMETER_FONT_HEIGHT		26

#define DISPLAY_X_CENTER			(ST7789_WIDTH/2)

#define TRIANGLE_UP					1
#define TRIANGLE_DOWN				2

uint16_t centerString(FontDef font, const char *str);
uint16_t calculateItemStartHeight(uint8_t itemNumber, FontDef font);

volatile static uint8_t trianglesDrawed = 0;

void enterSubMenu(void *param)
{
	currentMenu = param;
	ST7789_Fill_Color(WHITE);
	currentMenu->currentItem = 0;

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

void goToParentMenu(void)
{
	currentMenu = currentMenu->parentMenu;
	ST7789_Fill_Color(WHITE);
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

void showParameterValue(void)
{
	char paramStr[20];

	if(currentParameter->value < currentParameter->maxValue && !(trianglesDrawed & TRIANGLE_UP))
	{
		trianglesDrawed |= TRIANGLE_UP;
		ST7789_DrawTriangle(DISPLAY_X_CENTER - TRIANGLE_WIDTH, PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT, DISPLAY_X_CENTER + TRIANGLE_WIDTH,
				PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT, DISPLAY_X_CENTER, PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT - TRIANGLE_HEIGHT, BLACK);
	}
	else if(currentParameter->value == currentParameter->maxValue && (trianglesDrawed & TRIANGLE_UP))
	{
		trianglesDrawed &= ~TRIANGLE_UP;
		ST7789_DrawTriangle(DISPLAY_X_CENTER - TRIANGLE_WIDTH, PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT, DISPLAY_X_CENTER + TRIANGLE_WIDTH,
				PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT, DISPLAY_X_CENTER, PARAMETER_HEIGHT - TRIANGLE_START_HEIGHT - TRIANGLE_HEIGHT, WHITE);
	}

	if(currentParameter->value > currentParameter->minValue && !(trianglesDrawed & TRIANGLE_DOWN))
	{
		trianglesDrawed |= TRIANGLE_DOWN;
		ST7789_DrawTriangle(DISPLAY_X_CENTER - TRIANGLE_WIDTH, PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + PARAMETER_FONT_HEIGHT, DISPLAY_X_CENTER + TRIANGLE_WIDTH,
				PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + PARAMETER_FONT_HEIGHT, DISPLAY_X_CENTER, PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + TRIANGLE_HEIGHT + PARAMETER_FONT_HEIGHT, BLACK);
	}
	else if(currentParameter->value == currentParameter->minValue && (trianglesDrawed & TRIANGLE_DOWN))
	{
		trianglesDrawed &= ~TRIANGLE_DOWN;
		ST7789_DrawTriangle(DISPLAY_X_CENTER - TRIANGLE_WIDTH, PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + PARAMETER_FONT_HEIGHT, DISPLAY_X_CENTER + TRIANGLE_WIDTH,
				PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + PARAMETER_FONT_HEIGHT, DISPLAY_X_CENTER, PARAMETER_HEIGHT + TRIANGLE_START_HEIGHT + TRIANGLE_HEIGHT + PARAMETER_FONT_HEIGHT, WHITE);
	}

	paramGetValueString(paramStr, currentParameter);
	ST7789_WriteString(centerString(Font_16x26, paramStr), PARAMETER_HEIGHT, paramStr, Font_16x26, BLACK, WHITE);
}

void enterParameterMenu(void *param)
{
	currentParameter = param;
	parametersAdjustmentMenu.header = currentMenu->menuItems[currentMenu->currentItem].name;
	parametersAdjustmentMenu.parentMenu = currentMenu;

	currentMenu = &parametersAdjustmentMenu;

	trianglesDrawed = 0;

	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(centerString(Font_16x26, currentMenu->header), 5, currentMenu->header, Font_16x26, BLACK, WHITE);

	showParameterValue();
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
		ST7789_Fill_Color(WHITE);
		goToParentMenu();
	}
}

MenuFormat parametersAdjustmentMenu =
{
	.menuType = PARAMETER_ADJUSTMENT,
};


MenuItem timesParameterMenuItems[] =
{
    { .name = "Czas luzownika",				.menuFunction = enterParameterMenu, .param = &parameterLooserTime},
    { .name = "Czas lancucha",	  			.menuFunction = enterParameterMenu, .param = &parameterEngineTime},
    { .name = "Czas stycznikow",   			.menuFunction = enterParameterMenu, .param = &parameterContactorTime},
    { .name = "Czas pracy szybko",	  		.menuFunction = enterParameterMenu, .param = &parameterFastTime},
    { .name = "Czas pracy wolno",       	.menuFunction = enterParameterMenu, .param = &parameterSlowTime},
    { .name = "Czas gwiazda/trojkat",      	.menuFunction = enterParameterMenu, .param = &parameterStarTriangleTime},
};

MenuFormat timesParameterMenu =
{
	.header = "Parametry czas",
	.menuItems = timesParameterMenuItems,   // ← pointer to first MenuItem
	.itemsNumber = sizeof(timesParameterMenuItems) / sizeof(timesParameterMenuItems[0]),
	.currentItem = 0,
	.parentMenu = &settingsMenu,
	.menuType = SCROLL_MENU,
};

MenuItem monitoringParameterMenuItems[] =
{
    { .name = "Kontrola lancucha",				.menuFunction = enterParameterMenu, .param = &parameterEngineControl },
};

MenuFormat monitoringParameterMenu =
{
	.header = "Kontrola",
	.menuItems = monitoringParameterMenuItems,   // ← pointer to first MenuItem
	.itemsNumber = sizeof(monitoringParameterMenuItems) / sizeof(monitoringParameterMenuItems[0]),
	.currentItem = 0,
	.parentMenu = &settingsMenu,
	.menuType = SCROLL_MENU,
};

MenuItem controlParameterMenuItems[] =
{
    { .name = "Auto Stop",					.menuFunction = enterParameterMenu, .param = &parameterAutoStop },
	{ .name = "Zwalnianie",					.menuFunction = enterParameterMenu, .param = &parameterReleasing },
	{ .name = "Syg. Kierunku",				.menuFunction = enterParameterMenu, .param = &parameterTrafficDirectionSignals },
	{ .name = "Oswietlenie",				.menuFunction = enterParameterMenu, .param = &parameterLightning },
};

MenuFormat controlParameterMenu =
{
	.header = "Sterowanie",
	.menuItems = controlParameterMenuItems,   // ← pointer to first MenuItem
	.itemsNumber = sizeof(controlParameterMenuItems) / sizeof(controlParameterMenuItems[0]),
	.currentItem = 0,
	.parentMenu = &settingsMenu,
	.menuType = SCROLL_MENU,
};

MenuItem settingsMenuItems[] =
{
    { .name = "Parametry czas"	,			.menuFunction = enterSubMenu, .param = &timesParameterMenu},
    { .name = "Kontrola",	  				.menuFunction = enterSubMenu, .param = &monitoringParameterMenu},
    { .name = "Sterowanie",        			.menuFunction = enterSubMenu, .param = &controlParameterMenu},
    { .name = "Dziennik zdarzen",	  		.menuFunction = 0, .param = 0 },
    { .name = "Jezyk",       				.menuFunction = 0, .param = 0 },
    { .name = "Ustawienia fabryczne",      	.menuFunction = enterParameterMenu, .param = &parameterFactoryReset},
};

MenuFormat settingsMenu =
{
    .header = "Ustawienia",
    .menuItems = settingsMenuItems,   // ← pointer to first MenuItem
    .itemsNumber = sizeof(settingsMenuItems) / sizeof(settingsMenuItems[0]),
    .currentItem = 0,
	.parentMenu = &settingsMenu,
	.menuType = SCROLL_MENU,
};

MenuFormat *currentMenu = &settingsMenu;
Parameter *currentParameter = 0;

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

void downButtonFunction(void)
{
	switch(currentMenu->menuType)
	{
		case SCROLL_MENU:
			goToNextItem();
			break;

		case PARAMETER_ADJUSTMENT:

			paramChangeValue(currentParameter, FALSE);
			showParameterValue();
			break;
	}
}

void upButtonFunction(void)
{
	switch(currentMenu->menuType)
	{
		case SCROLL_MENU:
			goToPreviousItem();
			break;

		case PARAMETER_ADJUSTMENT:
			paramChangeValue(currentParameter, TRUE);
			showParameterValue();
			break;
	}
}

void okButtonFunction(void)
{
	switch(currentMenu->menuType)
	{
		case SCROLL_MENU:
			callItemFunction();
			break;

		case PARAMETER_ADJUSTMENT:

			if(currentParameter == &parameterFactoryReset)
			{
				if(parameterFactoryReset.value)
				{
					flash_factoryReset();
				}
				else
				{
					backToParentMenu();
				}
			}
			else
			{
				paramSaveValue(currentParameter);
				backToParentMenu();
			}

			break;
	}
}

void escButtonFunction(void)
{
	switch(currentMenu->menuType)
	{
		case SCROLL_MENU:
			break;

		case PARAMETER_ADJUSTMENT:
			paramCancelValue(currentParameter);
			break;
	}

	backToParentMenu();
}
