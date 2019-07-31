/*
 * drawFunctions.c
 *
 *  Created on: May 15, 2017
 *      Author: e1218968
 */

#include "drawFunctions.h"
#include "fontTable.h"
#include "OLED.h"


#define CHINESE_CHAR_WIDTH				14
#define MAX_NUM_OF_WORDS_IN_TEXT		20
 _Bool scrollingHighlight;


/* Function Prototypes */
static int charStartIndex(unsigned char ch, const unsigned char fontTable[]);

/* Function Definitions */

int charStartIndex(unsigned char ch, const unsigned char fontTable[])
{
	int start = (fontTable[1] - fontTable[0] + 1) +2;
	int offset = 0;
	int x = ch - fontTable[0];
	if (x!=0) {
		for(int i=0; i<x; i++){
			offset += fontTable[2+i]*2;
		}
	}
	int startIndex = start + offset;
	return startIndex;
}
int charWidth(unsigned char ch, const unsigned char fontTable[])
{
	int x = ch - fontTable[0];
	int width = fontTable[2+x];
	return width;
}
void drawCharacter(unsigned char ch, const unsigned char fontTable[], unsigned char page, unsigned char col)
{
	int startIndex = charStartIndex(ch, fontTable);
	int width = charWidth(ch, fontTable);
	int maxCol = 0;

	if ((col+width) > 127)
	{
		maxCol = COLUMN_NUM_MAX+1;
	}
	else //comment this out will not display any text message
	{
		maxCol = col+width;
	}

	// upper page:
	for (int i = col; i < maxCol; i++)
	{
		I2CDataBuffer[page][i] = fontTable[startIndex++];
	}
	// lower page:
	for (int i = col; i < maxCol; i++)
	{
		I2CDataBuffer[page+1][i] = fontTable[startIndex++];
	}
	endOfMajorColumn = maxCol;
	/*if(scrolling)
	{
		I2CSendData();
	}*/
}
void drawAndHighlightCharacter(unsigned char ch, const unsigned char fontTable[], unsigned char page, unsigned char col)
{
	int startIndex = charStartIndex(ch, fontTable);
	int width = charWidth(ch, fontTable);
	int maxCol = 0;
	char unsigned static scrllIndex = 0;

	if ((col+width) > 127)
	{
		maxCol = COLUMN_NUM_MAX+1;
	}
	else
	{
		maxCol = col+width;
	}

	if(scrollingHighlight)
	{
		// upper page:

		for (int i = page; i < page+2; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				I2CDataBuffer[i][j] = 0xff;
			}
		}
		for (int i = col; i < maxCol; i++)
		{

			I2CDataBuffer[page][i] = ~fontTable[startIndex++];
		}
		// lower page:
		for (int i = col; i < maxCol; i++)
		{

			I2CDataBuffer[page+1][i] = ~fontTable[startIndex++];
		}

		for (int i = page; i < page+2; i++)
		{
			for (int j = maxCol; j < 128; j++)
			{
				I2CDataBuffer[i][j] = 0xff;
			}
		}
	}
	else
	{
		// upper page:
		for (int i = col; i < maxCol; i++)
		{
			I2CDataBuffer[page][i] = ~fontTable[startIndex++];
		}
		// lower page:
		for (int i = col; i < maxCol; i++)
		{
			I2CDataBuffer[page+1][i] = ~fontTable[startIndex++];
		}
	}
}
void displayTextInOneLine(const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col)
{
	int len = strlen(string);
	int width;
	for (int i = 0; i < len; i++)
	{
		width = charWidth(string[i],fontTable);
		if(col + width > 127)
		{
			/*do nothing*/
		}
		else
		{
			drawCharacter(string[i], fontTable, page, col);
		}
		col += width;
	}
	if(aboutMenuItems)
	{
		if (page == 0)
		{
			pgOneCol = col;
		}
		else if(page==2)
		{
			pgTwoCol = col;
		}
		else if(page == 4)
		{
			pgThreeCol = col;
		}
		else
		{
			pgFourCol = col;
			aboutMenuItems = FALSE;
		}
	}
}
void displayTextInOneLineHighlighted (const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col)
{
	int len = strlen(string);
	int width;
	for (int i = 0; i < len; i++)
	{
		width = charWidth(string[i],fontTable);
		if(col + width > 127)
		{

		}
		else
		{
			drawAndHighlightCharacter(string[i], fontTable, page, col);
		}
		col += width;
	}
}
void displayTextInOneLineScrolling(const unsigned char string[], const unsigned char fontTable[], unsigned char page, unsigned char col, unsigned char scrollingIndex, _Bool highlightTxt)
{
	if(highlightTxt)
	{
		displayTextInOneLineHighlighted(string, fontTable, page, col-scrollingIndex);
		//clearAreaHighlighted(page, 0, page+1, col);
	}
	else
	{
		displayTextInOneLine(string, fontTable, page, col-scrollingIndex);
		clearArea(page, 0, page+1, col);
	}
}

