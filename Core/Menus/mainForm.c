/*
 * mainForm.c
 *
 *  Created on: 18 lut 2026
 *      Author: mateo
 */


#include "mainForm.h"
#include "st7789.h"
#include "fonts.h"
#include "safetyCircuit.h"
#include "engineFunctions.h"

#define ITEMS_NUMBER	10

#define SAFETY_CIRCUIT	8

#define CHARS_PER_LINE 21

const char clearLine[22] = "                     ";

const char *safetyCircuitStrings[] = {
    "Stop\nmaszynownia", //ok
    "Stop wlot",
    "Prawa\nplyta grzebieniowa",
	"Lewa\nplyta grzebieniowa",
	"Prawy\nwlot poreczy",
	"Lewy\nwlot poreczy",
	"Odpad.\nstopnia", //dupa
	"rezerwa",
	"Blad\nobwodu",
	"OK"
};

typedef struct
{
	char *staticString;
	uint8_t state;
	char *stateString;
	uint16_t height;
}MenuItem;

MenuItem menuItems[ITEMS_NUMBER] =
{
		{
				"Tryb pracy: ",
				TRUE,
				"OK",
				5,
		},

		{
				"Kierunek: ",
				TRUE,
				"OK",
				28,
		},

		{
				"Predkosc: ",
				TRUE,
				"OK",
				51,
		},

		{
				"Stan hamulca: ",
				TRUE,
				"OK",
				74,
		},

		{
				"Stan lancucha: ",
				TRUE,
				"OK",
				97,
		},

		{
				"Stan stycznikow: ",
				TRUE,
				"OK",
				120,
		},

		{
				"Czujnik gora: ",
				TRUE,
				"OK",
				143,
		},

		{
				"Czujnik dol: ",
				TRUE,
				"OK",
				166,
		},

		{
				"Obwod bezp: ",
				0,
				"OK",
				189,
		},

		{
				"Status bledu: ",
				TRUE,
				"OK\ndupa",
				230,
		},

};


void initMainForm(void)
{
	for(uint8_t i = 0; i < ITEMS_NUMBER; i++)
	{
		uint16_t stateStringBeggining = (strlen(menuItems[i].staticString)*Font_11x18.width) + 2;
		ST7789_WriteString(2, menuItems[i].height, menuItems[i].staticString, Font_11x18, BLACK, WHITE);
		ST7789_WriteString(stateStringBeggining, menuItems[i].height, menuItems[i].stateString, Font_11x18, DARKGREEN, WHITE);
	}
}

void updateSafetyCircuitState(void)
{
	if(menuItems[SAFETY_CIRCUIT].state != safetyCircuitPoint)
	{
		uint16_t stateStringBeggining = (strlen(menuItems[SAFETY_CIRCUIT].staticString)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, menuItems[SAFETY_CIRCUIT].height + Font_11x18.height, clearLine, Font_11x18, BLACK, WHITE);
		menuItems[SAFETY_CIRCUIT].state = safetyCircuitPoint;
		menuItems[SAFETY_CIRCUIT].stateString = safetyCircuitStrings[menuItems[SAFETY_CIRCUIT].state];

		if(menuItems[SAFETY_CIRCUIT].state == SAFETY_CIRCUIT_UNBROKEN)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, menuItems[SAFETY_CIRCUIT].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, menuItems[SAFETY_CIRCUIT].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void updateSensorUp(void)
{
	checkIsHumanOnStairsUp();
}


