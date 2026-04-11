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
#include "parameters.h"
#include "softwareTimer_ms.h"
#include "settingsForm.h"
#include "buttonsFunctions.h"
#include "systemStartup.h"
#include "suppCpuCommunication.h"
#include "canCommunication.h"
#include "logs.h"

#define UNINITIALIZED_VALUE			255

#define WORK_MODE					0
#define DIRECTION					1
#define SPEED						2
#define BREAK_STATE					3
#define ENGINE_STATE				4
#define CONTACTORS_STATE			5
#define SENSOR_UP					6
#define SENSOR_DOWN					7
#define SAFETY_CIRCUIT				8
#define ERROR_STATE					9
#define ITEMS_NUMBER				10

#define CHARS_PER_LINE 				21

#define NUMBER_OF_ERROR_TYPES 		11
#define CHANGE_ERROR_TIME_MS		2000
#define INFORMATION_TIME_MS			3000

MenusTypes activeMenu = MAIN_MENU;
bool errorStateActive = FALSE;
static bool informationActive = FALSE;
static bool informationWasShown = FALSE;
static bool hardFaultState = FALSE;

volatile uint8_t numberOfErrors = 0;
static SoftwareTimerHandler errorChangeTimer;
static SoftwareTimerHandler informationTimer;
static uint8_t displayedErrorIndex = 0;
static uint8_t changeString = 0;

WorkModeType workMode;

static ErrorsType actualErrors[NUMBER_OF_ERROR_TYPES] =
		{
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
		};

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

const char *safetyCircuitBottomStrings[] = {
    "Stop\nmaszynownia dol",
    "Stop wlot\ndol",
    "Prawa\nplyta grzeb. dol",
	"Lewa plyta\ngrzebieniowa dol",
	"Lewy\nwlot poreczy dol",
	"Prawy\nwlot poreczy dol",
	"Odpad.\nstopnia dol",
	"kontakt\nlancucha lewy dol",
	"kontakt\nlancucha prawy dol"
};

