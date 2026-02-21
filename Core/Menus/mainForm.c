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
#include "sensors.h"

#define WORK_MODE			0
#define DIRECTION			1
#define SPEED				2
#define BREAK_STATE			3
#define ENGINE_STATE		4
#define CONTACTORS_STATE	5
#define SENSOR_UP			6
#define SENSOR_DOWN			7
#define SAFETY_CIRCUIT		8
#define ERROR_STATE			9
#define ITEMS_NUMBER		10

#define CHARS_PER_LINE 21

const char clearLine[22] = "                     ";

const char *humanSensorsStrings[] = {
		"Brak",
		"Wykrycie",
};

const char *safetyCircuitStrings[] = {
    "Stop\nmaszynownia",
    "Stop wlot",
    "Prawa\nplyta grzebieniowa",
	"Lewa\nplyta grzebieniowa",
	"Prawy\nwlot poreczy",
	"Lewy\nwlot poreczy",
	"Odpad.\nstopnia",
	"rezerwa",
	"Blad\nobwodu",
	"OK"
};

const char *directionStrings[] = {
		"Brak",
		"Dol",
		"Gora",
		"Blad",
};

const char *speedStrings[] = {
		"Stop",
		"Wolno",
		"Szybko",
		"Blad",
};

const char *looserStrings[] = {
		"Luz",
		"Pierwszy",
		"Drugi",
		"Dwa",
};

const char *onOffStrings[] = {
		"OFF",
		"ON",
};

const char *chainMotorStrings[] = {
		"Blad",
		"OK",
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
				"Brak",
				28,
		},

		{
				"Predkosc: ",
				TRUE,
				"Stop",
				51,
		},

		{
				"Stan hamulca: ",
				TRUE,
				"Luz",
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
				"OFF",
				120,
		},

		{
				"Czujnik gora: ",
				TRUE,
				"Brak",
				143,
		},

		{
				"Czujnik dol: ",
				TRUE,
				"Brak",
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
		uint16_t stateStringBeggining = ((strlen(menuItems[i].staticString)-1)*Font_11x18.width) + 2;
		ST7789_WriteString(2, menuItems[i].height, menuItems[i].staticString, Font_11x18, BLACK, WHITE);
		ST7789_WriteString(stateStringBeggining, menuItems[i].height, menuItems[i].stateString, Font_11x18, DARKGREEN, WHITE);
	}
}

void updateSafetyCircuitState(void)
{
	if(menuItems[SAFETY_CIRCUIT].state != safetyCircuitPoint)
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[SAFETY_CIRCUIT].staticString) - 1)*Font_11x18.width) + 2;
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
	if(menuItems[SENSOR_UP].state != checkIsHumanOnStairsUp())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[SENSOR_UP].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_UP].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[SENSOR_UP].state = checkIsHumanOnStairsUp();
		menuItems[SENSOR_UP].stateString = humanSensorsStrings[menuItems[SENSOR_UP].state];

		if(menuItems[SENSOR_UP].state)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_UP].height, menuItems[SENSOR_UP].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_UP].height, menuItems[SENSOR_UP].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void updateSensorUp(void);

void updateDirection(void)
{
	if(menuItems[DIRECTION].state != getDirection())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[DIRECTION].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[DIRECTION].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[DIRECTION].state = getDirection();
		menuItems[DIRECTION].stateString = directionStrings[menuItems[DIRECTION].state];

		if(menuItems[DIRECTION].state == DOWN || menuItems[DIRECTION].state == UP)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[DIRECTION].height, menuItems[DIRECTION].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[DIRECTION].height, menuItems[DIRECTION].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void updateSpeed(void)
{

}

void updateLooserState(void)
{
	if(menuItems[BREAK_STATE].state != getLoosersState())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[BREAK_STATE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[BREAK_STATE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[BREAK_STATE].state = getLoosersState();
		menuItems[BREAK_STATE].stateString = looserStrings[menuItems[BREAK_STATE].state];

		if(menuItems[BREAK_STATE].state == NO_LOOSER)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[BREAK_STATE].height, menuItems[BREAK_STATE].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[BREAK_STATE].height, menuItems[BREAK_STATE].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void updateContactorsState(void)
{
	if(menuItems[CONTACTORS_STATE].state != getContactorsState())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[CONTACTORS_STATE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[CONTACTORS_STATE].state = getContactorsState();
		menuItems[CONTACTORS_STATE].stateString = onOffStrings[menuItems[CONTACTORS_STATE].state];

		if(menuItems[CONTACTORS_STATE].state)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, menuItems[CONTACTORS_STATE].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, menuItems[CONTACTORS_STATE].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void updateChainMotorState(void)
{
	if(menuItems[ENGINE_STATE].state != getChainMotorState())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[ENGINE_STATE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[ENGINE_STATE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[ENGINE_STATE].state = getChainMotorState();
		menuItems[ENGINE_STATE].stateString = chainMotorStrings[menuItems[ENGINE_STATE].state];

		if(menuItems[ENGINE_STATE].state)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[ENGINE_STATE].height, menuItems[ENGINE_STATE].stateString, Font_11x18, DARKGREEN, WHITE);
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[ENGINE_STATE].height, menuItems[ENGINE_STATE].stateString, Font_11x18, RED, WHITE);
		}
	}
}

void mainMenuSubTask(void)
{
	updateSensorUp();
	updateSafetyCircuitState();
	updateDirection();
	updateLooserState();
	updateContactorsState();
	updateChainMotorState();
}



