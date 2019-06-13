/* ###################################################################
 **     Filename    : main.c
 **     Project     : Display
 **     Processor   : MKE04Z128VLD4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2016-11-01, 12:57, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **	   Description : This version uses the calibri light
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
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
#include "Bit1.h"
#include "BitIoLdd10.h"
#include "IFsh1.h"
#include "IntFlashLdd1.h"
#include "Back_Button.h"
#include "BitIoLdd6.h"
#include "OK_Button.h"
#include "BitIoLdd7.h"
#include "Up_Button.h"
#include "BitIoLdd8.h"
#include "Down_Button.h"
#include "BitIoLdd9.h"
#include "Display.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "button.h"
#include "display_UART.h"
#include "displayMemory.h"
#include "displayPosition.h"
#include "textTable.h"
#include "OLED.h"
#include "stdbool.h"
#include "drawFunctions.h"
#include "bootstrap.h"

// Firmware & Hardware Versions
#define DU_FIRMWARE_VERSION_MAJOR	'1'
#define DU_FIRMWARE_VERSION_MINOR	'5'
#define DU_HARDWARE_VERSION_MAJOR	'5'
#define DU_HARDWARE_VERSION_MINOR	'0'

// bootload boolean
_Bool goToBootload;

// Communication Error Count
#define LOST_COMMUNICATION_COUNT    1000

/* Global Variable Declarations */

uint16_union modbus_ro_reg_rcv[RO_REG_LEN];					// read only registers controller copy
byte modbus_rw_coil_rcv[RW_COIL_LEN/8+1];					// read/write coils controller copy
uint16_union modbus_ero_reg_rcv[ERO_REG_LEN+1];		        // extended read only registers, modbus 501-509 599

// "button.h" Button states
_Bool pressedBack, pressedOK, pressedUp, pressedDown, fourButtonsPressed;
_Bool releasedBack, releasedOK, releasedUp, releasedDown;
_Bool heldUp, heldDown; // only up and down buttons have hold function

// "displayMemory.h"
_Bool refreshScreen;
_Bool updateDisplayMemory;
int userInput;
_Bool parameterIsSet;
int currentLanguage;
const unsigned char **currentTextTable = englishText;

// "displayPosition.h"
struct Location currentPosition;
_Bool updateDisplayPosition, lostCommunication;
int alarmOutput[NUMBER_OF_ALARMS];
int alarmOutputIndex, alarmStartPosition;
//_Bool isInDegradedMode;
//int coolingSPBeforeEnterDegradedModeInDegreeF, coolingSPBeforeEnterDegradedModeInDegreeC;

// "Events.h"
/* I2C */
_Bool I2CIsBusy;

/* Update Main Screen Probe (#1-#3) Reading */
_Bool TI1_mainScreenUpdateTimer_Flag;
int TI1_mainScreenUpdateTimer;

/* Company Logo timer */
_Bool TI1_showComapnyLogoTimer_Flag;
int TI1_showComapnyLogoTimer;

/* icon all on timer */
_Bool TI1_iconAllOnTimer_Flag;
int TI1_iconAllOnTimer;

/* Update Screen Timer */
_Bool TI1_updateScreenTimerIsOn;
int TI1_updateScreenTimer;
_Bool updateScreenTimerDone;
int updateScreenRate;

// Up Button
_Bool TI1_upButtonTimerIsOn;
int TI1_upButtonTimer;

// Down Button
_Bool TI1_downButtonTimerIsOn;
int TI1_downButtonTimer;

// Back button
_Bool TI1_backButtonTimerIsOn;
int TI1_backButtonTimer;

// OK Button
_Bool TI1_OKButtonTimerIsOn;
int TI1_OKButtonTimer;

// check pixels Timer
_Bool TI1_checkPixelsTimer_Flag;
int TI1_checkPixelsTimer;

// idle Timer
_Bool TI1_idleTimer_Flag;
int TI1_idleTimer, TI1_idleTimer_cnt;

// Validation Screen Timer
_Bool TI1_validationScreenTimer_Flag;
int TI1_validationScreenTimer;

// alarm Timer
_Bool TI1_alarmTimer_Flag;
int TI1_alarmTimer;

// Count Down Timer
_Bool TI1_countDownTimer_Flag;
int TI1_countDownTimer;

// UART Write Wait Timer
_Bool TI1_UARTWriteTimer_Flag;
int TI1_UARTWriteTimer;
_Bool showValidationScreen;

// "OLED.h"
unsigned char I2CDataBuffer[PAGE_NUM_MAX+1][COLUMN_NUM_MAX+1];
unsigned char I2CDataToSend[I2CBufferSize];
_Bool updateScreen;