const char *directionStrings[] = {
		"Brak",
		"Dol",
		"Gora",
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

const char *speedStrings[] = {
		"Stop",
		"Wolno",
		"Szybko"
};

const char *workModeStrings[] = {
		"Ciagla",
		"Szyb-stop",
		"Szyb-wol",
		"Szyb-wol-s",
		"Inspekcja",
		"Serwisowy",
};

const char *errorsStrings[] = {
		"Brak\nbledu",
		"\nTermistor",
		"\nOdpad. lancuch",
		"Brak\nstopnia",
		"Gwiazda\ntrojkat",
		"Blad\nprzekaznika 1",
		"Blad\nstycznika 2",
		"Blad\nstycznika 3",
		"Obwod\nbezpieczenstwa",
		"Blad\nluzownika",
		"Blad\nzmiany predkosci",
		"Bledna\npredkosc",
		"\nCPU2 OFF",
		"\nDOL OFFLINE",
		"Blad\nenkodera"
};

typedef struct
{
	char *staticString;
	uint8_t state;
	char *stateString;
	uint16_t stringColor;
	uint16_t height;
}MenuObjects;

MenuObjects menuItems[ITEMS_NUMBER] =
{
		{
				"Tryb pracy: ",
				UNINITIALIZED_VALUE,
				"Ciagly",
				DARKGREEN,
				5,
		},

		{
				"Kierunek: ",
				UNINITIALIZED_VALUE,
				"Brak",
				DARKGREEN,
				28,
		},

		{
				"Predkosc: ",
				UNINITIALIZED_VALUE,
				"Stop",
				DARKGREEN,
				51,
		},

		{
				"Stan hamulca: ",
				UNINITIALIZED_VALUE,
				"Luz",
				DARKGREEN,
				74,
		},

		{
				"Stan lancucha: ",
				UNINITIALIZED_VALUE,
				"OK",
				DARKGREEN,
				97,
		},

		{
				"Stycz: ",
				UNINITIALIZED_VALUE,
				"OFF",
				DARKGREEN,
				120,
		},

		{
				"Czujnik gora: ",
				UNINITIALIZED_VALUE,
				"Brak",
				DARKGREEN,
				143,
		},

		{
				"Czujnik dol: ",
				UNINITIALIZED_VALUE,
				"Brak",
				DARKGREEN,
				166,
		},

		{
				"Obwod bezp: ",
				UNINITIALIZED_VALUE,
				"OK",
				DARKGREEN,
				189,
		},

		{
				"Status bledu: ",
				UNINITIALIZED_VALUE,
				"OK\n",
				DARKGREEN,
				232,
		},

};

void downButtonFunctionMainMenu(void *param);
void upButtonFunctionMainMenu(void *param);
void okButtonFunctionMainMenu(void *param);
void escButtonFunctionMainMenu(void *param);

void errorChangeTimerCallback(void *param)
{
	if(displayedErrorIndex == numberOfErrors - 1)
	{
		displayedErrorIndex = 0;
	}
	else
	{
		displayedErrorIndex++;
	}
}

void InformationTimerCallback(void)
{
	changeString++;

	if(checkSpeedTeached())
	{
		if(changeString >=3)
		{
			changeString = 0;
		}
	}
	else
	{
		if(changeString >=4)
		{
			changeString = 0;
		}
	}
}

void updateInformationState()
{
	static uint8_t changeStringTmp = 1;


	if(changeStringTmp != changeString)
	{
		changeStringTmp = changeString;

		switch(changeStringTmp)
		{
		case 0:
			ST7789_WriteString(centerString(Font_11x18, "OK -> Ustawienia"), 290, "OK -> Ustawienia", Font_11x18, BLACK, WHITE);
			break;
		case 1:
			if(cpu2Alive)
			{
				ST7789_WriteString(centerString(Font_11x18, "CPU1:ON CPU2:ON "), 290, "CPU1:ON CPU2:ON ", Font_11x18, BLACK, WHITE);
			}
			else
			{
				ST7789_WriteString(centerString(Font_11x18, "CPU1:ON CPU2:OFF"), 290, "CPU1:ON CPU2:OFF", Font_11x18, BLACK, WHITE);
			}
			break;
		case 2:
			if(getBootomBoardAlive())
			{
				ST7789_WriteString(centerString(Font_11x18, "  DOL:  ONLINE  "), 290, "  DOL:  ONLINE  ", Font_11x18, BLACK, WHITE);
			}
			else
			{
				ST7789_WriteString(centerString(Font_11x18, "  DOL: OFFLINE  "), 290, "  DOL: OFFLINE  ", Font_11x18, BLACK, WHITE);
			}
			break;
		case 3:
			ST7789_WriteString(centerString(Font_11x18, "Brak zpis. pred."), 290, "Brak zpis. pred.", Font_11x18, BLACK, WHITE);
			break;
		}
	}
}

void initMainForm(void)
{
	if(!informationActive && !hardFaultState)
	{
		ST7789_Fill_Color(WHITE);
		for(uint8_t i = 0; i < ITEMS_NUMBER; i++)
		{
			uint16_t stateStringBeggining = ((strlen(menuItems[i].staticString)-1)*Font_11x18.width) + 2;
			ST7789_WriteString(2, menuItems[i].height, menuItems[i].staticString, Font_11x18, BLACK, WHITE);
			ST7789_DrawLine(0, 272, ST7789_WIDTH, 272, BLACK);
			ST7789_DrawLine(0, 230, ST7789_WIDTH, 230, BLACK);

			ST7789_WriteString(centerString(Font_11x18, "OK -> Ustawienia"), 290, "OK -> Ustawienia", Font_11x18, BLACK, WHITE);

			if(menuItems[i].state != UNINITIALIZED_VALUE)
			{
				ST7789_WriteString(stateStringBeggining, menuItems[i].height, menuItems[i].stateString, Font_11x18, menuItems[i].stringColor, WHITE);
			}
		}


		initSoftwareTimer(&informationTimer, INFORMATION_TIME_MS, InformationTimerCallback, TRUE, 0);
		startSoftwareTimer(&informationTimer);
	}

	upButtonFunction = upButtonFunctionMainMenu;
	downButtonFunction = downButtonFunctionMainMenu;
	okButtonFunction = okButtonFunctionMainMenu;
	escButtonFunction = escButtonFunctionMainMenu;
}

void updateWorkMode(void)
{
	if(workWithoutBottom.value == FALSE)
	{
		if(serviceMode)
		{
			workMode = SERVICE;
		}
		else if(getIspectionMode())
		{
			workMode = INSPECTION;
		}
		else if(parameterAutoStop.value)
		{
			if(parameterReleasing.value)
			{
				workMode = FAST_SLOW_STOP;
			}
			else
			{
				workMode = FAST_STOP;
			}
		}
		else
		{
			if(parameterReleasing.value)
			{
				workMode = FAST_SLOW;
			}
			else
			{
				workMode = CONSTANS;
			}
		}
	}
	else
	{
		workMode = CONSTANS;
	}

	if(menuItems[WORK_MODE].state != workMode)
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[WORK_MODE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[WORK_MODE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[WORK_MODE].state = workMode;
		menuItems[WORK_MODE].stateString = workModeStrings[menuItems[WORK_MODE].state];

		if(menuItems[WORK_MODE].state == INSPECTION || menuItems[WORK_MODE].state == SERVICE)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[WORK_MODE].height, menuItems[WORK_MODE].stateString, Font_11x18, RED, WHITE);
			menuItems[WORK_MODE].stringColor = RED;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[WORK_MODE].height, menuItems[WORK_MODE].stateString, Font_11x18, DARKGREEN, WHITE);
			menuItems[WORK_MODE].stringColor = DARKGREEN;
		}
	}
}