// startCharNum start with 0
void displayChineseTextInOneLine(const char fontTable[], int startCharNum, int endCharNum, int spaceInBetween, int startPage, int startColumn)
{
	int index = startCharNum * (CHINESE_CHAR_WIDTH*2); // each chinese char is (CHINESE_CHAR_WIDTH*2) bytes wide.
		int numOfChar;
		if(startCharNum <= endCharNum)
		{
			numOfChar = endCharNum-startCharNum + 1;
			while(numOfChar>0)
			{
				for (int i = startPage; i < startPage+2; i++) // the height is 2 for every line
				{
					for (int j = startColumn; j < startColumn+CHINESE_CHAR_WIDTH; j++)
					{
						I2CDataBuffer[i][j] = fontTable[index++];

					}
				}
				startColumn = startColumn + CHINESE_CHAR_WIDTH + spaceInBetween;
				numOfChar--;
			}
		}
}

// startCharNum start with 0
void displayChineseTextInOneLineHighlighted(const char fontTable[], int startCharNum, int endCharNum, int spaceInBetween, int startPage, int startColumn)
{
	int index = startCharNum * (CHINESE_CHAR_WIDTH*2); // each chinese char is (CHINESE_CHAR_WIDTH*2) bytes wide.
	int numOfChar;
	if(startCharNum <= endCharNum)
	{
		numOfChar = endCharNum-startCharNum + 1;
		while(numOfChar>0)
		{
			for (int i = startPage; i < startPage+2; i++)
			{
				for (int j = startColumn; j < startColumn+CHINESE_CHAR_WIDTH; j++)
				{
					I2CDataBuffer[i][j] = ~fontTable[index++];
				}
			}
			startColumn = startColumn + CHINESE_CHAR_WIDTH + spaceInBetween;
			numOfChar--;
		}
	}
}

/** Display text on the screen in multiple lines.
 * @param char *text: Text that needs to be displayed. Each word or symbol in the string is separated by '_'.
 * @param int index: index of a string in the text
 * @param const char *font: The font table of the text.
 * @param int* lastPage: return the last page number after displaying the text.
 * @param int* lastColumn: return the last column after displaying the text.
 * @param _Bool isAlarm: if text is for an alarm, then show an attention icon before it.
 * The position is adjusted based on the length of the text.
 * Assume one line can fit at least one word.
 * If the text is too long, than the screen will show "Too Long!"
**/
void displayTextInMultipleLines(const unsigned char *text, const unsigned char *font, int* lastPage, int* lastColumn, _Bool isAlarm)
{
	// Initialize an array, then go through the alarm text and put width of each word in the array;
	// MAX_NUM_OF_WORDS_IN_ALARM_TEXT is defined as the maximum number of words in a single
	// alarm text, for now it is set to 10
	int wordLength[MAX_NUM_OF_WORDS_IN_TEXT];
	int wordIndex = 0;//Index of word in the array wordLength[];
	for (int i = 0; i < MAX_NUM_OF_WORDS_IN_TEXT; i++)
	{
		wordLength[i] = 0;
	}
	int stringLength = strlen(text);
	int width = 0;
	for (int i = 0; i <= stringLength; i++)// Find width of each word.
	{
		if(text[i] == ' ')
		{
			wordLength[wordIndex++] = width;
			width = 0;
		}
		else
		{
			width += charWidth(text[i],font);
		}
	}
	// Write each word into the display memory.
	int line = 1;
	int column = 5;//start at column 5 on every line
	wordIndex = 1;//set wordIndex to second word in the array (Assume one line can fit at least one word)
	for(int i = 0; i <= stringLength; i++)//Determine number of lines that text will take up
	{
		if ((text[i] == ' ') && (i != stringLength))//space, but not the last space
		{
			column += 3;//Space symbol is 3 column wide.
			if ((column + wordLength[wordIndex++]) > 127)//If the next word goes off screen...
			{
				line++; //go to the next line
				if(isAlarm){column = 23;}
				else{column = 5;}
			}
		}
		else if((text[i] == ' ') && (i == stringLength))//last space
		{/*do nothing*/}
		else
		{column += charWidth(text[i],font);}
	}

	int page = 0;
	column = 5;//start at column 5 on every line
	wordIndex = 1;
	if(line > 4)//too many words
	{
		clearOneLine(3);
		displayTextInOneLine("TOO LONG!", font, 3, 20);
		*lastPage = -1;
		*lastColumn = -1;
	}
	else
	{
		page = 4 - line; //determine page start point
		if (isAlarm)//add alarm icon
		{
			drawIcon(attentionIcon, 16, 2, 3, column);
			column+=18;
		}
		for(int i = 0; i <= stringLength; i++)//start printing
		{
			if ((text[i] == ' ') && (i != stringLength))//space detect
			{
				column += 3;
				if ((column + wordLength[wordIndex++]) > 127)//If the next word goes off screen, start on the next line
				{
					page += 2;
					if(isAlarm) {column = 23;}
					else {column = 5;}
				}
			}
			else if ((text[i] == ' ') && (i == stringLength)) //last space
			{/*do nothing*/}
			else
			{
				drawCharacter(text[i], font, page, column);
				column += charWidth(text[i],font); //move the index to the next character
			}
		}

		*lastPage = page;
		*lastColumn = column;
	}
}



