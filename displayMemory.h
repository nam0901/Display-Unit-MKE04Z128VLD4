#ifndef __DISPLAY_MEMORY_H
#define __DISPLAY_MEMORY_H

#include <stdio.h>
#include <string.h>

#define ENGLISH	 1
#define CHINESE  2
#define FRENCH   3
#define GERMAN   4
#define SPANISH  5
#define ITALIAN  6
#define POLISH   7

#define NOVALUE			0
#define TEMPERATURE		1
#define	MIN_SEC			2
#define HOUR			3
#define MIN_SEC_COLON	4
#define INTEGER 		5

char isAlarmTxt;
extern int userInput;
extern _Bool parameterIsSet;
extern _Bool updateDisplayMemory;
extern _Bool refreshScreen;
extern int maxIndex;
_Bool SysIntArrowPosition; //need this to adjust the arrow to make more space to fit "comp restart delay" in French in one line in System Interface;
_Bool SysSettingArrowPosition; //need this to adjust the arrow position to make the "MALF Deactivate" in French fit into one line in System Settings

extern int currentLanguage;
extern const unsigned char **currentTextTable;

void checkDisplayMemory(void);

/****************************************************************************Bootload Message Displaying*****************************************************************************************/
void displayBootloadMessage();

#endif /* __DISPLAY_MEMORY_H */