void updateSafetyCircuitState(void)
{
	uint8_t safetyCircuitBottom;
	static bool isBottom = FALSE;
	if(safetyCircuitPoint == SAFETY_CIRCUIT_ERROR)
	{
		isBottom = TRUE;
		safetyCircuitBottom = getSafetyCircuitBottom();
	}

	if(isBottom && safetyCircuitPoint == SAFETY_CIRCUIT_ERROR)
	{
		if(menuItems[SAFETY_CIRCUIT].state != safetyCircuitBottom)
		{
			uint16_t stateStringBeggining = ((strlen(menuItems[SAFETY_CIRCUIT].staticString) - 1)*Font_11x18.width) + 2;
			uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
			char clearFirstLine[clearChars+1];

			memset(clearFirstLine, ' ', clearChars);
			clearFirstLine[clearChars] = '\0';
			ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, clearFirstLine, Font_11x18, BLACK, WHITE);
			ST7789_WriteString(2, menuItems[SAFETY_CIRCUIT].height + Font_11x18.height, clearLine, Font_11x18, BLACK, WHITE);
			menuItems[SAFETY_CIRCUIT].state = safetyCircuitBottom;
			menuItems[SAFETY_CIRCUIT].stateString = safetyCircuitBottomStrings[menuItems[SAFETY_CIRCUIT].state];

			if(menuItems[SAFETY_CIRCUIT].state == SAFETY_CIRCUIT_UNBROKEN)
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, menuItems[SAFETY_CIRCUIT].stateString, Font_11x18, DARKGREEN, WHITE);
				menuItems[SAFETY_CIRCUIT].stringColor = DARKGREEN;
			}
			else
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, menuItems[SAFETY_CIRCUIT].stateString, Font_11x18, RED, WHITE);
				menuItems[SAFETY_CIRCUIT].stringColor = RED;

			}
		}
	}
	else
	{
		if(isBottom)
		{
			isBottom = FALSE;
			menuItems[SAFETY_CIRCUIT].state = UNINITIALIZED_VALUE;
		}

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
				menuItems[SAFETY_CIRCUIT].stringColor = DARKGREEN;
			}
			else
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SAFETY_CIRCUIT].height, menuItems[SAFETY_CIRCUIT].stateString, Font_11x18, RED, WHITE);
				menuItems[SAFETY_CIRCUIT].stringColor = RED;

			}
		}
	}
}

void updateErrorState(void)
{
	if(menuItems[ERROR_STATE].state != actualErrors[displayedErrorIndex])
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[ERROR_STATE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[ERROR_STATE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, menuItems[ERROR_STATE].height + Font_11x18.height, clearLine, Font_11x18, BLACK, WHITE);
		menuItems[ERROR_STATE].state = actualErrors[displayedErrorIndex];
		menuItems[ERROR_STATE].stateString = errorsStrings[menuItems[ERROR_STATE].state];

		if(menuItems[ERROR_STATE].state == NO_ERROR)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[ERROR_STATE].height, menuItems[ERROR_STATE].stateString, Font_11x18, DARKGREEN, WHITE);
			menuItems[ERROR_STATE].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[ERROR_STATE].height, menuItems[ERROR_STATE].stateString, Font_11x18, RED, WHITE);
			menuItems[ERROR_STATE].stringColor = RED;
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
			menuItems[SENSOR_UP].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_UP].height, menuItems[SENSOR_UP].stateString, Font_11x18, RED, WHITE);
			menuItems[SENSOR_UP].stringColor = RED;
		}
	}
}