/* ===================================================================
** Function Definitions
** =================================================================== /
/*
** ===================================================================
**     Method      :  		drawDigit
**     Description :		Show a single digit start at position
**     						specified by page and column.
**     Parameters  :
**         NAME            - DESCRIPTION
**         font[]          - font table for the digit
**         digit		   - choose from digit 0 --> digit 9
**         heightInPages   - height of digit in pages
**         widthInRows     - width of digit in rows
**         page			   - start position: page number
**         column 		   - start position: column number
** ===================================================================
*/
void drawDigit(const char font[], unsigned char digit, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column)
{
	int startIndex = digit*(heightInPages*widthInRows);
	int width = widthInRows;

	if(checkRangeOfI2CDataBuffer(page, column, page+heightInPages-1, column+width-1))
	{
		int pageNumber;

		for (pageNumber = page; pageNumber < page+heightInPages; pageNumber++)
		{
			for (int i = column; i < column+width; i++)
			{
				I2CDataBuffer[pageNumber][i] = font[startIndex++];
			}
		}
	}
}

/*
** ===================================================================
**     Method      :  		drawIntegerRightAligned
**     Description :		Show a integer start at position specified
**     						by page and column up to three digits. The
**     						rightmost position should be the start
**     						position.
**     Parameters  :
**         NAME            - DESCRIPTION
**         font[]          - font table for the digit.
**         number		   - number to be displayed (-999 to 999)
**         heightInPages   - height of digit in pages
**         widthInRows     - width of digit in rows
**         page			   - start position: page number
**         column 		   - start position: column number (within a range)
** ===================================================================
*/
void drawIntegerRightAligned(const char font[], int number, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column)
{
	unsigned char hundreds = number/100;
	unsigned char tens = (number%100)/10;
	unsigned char ones = number%10;
	if (number >= 100 && number <= 999)
	{
		char colNumHundreds;
		char colNumTens;
		char colNumOnes;
		int startCol;
		if(confirmDigitDisplay)
		{
			startCol = column-widthInRows*2;
			colNumHundreds = column-widthInRows*2-3;
			colNumTens = column-widthInRows-3;
			colNumOnes = column-3;
		}
		else
		{
			startCol = column-widthInRows*4;
			colNumHundreds = column-widthInRows*3;
			colNumTens = column-widthInRows*2;
			colNumOnes = column-widthInRows;
		}
		if (startCol < COLUMN_NUM_MIN)
		{
			startCol = COLUMN_NUM_MIN;
		}
		int endCol = column;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}

		clearArea(page, startCol, page+heightInPages, endCol);
		drawDigit(font, hundreds, heightInPages, widthInRows, page, colNumHundreds);
		drawDigit(font, tens, heightInPages, widthInRows, page, colNumTens);
		drawDigit(font, ones, heightInPages, widthInRows, page, colNumOnes);
	}
	else if (number < 100 && number >= 10)
	{
		int endCol = column;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}

		//clearArea(page, startCol, page+heightInPages, endCol);
		if(confirmDigitDisplay) //print number of confirmation text, column must be adjusted to make space between digit and message look nice
		{
			int startCol = column-widthInRows*2-5;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
			drawDigit(font, tens, heightInPages, widthInRows, page, column-widthInRows*2-5); //higher column number, more space between text message and this digit number.
			drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows-5);
		}
		else //print number on the adjustable screen
		{
			int startCol = column-widthInRows*4;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
			drawDigit(font, tens, heightInPages, widthInRows, page, column-widthInRows*2);
			drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows);
		}
	}
	else if (number < 10 && number >= 0) //will display the -0.1 to -0.9 since we will send a 0 to here
	{
		/* For -0.1 to 0.9 Celsius, we will read in -1 and -9 in modbus and when we do division of 10, we will get 0.
		 * That is, -9/10 or -1/10 will always result in zero and so it will come to this case for -0.1 to -0.9 Celsius.
		 * thus, to correctly display these range of negative temperature, we will check for a "number == 0" to accomadate for this case.*/
		int endCol = column;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}

		//clearArea(page, startCol, page+heightInPages, endCol);
		if(confirmDigitDisplay) //print digit for comfirming message
		{
			int startCol = column-widthInRows*3+3;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
			if(number == 0 && negativeDecimalZero)//-0.1 t0 -0.9 Celsius
			{
				if (widthInRows == 28)
				{
					drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-widthInRows*2-NEGATIVE_SIGN_WIDTH);
					drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows);
				}
				else
				{
					drawDigit(font, NEGATIVE_SIGN_INDEX, heightInPages, widthInRows, page, column-widthInRows*3-3);
					drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows*3+2);
				}
			}
			else //0.0 to 9.9
			{

				drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows-5);
			}
		}
		else //print digit on adjusting screen
		{
			int startCol = column-widthInRows*4;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
			drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows);
		}
	}
	else if (number < 0 && number > -10) // -1.0 to -9.9
	{
		int startCol;
		char colDigit;
		char colNegativeSign;
		if(confirmDigitDisplay)
		{
			startCol = column-widthInRows*2;
			colNegativeSign = column-widthInRows-2;
			colDigit = column-widthInRows+6;
		}
		else
		{
			startCol = column-widthInRows*4;
			colNegativeSign = column-widthInRows*2;
			colDigit = column-widthInRows;
		}

		if (startCol < COLUMN_NUM_MIN)
		{
			startCol = COLUMN_NUM_MIN;
		}

		int endCol = column;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}
		clearArea(page, startCol, page+heightInPages, endCol);
		if (widthInRows == 28)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-widthInRows-NEGATIVE_SIGN_WIDTH);
		}
		else
		{
			drawDigit(font, NEGATIVE_SIGN_INDEX, heightInPages, widthInRows, page, colNegativeSign);
		}
		ones = (-number)%10;
		drawDigit(font, ones, heightInPages, widthInRows, page, colDigit);
	}
	else if (number <= -10 && number > -100) //display -10 to -99 Celsius
	{
		int startCol;
		char colNegativeSign;
		char colNumTensDigit;
		char colNumOnesDigit;
		if(confirmDigitDisplay)
		{
			startCol = column-widthInRows*2;
			colNegativeSign = column-widthInRows-3;
			colNumTensDigit = column-3;
			colNumOnesDigit = column+widthInRows-3;
		}
		else
		{
			startCol = column-widthInRows*4;
			colNegativeSign = column-widthInRows*3;
			colNumTensDigit = column-widthInRows*2;
			colNumOnesDigit = column-widthInRows;
		}

		if (startCol < COLUMN_NUM_MIN)
		{
			startCol = COLUMN_NUM_MIN;
		}
		int endCol = column;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}

		clearArea(page, startCol, page+heightInPages, endCol);
		tens = ((-number)%100)/10;
		ones = (-number)%10;
		if (widthInRows == 28)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-widthInRows*2-NEGATIVE_SIGN_WIDTH);
		}
		else
		{
			drawDigit(font, NEGATIVE_SIGN_INDEX, heightInPages, widthInRows, page, colNegativeSign);
		}
		drawDigit(font, tens, heightInPages, widthInRows, page, colNumTensDigit);//draw the first number
		drawDigit(font, ones, heightInPages, widthInRows, page, colNumOnesDigit); //draw second digit;
	}
	else if (number <= -100) //display any number less than -100
	{
		if (widthInRows == 28)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-widthInRows*3-NEGATIVE_SIGN_WIDTH);
		}
		else
		{
			drawDigit(font, NEGATIVE_SIGN_INDEX, heightInPages, widthInRows, page, column-widthInRows*4);
		}
		hundreds = (-number)/100;
		tens = ((-number)%100)/10;
		ones = (-number)%10;
		drawDigit(font, hundreds, heightInPages, widthInRows, page, column-widthInRows*3);
		drawDigit(font, tens, heightInPages, widthInRows, page, column-widthInRows*2);
		drawDigit(font, ones, heightInPages, widthInRows, page, column-widthInRows);
	}
}