/* Jason's variables */
bool second_flag = FALSE;
bool prev_second_flag = FALSE;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
 	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/*****************************************************************************/
	/*                           USER CODE START 								 */
	/*****************************************************************************/

	/*---------------   PART I: Global Variables Initializations   ---------------*/
	// "button.h" Button states
	pressedBack = false;
	pressedOK = false;
	pressedUp = false;
	pressedDown = false;
	releasedBack = false;
	releasedOK = false;
	releasedUp = false;
	releasedDown = false;
	heldUp = false;
	heldDown = false;

	// "displayMemory.h"
	updateDisplayMemory = false;
	refreshScreen = true;
	parameterIsSet = false;
	userInput = 0;
	currentLanguage = ENGLISH;
	currentTextTable = englishText;

	// "displayPosition.h"
	currentPosition.displayLevel = COMPANY_ICON_POSITION;
	currentPosition.lineNumber   = INLET_LINENUM;
	updateDisplayPosition = true;
	//Disable all the alarms
	for (int i = 0; i < NUMBER_OF_ALARMS; i++)
	{
		alarmOutput[i] = 0;
	}
	alarmOutputIndex = 0;
	alarmStartPosition = HIGH_TEMP_ALARM_POSITION;
	isInDegradedMode = false;
	// Set to Default cooling Set point
	//coolingSPBeforeEnterDegradedModeInDegreeF = 800;
	//coolingSPBeforeEnterDegradedModeInDegreeC = 267;

	// "Events.h"
	// I2C
	I2CIsBusy = false;

	// Update Main Screen Probe (#1-#3) Reading
	TI1_mainScreenUpdateTimer_Flag = true;
	TI1_mainScreenUpdateTimer = 0;

	// pentair icon timer
	TI1_showComapnyLogoTimer_Flag = false;
	TI1_showComapnyLogoTimer = 0;

	// icon all on timer
	TI1_iconAllOnTimer_Flag = false;
	TI1_iconAllOnTimer = 0;

	// Update Screen Timer
	TI1_updateScreenTimerIsOn = false;
	TI1_updateScreenTimer = 0;
	updateScreenTimerDone = false;
	updateScreenRate = 4000;

	// Back button
	TI1_backButtonTimerIsOn = false;
	TI1_backButtonTimer = 0;

	// OK Button
	TI1_OKButtonTimerIsOn = false;
	TI1_OKButtonTimer = 0;

	// Up Button
	TI1_upButtonTimerIsOn = false;
	TI1_upButtonTimer = 0;

	// Down Button
	TI1_downButtonTimerIsOn = false;
	TI1_downButtonTimer = 0;

	// check pixels Timer
	TI1_checkPixelsTimer_Flag = false;
	TI1_checkPixelsTimer = 0;

	// idle timer
	TI1_idleTimer_Flag = false;
	TI1_idleTimer = 0;
	TI1_idleTimer_cnt = IDLE_TIMER_COUNTER_1_MIN;

	// Validation Screen Timer
	TI1_validationScreenTimer_Flag = false;
	TI1_validationScreenTimer = 0;

	// alarm Timer
	TI1_alarmTimer_Flag = false;
	TI1_alarmTimer = 0;

	// Count Down Timer
	TI1_countDownTimer_Flag = false;
	TI1_countDownTimer = 0;

	// UART Write Wait Timer
	TI1_UARTWriteTimer_Flag = false;
	TI1_UARTWriteTimer = 0;
	showValidationScreen = false;

	// "OLED.h"
	updateScreen = true;
	OLED_init();
	for (int i = PAGE_NUM_MIN; i <= PAGE_NUM_MAX; i++)
	{
		for (int j = COLUMN_NUM_MIN; j <= COLUMN_NUM_MAX; j++)
		{
			I2CDataBuffer[i][j] = 0;
		}
	}
	for (int i = 0; i < I2CBufferSize; i++)
	{
		I2CDataToSend[i] = 0;
	}
	I2CDataToSend[0] = 0x40;

	// UART
	display_uart_init();
	LDD_TError display_uart_run_return;

	/* Send DU firmware&hardware versions to main board*/
	int displayFirmware = 0;
	displayFirmware += DU_FIRMWARE_VERSION_MAJOR;
	displayFirmware <<= 8;
	displayFirmware += DU_FIRMWARE_VERSION_MINOR;

	int displayHardware = 0;
	displayHardware += DU_HARDWARE_VERSION_MAJOR;
	displayHardware <<= 8;
	displayHardware += DU_HARDWARE_VERSION_MINOR;

	uint16 uart_write_return;
	uart_write_return = display_uart_update(REGISTER, WO_REG_START + DISPLAY_FIRMWARE_REV, false, displayFirmware, DISPLAY_FIRMWARE_REV_F, 0); // For example, 0x3031 = '0''1', means 0.1.
	uart_write_return = display_uart_update(REGISTER, WO_REG_START + DISPLAY_HARDWARE_REV, false, displayHardware, DISPLAY_HARDWARE_REV_F, 0);

	int commErrorCount = 0;
	int static bootLoadCount = 0;
	/*----------------------   PART II: MAIN LOOP START  ----------------------*/
	while (1)
	{
		if(goToBootload)
		{
			clearDisplay();
			displayBootloadMessage();
			updateScreen = TRUE;
			I2CSendData();
			updateScreen = FALSE;
			bootLoadCount++;
			if(bootLoadCount >= 1000)
			{
				goToBootload = FALSE;
				bootLoadCount = 0;
				TI1_125us_Disable(); 	//disable timer to prevent unexpected interrupts
				setupBootLoad(); //soft reset
			}
		}
		else
		{
			/* Read User Input */
			scanButton();

			/* Check Degraded Mode */
//			uint16 uart_write_return;

			/* Update Positions */
			checkDisplayPosition();

			/* Update display Memory */
			checkDisplayMemory();

			/* Communication: UART + I2C */
			display_uart_run_return = display_uart_run();

			// Check for lost communication error. After 10 consecutive communication transactions fails, Show "Display Error!" on the Main screen.
			if (display_uart_run_return)
			{
				commErrorCount+=1;
			}
			else
			{
				lostCommunication = false;
				commErrorCount = 0;
			}

			if (commErrorCount > LOST_COMMUNICATION_COUNT)
			{
				lostCommunication = true;
				commErrorCount = 0;
			}
			// I2C Communication: Drawing screens on the display
			I2CSendData();
		}
	}

	/*****************************************************************************/
	/*                           USER CODE END 								     */
	/*****************************************************************************/

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