void updateSensorDown(void)
{
	if(menuItems[SENSOR_DOWN].state != getHumanDown())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[SENSOR_DOWN].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_DOWN].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[SENSOR_DOWN].state = getHumanDown();
		menuItems[SENSOR_DOWN].stateString = humanSensorsStrings[menuItems[SENSOR_DOWN].state];

		if(menuItems[SENSOR_DOWN].state)
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_DOWN].height, menuItems[SENSOR_DOWN].stateString, Font_11x18, DARKGREEN, WHITE);
			menuItems[SENSOR_DOWN].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[SENSOR_DOWN].height, menuItems[SENSOR_DOWN].stateString, Font_11x18, RED, WHITE);
			menuItems[SENSOR_DOWN].stringColor = RED;
		}
	}
}

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
			menuItems[DIRECTION].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[DIRECTION].height, menuItems[DIRECTION].stateString, Font_11x18, RED, WHITE);
			menuItems[DIRECTION].stringColor = RED;
		}
	}
}

void updateSpeed(void)
{
	static SpeedType speedState = SPEED_STOP;

	if(checkTargetFrequencyReached())
	{
		if(highSpeedSet)
		{
			speedState = FAST;
		}

		if(slowSpeedSet)
		{
			speedState = SLOW;
		}
	}

	if(!highSpeedSet && !slowSpeedSet)
	{
		speedState = SPEED_STOP;
	}

	if(menuItems[SPEED].state != speedState)
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[SPEED].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[SPEED].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[SPEED].state = speedState;
		menuItems[SPEED].stateString = speedStrings[menuItems[SPEED].state];

		switch(menuItems[SPEED].state)
		{
			case SPEED_STOP:
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SPEED].height, menuItems[SPEED].stateString, Font_11x18, RED, WHITE);
				menuItems[SPEED].stringColor = RED;
				break;
			}

			case SLOW:
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SPEED].height, menuItems[SPEED].stateString, Font_11x18, YELLOW, WHITE);
				menuItems[SPEED].stringColor = YELLOW;
				break;
			}

			case FAST:
			{
				ST7789_WriteString(stateStringBeggining, menuItems[SPEED].height, menuItems[SPEED].stateString, Font_11x18, DARKGREEN, WHITE);
				menuItems[SPEED].stringColor = DARKGREEN;
				break;
			}
		}
	}
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
			menuItems[BREAK_STATE].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[BREAK_STATE].height, menuItems[BREAK_STATE].stateString, Font_11x18, RED, WHITE);
			menuItems[BREAK_STATE].stringColor = RED;
		}
	}
}

void updateContactorsState(void)
{
	static uint8_t tempState, directionTmp;
	tempState = getIntContactorState() + (getAckK2()*2) + (getExtContactorState()*4);
	static char contactorsBuffer[20];
	sprintf(contactorsBuffer, "K1:%u K2:%u S1:%u", getIntContactorState(), getAckK2(), getExtContactorState());


	if(menuItems[CONTACTORS_STATE].state != tempState || directionTmp != getDirection())
	{
		uint16_t stateStringBeggining = ((strlen(menuItems[CONTACTORS_STATE].staticString) - 1)*Font_11x18.width) + 2;
		uint8_t clearChars = (ST7789_WIDTH - stateStringBeggining)/Font_11x18.width;
		char clearFirstLine[clearChars+1];

		memset(clearFirstLine, ' ', clearChars);
		clearFirstLine[clearChars] = '\0';
		ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, clearFirstLine, Font_11x18, BLACK, WHITE);
		menuItems[CONTACTORS_STATE].state = tempState;
		menuItems[CONTACTORS_STATE].stateString = contactorsBuffer;

		if((menuItems[CONTACTORS_STATE].state == 7 && getDirection()) || (menuItems[CONTACTORS_STATE].state == 3 && !getDirection()))
		{
			ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, menuItems[CONTACTORS_STATE].stateString, Font_11x18, DARKGREEN, WHITE);
			menuItems[CONTACTORS_STATE].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[CONTACTORS_STATE].height, menuItems[CONTACTORS_STATE].stateString, Font_11x18, RED, WHITE);
			menuItems[CONTACTORS_STATE].stringColor = RED;
		}
	}

	directionTmp = getDirection();
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
			menuItems[ENGINE_STATE].stringColor = DARKGREEN;
		}
		else
		{
			ST7789_WriteString(stateStringBeggining, menuItems[ENGINE_STATE].height, menuItems[ENGINE_STATE].stateString, Font_11x18, RED, WHITE);
			menuItems[ENGINE_STATE].stringColor = RED;
		}
	}
}