/*
** ===================================================================
**     Method      :  		drawIntegerLeftAligned
**     Description :		Show a integer start at position specified
**     						by page and column up to three digits. The
**     						leftmost position should be the start
**     						position.
**     Parameters  :
**         NAME            - DESCRIPTION
**         font[]          - font table for the digit.
**         number		   - number to be displayed (-999 to 999)
**         heightInPages   - height of digit in pages (Currently only works for digitCourier2x7 and digitCalibriLight4x28)
**         widthInRows     - width of digit in rows
**         page			   - start position: page number
**         column 		   - start position: column number
** ===================================================================
*/
void drawIntegerLeftAligned(const char font[], int number, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column)
{
	unsigned char hundreds = number/100;
	unsigned char tens = (number%100)/10;
	unsigned char ones = number%10;
	unsigned int width;

	if (number >= 100 && number <= 999)
	{
		if (heightInPages == 2)
		{
			width = NEGATIVE_SIGN_WIDTH_ARIAL;
		}
		else if (heightInPages == 4)
		{
			width = NEGATIVE_SIGN_WIDTH;
		}
		int startCol = column-width;
		if (startCol < COLUMN_NUM_MIN)
		{
			startCol = COLUMN_NUM_MIN;
		}
		int endCol = column+widthInRows*3;
		if (endCol > COLUMN_NUM_MAX)
		{
			endCol = COLUMN_NUM_MAX;
		}
		clearArea(page, startCol, page+heightInPages, endCol);
		drawDigit(font, hundreds, heightInPages, widthInRows, page, column);
		drawDigit(font, tens, heightInPages, widthInRows, page, column+widthInRows);
		drawDigit(font, ones, heightInPages, widthInRows, page, column+widthInRows*2);
	}
	else if (number < 100 && number >= 10)
	{
		if (heightInPages == 2)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH_ARIAL;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		else if (heightInPages == 4)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		drawDigit(font, tens, heightInPages, widthInRows, page, column);
		drawDigit(font, ones, heightInPages, widthInRows, page, column+widthInRows);
	}
	else if (number < 10 && number >= 0)
	{
		if (heightInPages == 2)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH_ARIAL;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		else if (heightInPages == 4)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		drawDigit(font, ones, heightInPages, widthInRows, page, column);
	}
	else if (number < 0 && number > -10)
	{
		if (heightInPages == 2)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH_ARIAL;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		else if (heightInPages == 4)
		{
			int startCol = column-NEGATIVE_SIGN_WIDTH;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column+widthInRows*3;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		// Negative Sign
		if (heightInPages == 2)
		{
			drawCharacter('-', arial14, page, column-NEGATIVE_SIGN_WIDTH_ARIAL);
		}
		else if (heightInPages == 4)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-NEGATIVE_SIGN_WIDTH);
		}
		ones = (-number)%10;
		drawDigit(font, ones, heightInPages, widthInRows, page, column);
	}
	else if (number <= -10 && number > -100)
	{
		if (heightInPages == 2)
		{
			int startCol = column;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column-NEGATIVE_SIGN_WIDTH_ARIAL;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		else if (heightInPages == 4)
		{
			int startCol = column;
			if (startCol < COLUMN_NUM_MIN)
			{
				startCol = COLUMN_NUM_MIN;
			}
			int endCol = column-NEGATIVE_SIGN_WIDTH;
			if (endCol > COLUMN_NUM_MAX)
			{
				endCol = COLUMN_NUM_MAX;
			}
			clearArea(page, startCol, page+heightInPages, endCol);
		}
		tens = ((-number)%100)/10;
		ones = (-number)%10;
		if (heightInPages == 2)
		{
			drawCharacter('-', arial14, page, column-NEGATIVE_SIGN_WIDTH_ARIAL);
		}
		else if (heightInPages == 4)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-NEGATIVE_SIGN_WIDTH);
		}
		drawDigit(font, tens, heightInPages, widthInRows, page, column);
		drawDigit(font, ones, heightInPages, widthInRows, page, column+widthInRows);
	}
	else if (number <= -100)
	{
		if (heightInPages == 2)
		{
			drawCharacter('-', arial14, page, column-NEGATIVE_SIGN_WIDTH_ARIAL);
		}
		else if (heightInPages == 4)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-NEGATIVE_SIGN_WIDTH);
		}
	    hundreds = (-number)/100;
		tens = ((-number)%100)/10;
		ones = (-number)%10;
		drawDigit(font, hundreds, heightInPages, widthInRows, page, column);
		drawDigit(font, tens, heightInPages, widthInRows, page, column+widthInRows);
		drawDigit(font, ones, heightInPages, widthInRows, page, column+widthInRows*2);
	}
}

