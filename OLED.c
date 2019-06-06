#include "OLED.h"
#include "Cpu.h"
#include "Events.h"
#include "RES.h"
#include "BitIoLdd1.h"
#include "R_W.h"
#include "BitIoLdd2.h"
#include "E_RD.h"
#include "BitIoLdd3.h"
#include "D_C.h"
#include "BitIoLdd4.h"
#include "CS.h"
#include "BitIoLdd5.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "TI1_125us.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "PE_Types.h"
#include "string.h"
#include "stdbool.h"


/* Function Prototypes */
static void OLED_WriteCommand (unsigned char commandToWrite);
static void setHorizontalMode(void);

int checkRangeOfI2CDataBuffer(int startPage, int startColumn, int endPage, int endColumn)
{
	if ((startPage > endPage) || (startColumn > endColumn) || (startPage < PAGE_NUM_MIN) || (endPage > PAGE_NUM_MAX) || (startColumn < COLUMN_NUM_MIN) || (endColumn > COLUMN_NUM_MAX))
	{
		return I2CDATABUFFER_RANGE_ERROR;
	}
	else
	{
		return I2CDATABUFFER_RANGE_OK;
	}
}

void OLED_init(void)
{
	/* Reset OLED */
	RES_ClrVal();

	/* OLED MCU interface selection: I2C */
	E_RD_ClrVal();
	R_W_ClrVal();
	CS_ClrVal();
	D_C_PutVal(SA0);
	RES_SetVal();									// clearing this line resets the display driver
	CI2C1_SelectSlave(0x3Cu);						// set the slave 7-bit address

	OLED_WriteCommand(OLED_DISPLAY_OFF);
	OLED_WriteCommand(OLED_SET_DISPLAY_CLOCK_DIV);
	OLED_WriteCommand(0x80);  // the suggested ratio 0x80
    OLED_WriteCommand(OLED_SETMULTIPLEX); // 0xA8
    OLED_WriteCommand(0x3F);  // 0x3F 1/64 duty
	OLED_WriteCommand(OLED_SETDISPLAYOFFSET); // 0xD3
	OLED_WriteCommand(0x0); // no offset
	OLED_WriteCommand(OLED_SETSTARTLINE); // line #0
	OLED_WriteCommand(OLED_SEGREMAP_SET); //0xA0
	OLED_WriteCommand(OLED_COMSCANDEC_MAP); //0xC0
	OLED_WriteCommand(OLED_SETCOMPINS); // 0xDA
	OLED_WriteCommand(0x12); // disable COM left/right remap
	OLED_WriteCommand(OLED_SETCONTRAST); //0X81
	OLED_WriteCommand(0x80); //RESET VALUE
	OLED_WriteCommand(OLED_SETPRECHARGE); // 0xd9
	OLED_WriteCommand(0xF1);
	OLED_WriteCommand(OLED_SETVCOMDETECT); // 0xDB
	OLED_WriteCommand(0x34);//RESET
	OLED_WriteCommand(OLED_DISPLAYALLON_RESUME); // 0xA4
	OLED_WriteCommand(OLED_NORMALDISPLAY); // 0xA6
	OLED_WriteCommand(OLED_DISPLAYON);//--turn on oled panel

	setHorizontalMode();
}

void OLED_WriteCommand(unsigned char commandToWrite)
{
	word tx_cnt;
	byte error;
	unsigned char dataToSend[2];
	dataToSend[0] = 0x80;
	dataToSend[1] = commandToWrite;
	I2CIsBusy = true;
	error =  CI2C1_SendBlock(&dataToSend, 2, &tx_cnt);
	while(I2CIsBusy);
	int i = 0;
	while(i<16000)
	{
		i++;
	}
}

void I2CSendData(void)
{
    if (updateScreen)
    {
    	int unsigned static num = 0;
        if (!I2CIsBusy)
        {
            word tx_cnt;
            byte error;
            int j = 1;
            int num = 0;
            for (int i = PAGE_NUM_MIN; i <= PAGE_NUM_MAX; i++)
            {
                for (int x = COLUMN_NUM_MIN; x <= COLUMN_NUM_MAX; x++)
                {
                    I2CDataToSend[j++] = I2CDataBuffer[i][x];
                }
            }
            I2CIsBusy = TRUE;
           /* if(waitTime)
		   {
				waitTime = false;
				while(num < 10)
					{
					n++;
					}
		   }*/
            error = CI2C1_SendBlock(&I2CDataToSend, I2CBufferSize, &tx_cnt);
            
            updateScreen = FALSE;
        }
    }
}

void setHorizontalMode(void)
{
	word tx_cnt;
	byte error;
	unsigned char dataToSend[16] = {0x80, 0x20, 0x80, 0x00, 0x80, 0x21, 0x80, 0, 0x80, 127, 0x80, 0x22, 0x80, 0, 0x80, 7};
	I2CIsBusy = true;
	error = CI2C1_SendBlock(&dataToSend, 16, &tx_cnt);
	while(I2CIsBusy);
}
