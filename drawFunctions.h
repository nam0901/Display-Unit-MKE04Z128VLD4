/*
 * drawFunctions.h
 *
 *  Created on: May 15, 2017
 *      Author: e1218968
 */

#ifndef SOURCES_DRAWFUNCTIONS_H_
#define SOURCES_DRAWFUNCTIONS_H_

#define BELL_ICON_COLUMN		15
#define HEATER_ICON_COLUMN		35
#define CONDENSOR_ICON_COLUMN	55
#define EVAPORATOR_ICON_COLUMN	75
#define COMPRESSOR_ICON_COLUMN	95
#define BELL_ICON_PAGE			6
#define HEATER_ICON_PAGE		6
#define CONDENSOR_ICON_PAGE		6
#define EVAPORATOR_ICON_PAGE	6
#define COMPRESSOR_ICON_PAGE	6

#define NEGATIVE_SIGN_WIDTH			14
#define NEGATIVE_SIGN_WIDTH_ARIAL 	4
#define DECIMAL_POINT_WIDTH     	4

#define DECIMAL_POINT_INDEX	  10
#define NEGATIVE_SIGN_INDEX   11
#define SCROLLING_NUM		  10

_Bool negativeDecimalZero;
_Bool confirmDigitDisplay; //used to make the comfirmation space between message and digit look "nice";
char static scrollingIndex[] = {0,0,0,0}; //used to create text scrolling illusion.
char static scrollingDelay[] = {0,0,0,0}; //used to delay scrolling speed.

_Bool aboutMenuItems; // use for About do check the last place of column used
unsigned char pgOneCol;
unsigned char pgTwoCol;
unsigned char pgThreeCol;
unsigned char pgFourCol;
char endOfMajorColumn;

/* Function Prototypes */
// Text
int charWidth(unsigned char ch, const unsigned char fontTable[]);

void drawCharacter(unsigned char ch, const unsigned char fontTable[], unsigned char page, unsigned char col);
void drawAndHighlightCharacter(unsigned char ch, const unsigned char fontTable[], unsigned char page, unsigned char col);
void displayTextInOneLine (const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col);
void displayTextInOneLineScrolling(const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col, unsigned char ScrollingIndex, _Bool highlightTxt);
void displayTextInOneLineHighlighted (const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col);
void displayChineseTextInOneLine(const char fontTable[], int startCharNum, int numOfChar, int spaceInBetween, int startPage, int startColumn);
void displayChineseTextInOneLineHighlighted(const char fontTable[], int startCharNum, int numOfChar, int spaceInBetween, int startPage, int startColumn);
void displayTextInMultipleLines(const unsigned char *text, const unsigned char *font, int* lastPage, int* lastColumn, _Bool isAlarm);
void displayChineseTextInMultipleLines(const char fontTable[], int startCharNum, int numOfChar, int spaceInBetween, int* lastPage, int* lastColumn, _Bool isAlarm);

// Digit
void drawDigit(const char font[], unsigned char digit, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column);
void drawIntegerRightAligned(const char font[], int number, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column);
void drawIntegerLeftAligned(const char font[], int number, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column);

// Icon
void drawIcon(const char icon[], int width, unsigned char pageNum, unsigned char page, unsigned char col);

// Generic Methods
void clearDisplay(void);
void fillDisplay(void);
void clearArea(int startPage, int startColumn, int endPage, int endColumn);
void clearOneLine(int page);
void highlightOneLine(int page);

// Display default screen on start-up
void displayAllOn(void);


#endif /* SOURCES_DRAWFUNCTIONS_H_ */