void drawIcon(const char icon[], int width, unsigned char pageNum, unsigned char page, unsigned char col) // pageNum: how many pages the icon uses.
{
	int startIndex = 0;
	for (int p = 0; p < pageNum; p++)
	{
		for (int i = col; i < col+width; i++)
		{
			I2CDataBuffer[page+p][i] = icon[startIndex++];
		}
	}
}
void clearDisplay(void)
{
	for (int i = PAGE_NUM_MIN; i <= PAGE_NUM_MAX; i++)
	{
		for (int j = COLUMN_NUM_MIN; j <= COLUMN_NUM_MAX; j++)
		{
			I2CDataBuffer[i][j] = 0;
		}
	}
}
void fillDisplay(void)
{
	for (int i = PAGE_NUM_MIN; i <= PAGE_NUM_MAX; i++)
	{
		for (int j = COLUMN_NUM_MIN; j <= COLUMN_NUM_MAX; j++)
		{
			I2CDataBuffer[i][j] = 0xFF;
		}
	}
}
void clearArea(int startPage, int startColumn, int endPage, int endColumn)
{
	if (checkRangeOfI2CDataBuffer(startPage, startColumn, endPage, endColumn))
	{
		for (int i = startPage; i <= endPage; i++)
		{
			for (int j = startColumn; j <= endColumn; j++)
			{
				I2CDataBuffer[i][j] = 0;
			}
		}
	}
}
void clearOneLine(int page)
{
	// Clear The line
	for (int i = page; i < page+2; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			I2CDataBuffer[i][j] = 0;
		}
	}
}
void highlightOneLine(int page)
{
	// Highlight The line
	for (int i = page; i < page+2; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			I2CDataBuffer[i][j] = 0xff;
		}
	}
}
void displayAllOn(void)
{
	// symbols at bottom
	drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	drawIcon(heaterIcon, 14, 2, HEATER_ICON_PAGE, HEATER_ICON_COLUMN);
	drawIcon(condensorIcon, 14, 2, CONDENSOR_ICON_PAGE, CONDENSOR_ICON_COLUMN);
	drawIcon(evapIcon, 14, 2, EVAPORATOR_ICON_PAGE, EVAPORATOR_ICON_COLUMN);
	drawIcon(compIcon, 14, 2, COMPRESSOR_ICON_PAGE, COMPRESSOR_ICON_COLUMN);
	// Digits
	drawIntegerRightAligned(digitCalibriLight4x28, 888, 4, 28, 1, 100);
	drawIcon(decimal_point, DECIMAL_POINT_WIDTH, 1, 4, 100-28);
	drawIcon(degreeF, 8, 2, 1, 105);
	drawIcon(degreeC, 8, 2, 3, 105);
}