void addRemoveError(ErrorsType error, bool removeAdd)
{
	if(!serviceMode)
	{
		if(removeAdd)
		{
			for(uint8_t i = 0; i < numberOfErrors; i++)
			{
				if(actualErrors[i] == error)
				{
					return;
				}
			}

			actualErrors[numberOfErrors] = error;
			addLog(error);
			errorEventTx(error);
			numberOfErrors++;

			if(numberOfErrors > 1)
			{
				deInitSoftwareTimer(&errorChangeTimer);
				initSoftwareTimer(&errorChangeTimer, CHANGE_ERROR_TIME_MS, errorChangeTimerCallback, TRUE, 0);
				startSoftwareTimer(&errorChangeTimer);
			}
		}
		else
		{
			if(numberOfErrors > 0)
			{
				for(uint8_t i = 0; i < numberOfErrors; i++)
				{
					if(actualErrors[i] == error)
					{
						memcpy(&actualErrors[i], &actualErrors[i+1], NUMBER_OF_ERROR_TYPES - i - 1);

						actualErrors[numberOfErrors-1] = NO_ERROR;
						numberOfErrors--;

						if(numberOfErrors < 1)
						{
							stopSoftwareTimer(&errorChangeTimer);
						}

						displayedErrorIndex = 0;

					}
				}

			}
		}

		if(numberOfErrors)
		{
			errorStateActive = TRUE;
		}
		else
		{
			errorStateActive = FALSE;
		}
	}
}

void resetErrors(void)
{
	for(uint8_t i = 0; i < numberOfErrors; i++)
	{
		actualErrors[i] = 0;
	}

	errorStateActive = FALSE;
	numberOfErrors = 0;
}

void mainMenuSubTask(void)
{
	if(informationActive == FALSE && !hardFaultState)
	{
		updateSensorUp();
		updateSensorDown();
		updateSafetyCircuitState();
		updateDirection();
		updateLooserState();
		updateContactorsState();
		updateChainMotorState();
		updateSpeed();
		updateWorkMode();
		updateErrorState();
		updateInformationState();
	}
}

void downButtonFunctionMainMenu(void *param)
{

}

void upButtonFunctionMainMenu(void *param)
{

}

void okButtonFunctionMainMenu(void *param)
{
	if(!hardFaultState)
	{
		if(informationActive)
		{
			informationActive = FALSE;
			initMainForm();
		}
		else
		{
			enterSettingsMenu();
		}
	}
}

void escButtonFunctionMainMenu(void *param)
{

}

void openInformationForm(void)
{
	if(informationWasShown == FALSE)
	{
		if(!informationActive)
		{
			activeMenu = MAIN_MENU;
			ST7789_Fill_Color(WHITE);
			ST7789_WriteString(2, 5, "Brak polaczenia z\ndolna plytka!\nAby kontynuowac prace\nprzed przyjazdem\nserwisu, wlacz prace\nbez dolu w\nustawieniach", Font_11x18, BLACK, WHITE);
			ST7789_WriteString(2, 250, "Nacisinij ok aby\nkontynuowac", Font_11x18, BLACK, WHITE);
			informationActive = TRUE;
			informationWasShown = TRUE;
		}
	}
}

void openHardfaultForm(void)
{
	if(hardFaultState == FALSE)
	{
		hardFaultState = TRUE;
		ST7789_Fill_Color(WHITE);
		ST7789_WriteString(2, 5, "Sterownik uszkodzony!\nWezwij serwis!", Font_11x18, BLACK, WHITE);
	}
}



