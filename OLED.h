#ifndef __OLED_H
#define __OLED_H

#include "PE_Types.h"
#include "string.h"

/* Defines */

#define SA0								0
#define OLED_DISPLAY_OFF            	0xAE
#define OLED_SET_DISPLAY_CLOCK_DIV		0xD5
#define OLED_SETSTARTLINE               0x40
#define OLED_SETCONTRAST                0x81
#define OLED_SEGREMAP_SET               0xA1
#define OLED_DISPLAYALLON_RESUME        0xA4
#define OLED_DISPLAYALLON               0xA5
#define OLED_NORMALDISPLAY              0xA6
#define OLED_INVERTDISPLAY              0xA7
#define OLED_SETMULTIPLEX               0xA8
#define OLED_DISPLAYON                  0xAF
#define OLED_COMSCANDEC_MAP             0xC8
#define OLED_SETDISPLAYOFFSET           0xD3
#define OLED_SETPRECHARGE               0xD9
#define OLED_SETCOMPINS                 0xDA
#define OLED_SETVCOMDETECT              0xDB

// I2C Buffer size = num of columns (128) x num of rows (8) = 1024(All items in I2CDataBuffer) plus 1 because the first item is used for store data byte 0x40.
#define I2CBufferSize		1025

#define I2CDATABUFFER_RANGE_ERROR	0
#define I2CDATABUFFER_RANGE_OK		1

#define PAGE_NUM_MIN        0
#define PAGE_NUM_MAX		7
#define COLUMN_NUM_MIN      0
#define COLUMN_NUM_MAX      127

/* Global Variables */

	// I2CDataBuffer maps every column and page of the display to a two dimensional array of unsigned char. Using unsigned char because screen is written in bytes.
	extern unsigned char I2CDataBuffer[PAGE_NUM_MAX+1][COLUMN_NUM_MAX+1];
	// I2C Buffer size = num of columns (128) x num of rows (8) = 1024(All items in I2CDataBuffer) plus 1 because the first item is used for store data byte 0x40.
	extern unsigned char I2CDataToSend[I2CBufferSize];
	extern _Bool updateScreen;

/* Function Prototypes */

	void OLED_init (void);
	void I2CSendData(void);
	int checkRangeOfI2CDataBuffer(int startPage, int startColumn, int endPage, int endColumn);

#endif /* __OLED_H */
