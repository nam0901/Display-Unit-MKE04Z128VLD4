#include "button.h"
#include "chineseFontTable.h"
#include "display_UART.h"
#include "displayMemory.h"
#include "displayPosition.h"
#include "drawFunctions.h"
#include "Events.h"
#include "fontTable.h"
#include "OLED.h"
#include "textTable.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MENU_ITEM_START_COLUMN	10

uint16_union modbus_ro_reg_rcv[RO_REG_LEN];					// read only registers controller copy
byte modbus_rw_coil_rcv[RW_COIL_LEN/8+1];					// read/write coils controller copy
int pgg;
int ncol;
_Bool isInDegradedMode;

/* ---------------------------------------------------------------------------------Function Prototypes Opening---------------------------------------------------------------------------------*/
/*********************************************************************Main Screen Display Routines and its Helper Routines Opening***************************************************************/
static void mainScreenDisplayMemory(void);																																						//
static void showWorkingStatus1(void);																																							//
static void showWorkingStatus2(void);																																							//
static void showGroupContStatus(void);																																							//
static void showDegree(unsigned char page, unsigned char column);																																//
static void showTempReading(int tempReading, unsigned char page, unsigned char column);																											//
static void showTimer(const char arial14[], unsigned char minute, unsigned char second, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column);		//
/************************************************************************************************************************************************************************************************/

/**********************************************************************Selecting Lanuage to Do the Display***************************************************************************************/
static void displayChineseAlarm(int alarmIndex);																									   											//
static void alarmDisplayMemory(int alarmPosition);																									  											//
/**************************************************************Main Screen Display Routines and its Helper Routines Closing**********************************************************************/

/*******************************************************************Main Menu Display Routines and its Helper Rounties Opening*******************************************************************/
static void showSelectArrow(void);																													   											//
static void mainMenuDisplayMemory(void);																											   											//
static void enterPasswordDisplayMemory(void);																										   											//
/****************************************************************Main Menu Display Routines and its Helper Routines Closing**********************************************************************/

/********************************************************************Helper Routine for Confirmation text****************************************************************************************/
/*these routines are helper routines that are used to figure out the colunm to make the space between the confirmation message and the				   											//
 * digit as "perfect" as possible. There are a few case to be considered because of different number of digit a number can							   											//
 * have, 0.0 to 9.9, 10.0 to 99.9, and 100 to 999. Aside from this, negative and postive number must also be considered.							   											//
 */																																					   											//
//void displayDigitForConfirmationMessageEnglish(int digit);																							  											//
//void displayDigitForConfirmationMessageOtherLanguage(int page, int col, int digit);       																										//
void doScrolling(const unsigned char textZero[], const unsigned char textOne[], const unsigned char textTwo[], const unsigned char textThree[]);												//
void cat(const unsigned char first[], const unsigned char second[], int type, int value);
/************************************************************************************************************************************************************************************************/

/******************************************************************************User Interface Declaration****************************************************************************************/
static void displayChineseUserInterface(int lineNumber);																																		//
static void userInterfaceDisplayMemory(void);																																					//
static void coolingSetPointDisplayMemory(void);																																					//
static void coolingDifferentialDisplayMemory(void);																																				//
static void heatingSetPointDisplayMemory(void);																																					//
static void heatingDifferentialDisplayMemory(void);																																				//
static void highTempAlarmDisplayMemory(void);																																					//
static void lowTempAlarmDisplayMemory(void);																																					//
static void tempScaleDisplayMemory(void);																																						//
static void hysteresisDisplayMemory(void);																																						//
static void passwordDisplayMemory(void);																																						//
static void languageDisplayMemory(void);																																						//
/************************************************************************************************************************************************************************************************/

/*****************************************************************************System Interface Declaration***************************************************************************************/
static void displayChineseSystemInterface(int lineNumber);																																		//
static void systemInterfaceDisplayMemory(void);																																					//
static void groupControlModeDisplayMemory(void);																																				//
static void modeDisplayMemory(void);																																							//
static void standAloneModeDisplayMemory(void);																																					//
static void leadLagModeDisplayMemory(void);																																						//
static void masterSlaveModeDisplayMemory(void);																																					//
static void unitIdentificationDisplayMemory(void);																																				//
static void differential2DisplayMemory(void);																																					//
static void controlStrategyDisplayMemory(void);																																					//
static void numberOfUnitsDisplayMemory(void);																																					//
static void doorSwitchDisplayMemory(void);																																						//
static void disableSwitchAlarmDisplayMemory(void);																																				//
static void alarmRelayOutputDisplayMemory(void);																																				//
static void compressorRestartDelayDisplayMemory(void);																																			//
static void compressorCurrentDisplayMemory(void);																																				//
static void evapOutTempSensorDisplayMemory(void);																																				//
static void systemSettingsDisplayMemory(void);																																					//
static void currentLimitSettingsDisplayMemory(void);																																			//
static void malfDeactivateDisplayMemory(void);																																					//
static void malfActivateDisplayMemory(void);																																					//
static void hpcFanOnDisplayMemory(void);																																						//
static void hpcFanOffDisplayMemory(void);																																						//
/************************************************************************************************************************************************************************************************/

/***********************************************************************************About Menu Declaration***************************************************************************************/
static void showVersionNumber(int versionNumber, char page, char col);																															//
static void aboutMenuDisplayMemory(void);																																						//
/************************************************************************************************************************************************************************************************/

/*----------------------------------------------------------------------------Function Prototypes Closing--------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------Function Definitions Opening--------------------------------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	checkDisplayMemory																							//
//	Returned Value	:	None																										//
//																																	//
//			Check memory for display. It checks memory for the current language and display the main screen according				//
//			to this current language. It will also check for alarm messages if alarm is active.										//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void checkDisplayMemory(void)
{
//	if (updateDisplayMemory)
//	{
//		updateDisplayMemory = false;
		if (refreshScreen)
		{
			refreshScreen = false;
			clearDisplay();
		}
		updateScreen = true;
		// set the language //
		currentLanguage = modbus_ero_reg_rcv[DISPLAY_LANGUAGE].ivalue;
		switch(currentLanguage)
		{
			case ENGLISH:	currentTextTable = englishText;	break;
			case FRENCH:	currentTextTable = frenchText;	break;
			case GERMAN:	currentTextTable = germanText;	break;
			case SPANISH:	currentTextTable = spanishText;	break;
			case ITALIAN:	currentTextTable = italianText; break;
			//add chinese language
			default: break;
		}
		//checking to see if we have a compressor restart delay that has a timing of 00:00 sec. If we do, then don't display this alarm.
		if((modbus_ro_reg_rcv[COMP_DELAY_TIMER].ivalue == 0) &&(currentPosition.displayLevel == COMPRESSOR_RESTART_DELAY_COUNT_DOWN_POSITION))
		{
			currentPosition.displayLevel = MAIN_SCREEN_POSITION;
		}
		switch (currentPosition.displayLevel)
		{
		/******************** Startup Screens **************************/
			/* Display Company Logo for 1 second at Start-up */
//			case COMPANY_ICON_POSITION:
//				drawIcon(Pentair_Logo, 128, 4, 2, 0);
//				TI1_showComapnyLogoTimer_Flag = true;
//				break;
			/*if(!(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F ) && (currentPosition.displayLevel == HEATING_SET_POINT_POSITION || currentPosition.displayLevel == HEATING_DIF_POSITION))
			{
				currentPosition.displayLevel == HIGH_TEMP_ALARM_SP_POSITION;
			}*/
			/* Display All Icons for 1 second at Start-up */
			case ICON_ALL_ON_POSITION:					displayAllOn();
														TI1_iconAllOnTimer_Flag = true;			break;
			// Main Screen
			case MAIN_SCREEN_POSITION:					mainScreenDisplayMemory();				break;
			//update main screen every second, this is handle by mainScreenUpdateTimer

			/******************** Main Menu **************************/
			case MAIN_MENU_POSITION: 					mainMenuDisplayMemory();				break;
			case USER_INTERFACE_POSITION:				userInterfaceDisplayMemory();			break;
			case SYSTEM_INTERFACE_POSITION:				systemInterfaceDisplayMemory();			break;
			case USER_PASSWORD_POSITION:
			case SYSTEM_PASSWORD_POSITION:				enterPasswordDisplayMemory();			break;
			case ABOUT_POSITION:						aboutMenuDisplayMemory();				break;

			/******************** User Interface **************************/
			case COOLING_SET_POINT_POSITION: 			coolingSetPointDisplayMemory();			break;//7
			case COOLING_DIF_POSITION: 		 			coolingDifferentialDisplayMemory();		break;//8
			case HEATING_SET_POINT_POSITION: 			heatingSetPointDisplayMemory();			break;//9
			case HEATING_DIF_POSITION: 		 			heatingDifferentialDisplayMemory();		break;//10
			case HIGH_TEMP_ALARM_SP_POSITION:			highTempAlarmDisplayMemory();			break;//19
			case LOW_TEMP_ALARM_SP_POSITION:			lowTempAlarmDisplayMemory();			break;//20
			case TEMP_SCALE_POSITION: 					tempScaleDisplayMemory();				break;//11
			case HYSTERESIS_POSITION: 					hysteresisDisplayMemory();				break;//12
			case PASSWORD_POSITION: 					passwordDisplayMemory();				break;//13
			case LANGUAGE_MENU_POSITION: 				languageDisplayMemory();				break;//2

			/******************** System Interface ************************/
			case GROUP_CONTROL_MODE_POSITION:			groupControlModeDisplayMemory();		break;
			case MODE_POSITION:							modeDisplayMemory();					break;
			case STAND_ALONE_MODE_POSITION:				standAloneModeDisplayMemory();			break;
			case LEAD_LAG_MODE_POSITION:				leadLagModeDisplayMemory();				break;
			case MASTER_SLAVE_MODE_POSITION:			masterSlaveModeDisplayMemory();			break;
			case UNIT_IDENTIFICATION_POSITION:			unitIdentificationDisplayMemory();		break;
			case DIFFERENTIAL_2_POSITION:				differential2DisplayMemory();			break;
			case CONTROL_STRATEGY_POSITION:				controlStrategyDisplayMemory();			break;
			case NUMBER_OF_UNITS_POSITION:				numberOfUnitsDisplayMemory();			break;
			case DOOR_SWITCH_POSITION:					doorSwitchDisplayMemory();				break;
			case DISABLE_SWITCH_POSITION:				disableSwitchAlarmDisplayMemory();		break;
			case ALARM_RELAY_OUTPUT_POSITION:			alarmRelayOutputDisplayMemory();		break;
			case COMPRESSOR_RESTART_DELAY_POSITION:		compressorRestartDelayDisplayMemory();	break;
			case COMPRESSOR_CURRENT_POSITION:			compressorCurrentDisplayMemory();		break;
			case EVAP_OUT_TEMP_SENSOR_POSITION:			evapOutTempSensorDisplayMemory();		break;
			// System Settings
			case DISPLAY_SYS_SETTINGS_MENU_POSITION:	systemSettingsDisplayMemory();			break;
			case CURRENT_LIMIT_SETTINGS_POSITION:		currentLimitSettingsDisplayMemory();	break;
			case MALF_DEACTIVATE_POSITION:				malfDeactivateDisplayMemory();			break;
			case MALF_ACTIVATE_POSITION:				malfActivateDisplayMemory();			break;
			case HPC_FAN_ON_POSITION:					hpcFanOnDisplayMemory();				break;
			case HPC_FAN_OFF_POSITION:					hpcFanOffDisplayMemory();				break;
			// Push four buttons at same time to turn on all pixels
			case CHECK_ALL_PIXELS:						fillDisplay();
														TI1_checkPixelsTimer_Flag = true;		break;

			/******************** Alarms ***********************/
			case HIGH_TEMP_ALARM_POSITION:case LOW_TEMP_ALARM_POSITION:case DOOR_ALARM_POSITION:
			case PHASE_MISSING_ALARM_POSITION:case HIGH_TEMP_SHUTDOWN_POSITION:
			case COMPRESSOR_THERMAL_OVERLOAD_ALARM_POSITION:
//			case OVER_VOLTAGE_ALARM_POSITION: case UNDER_VOLTAGE_ALARM_POSITION:
			case BAD_BOARD_ALARM_POSITION:
			case HIGH_PRESSURE_ALARM_POSITION:case FROST_ALARM_POSITION:
		    case DISABLE_SWITCH_ALARM_POSITION:case COIL_TEMP_SENSOR_FAULT_ALARM_POSITION:
			case INLET_TEMP_SENSOR_FAULT_ALARM_POSITION:case OUTLET_TEMP_SENSOR_FAULT_ALARM_POSITION:
			case OVER_CURRENT_ALARM_POSITION:case COMPRESSOR_RESTART_DELAY_COUNT_DOWN_POSITION:
			case LOST_COMMUNICATION_POSITION:case GROUP_CONTROL_ALARM_POSITION:
			case DEGRADED_MODE_POSITION:
				alarmDisplayMemory(currentPosition.displayLevel); //displaying alarm message on the screen
				//alarmDisplayMemory(60); //use to debug the delay message with timer counting down
				isAlarmTxt = 1; //on back, up, or down button press, return to main menu when currently on Alarm message.
				break;
			default: break;
		}
//	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name					: 	mainScreenDisplayMemory																		//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//		-isAlarmTxt : use to roll back to the main page when user click up or down button when alarm message show up				//
//		-call showWorkingStatus1 and showWorkingStatus2 to do the display of alarm messages and alarm icon							//
//																																	//
//		Display the main page contents, including the reading of temperature. It will check for lost communication					//
//		and display alarm message accordingly.																						//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mainScreenDisplayMemory(void)
{
	/* Probe#1 - #3 */
	if(isAlarmTxt)
	{
		/* if click up button during Alarm message, want to display the main page only.
		 * This will help when condenser coil temp is disable and so the enclosure temp screen will not show up*/
		currentPosition.lineNumber = INLET_LINENUM;
		isAlarmTxt = 0;
	}
	switch (currentPosition.lineNumber)
	{
		case INLET_LINENUM:
			/* Outlet Temperature Sensor Reading */
			if (isInDegradedMode)
			{
				showTempReading(modbus_ro_reg_rcv[PROBE_2].ivalue, 1, 110);
				drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
				alarmOutput[DEGRADED_MODE] = 1;//1
			}
			/* Inlet Temperature Sensor Reading */
			else
			{
				showTempReading(modbus_ro_reg_rcv[PROBE_1].ivalue, 1, 110);
				alarmOutput[DEGRADED_MODE] = 0;
			}

			/*Show Unit Indicator*/
			if(modbus_ro_reg_rcv[GROUP_CONT_MODE_STATUS].ivalue > 1)
			{
				char cchar[6] = {' ',' ',' ',' ',' ',' '};
				uint8_t a = modbus_rw_reg_rcv[UNIT_ID].ivalue;
				uint8_t b = modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue;
				switch(modbus_ro_reg_rcv[GROUP_CONT_MODE_STATUS].ivalue)
				{
					case 2:	sprintf(cchar, "%d", a);		break; //lead/lag
					case 3:	sprintf(cchar, "%d:%d", a, b);	break; //master/slave
					default: break;
				}
				displayTextInOneLine(cchar, arial14, 0, 2);
			}

			showWorkingStatus1();	/* Working Status 1 */
			showWorkingStatus2();	/* Working Status 2 */
			showGroupContStatus();	/* Group Control Status */

			/* Check Lost Communication */
			if (lostCommunication)
			{
				drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
				alarmOutput[LOST_COMMUNICATION] = 1;
			}
			else
			{
				alarmOutput[LOST_COMMUNICATION] = 0;
			}
			TI1_alarmTimer_Flag = true;
			break;

		case OUTLET_LINENUM:
			TI1_alarmTimer_Flag = false;
			TI1_alarmTimer = 0;
			/*if(modbus_ro_reg_rcv[WORKING_STATUS_1].ivalue & 0x8000)
			{
				break;
			}*/
			highlightOneLine(0);
			if (currentLanguage != CHINESE)
			{
				doScrolling(currentTextTable[TEXT_INDEX_EnclosureOutletTemp], "\0", "\0", "\0");
			}
			else
			{
				displayChineseTextInOneLineHighlighted(probeNameChinese, 0, 7, 0, 0, 10);
			}
			showTempReading(modbus_ro_reg_rcv[PROBE_2].ivalue, 3, 105);
			break;

		case COIL_LINENUM:
			// Turn off alarm timer and clear its counter
			TI1_alarmTimer_Flag = false;
			TI1_alarmTimer = 0;
			highlightOneLine(0);
			if (currentLanguage != CHINESE)
			{
				doScrolling(currentTextTable[TEXT_INDEX_CondenserCoilTemp], "\0", "\0", "\0");
			}
			else
			{
				displayChineseTextInOneLineHighlighted(probeNameChinese, 8, 14, 0, 0, 10);
			}
			showTempReading(modbus_ro_reg_rcv[PROBE_3].ivalue, 3, 105);
			break;

		default: break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: showWorkingStatus1																							//
//	Returned Value	: None																											//
//																																	//
//	This routine will check for the read only register modbus_ro_reg_rcv, an array reading from the controller board,				//
//	for workingStatus1 value. This value is a 16 bits number and is the 9th element of the array modbus_ro_reg_rcv.					//
//	Within this 16 bits, each bit corresponds to a different alarm message. That is, bit15 (0x0001) correspondes to					//
//	"Phase Missing" alarm message, bit14 (0x0002) correspondes to "MCU Health Fail" alarm message, which we did not					//
// 	supported currently. Skipping to thelast bit, bit0 (0x8000), it correspondes to "Outlet Temp Senor Fault"						//
//	alarm message.																													//
//																																	//
// 	 				Bit Number		Register						Masking Flag													//
//					bit15			Phase Missing						0x0001														//
//					bit14			MCU Health Fail						0x0002														//
//					bit13			Imbalance Voltage					0x0004														//
//					bit12			Over Voltage						0x0008														//
//					bit11			Under voltage						0x0010														//
//					bit10			Door Alarm							0x0020														//
//					bit9			Coil Temp Senor fault			 	0x0040														//
//					bit8			HPCLowCoilTemp state			 	0x0080														//
//					bit7			Compressor Thermal Overload			0x0100														//
//					bit6			Bad Board							0x0200														//
//					bit5			Reverse Relay On				 	0x0400														//
//					bit4			MalfHighTemp Fault				 	0x0800														//
//					bit3			High Temp Shutdown Alarm			0x1000														//
//					bit2			Forward Relay On					0x2000														//
//					bit1			Inlet Temp Sensor Fault			 	0x4000														//
//					bit0			Outlet Temp Sensor Fault			0x8000														//
//																																	//
//	This 16 bits number is being save in a local variable workingStatus1. This variable is being used to check for					//
//	possible alarm message. If there is an alarm message, we save it in an array called alarmOutput. This alarmOutput				//
//	can then be used to do the display of alarm message in the DU.																	//
//																																	//
//	Looking at alarmOutput[PHASE_MISSING] = 1 means that we have a alarm message of "Phase Missing" and etc. Each					//
//	alarm message will draw an bell icon.																							//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showWorkingStatus1(void)
{
	int workingStatus1 = modbus_ro_reg_rcv[WORKING_STATUS_1].ivalue;
	int ID2WorkStatus = modbus_ro_reg_rcv[ID2_WORK_STATUS].ivalue;

	//	0 - Phase Missing
	if (workingStatus1 & 0x0001)
	{
		alarmOutput[PHASE_MISSING] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[PHASE_MISSING] = 0;
	}

	// 	1 - Not used

	//	2 - Imbalance Voltage

	//	3 - Over Voltage
	if (workingStatus1 & 0x0008)
	{
		alarmOutput[OVER_VOLTAGE] = 0;
		//drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN); //comment out since don't concern about this feature at the current unit
	}
	else
	{
		alarmOutput[OVER_VOLTAGE] = 0;
	}

	//	4 - Under Voltage
	if (workingStatus1 & 0x0010)
	{
		alarmOutput[UNDER_VOLTAGE] = 0;
		//drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN); //comment out since don't concern about this feature at the current unit
	}
	else
	{
		alarmOutput[UNDER_VOLTAGE] = 0;
	}

	//	5 - Door/Smoke Alarm
	if ((workingStatus1 & 0x0020) ||  (ID2WorkStatus & 0x0020))
	{
		alarmOutput[DOOR_SMOKE] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[DOOR_SMOKE] = 0;
	}

	//	6 - Coil Temp Sensor Fault
	if (workingStatus1 & 0x0040)
	{
		alarmOutput[COIL_TEMP_SENSOR_FAULT] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[COIL_TEMP_SENSOR_FAULT] = 0;
	}

	//	7 - HPC Low Coil Temp State

	//	8 - Compressor Thermal Overload
	if (workingStatus1 & 0x0100)
	{
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
		alarmOutput[COMPRESSOR_THERMAL_OVERLOAD] = 1;
	}
	else
	{
		alarmOutput[COMPRESSOR_THERMAL_OVERLOAD] = 0;
	}

	//	9 - Bad Board
	if (workingStatus1 & 0x0200)
	{
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
		alarmOutput[BAD_BOARD] = 1;
	}
	else
	{
		alarmOutput[BAD_BOARD] = 0;
	}

	//	10 - Reverse Relay On

	//	11 - High Pressure Alarm / MalfHigh Temp Fault
	if (workingStatus1 & 0x0800)
	{
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
		alarmOutput[HIGH_PRESSURE] = 1;
	}
	else
	{
		alarmOutput[HIGH_PRESSURE] = 0;
	}

	//	12 - High Temp Shutdown
	if (workingStatus1 & 0x1000)
	{
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
		alarmOutput[HIGH_TEMP_SHUTDOWN] = 1;
	}
	else
	{
		alarmOutput[HIGH_TEMP_SHUTDOWN] = 0;
	}

	//	13 - Forward Relay On

	//	14 - Inlet Temp Sensor Fault
	if (workingStatus1 & 0x4000)
	{
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
		alarmOutput[INLET_TEMP_SENSOR_FAULT] = 1;
	}
	else
	{
		alarmOutput[INLET_TEMP_SENSOR_FAULT] = 0;
	}

	//	15 - Outlet Temp Sensor Fault
	if (workingStatus1 & 0x8000)
	{
		alarmOutput[OUTLET_TEMP_SENSOR_FAULT] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[OUTLET_TEMP_SENSOR_FAULT] = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	showWorkingStatus2																							//
//	Returned Value	: 	None																										//
//																																	//
//	Same as showWorkingStatus1																										//
//																																	//
//  				Bit Number		Register							Masking Flag												//
//					****************************************************************												//
//					bit15			Frost Alram							0x0001														//
//					bit14			Degraded Mode						0x0002														//
//					bit13			Not Used							0x0004														//
//					bit12			Diagnostic Mode						0x0008														//
//					bit11			Low Temp Alarm						0x0010														//
//					bit10			High Temp Alarm						0x0020														//
//					bit9			Disable Switch Alarm			 	0x0040														//
//					bit8			Over Current					 	0x0080														//
//					bit7			Cooling On							0x0100														//
//					bit6			Not Used							0x0200														//
//					bit5			Not Used						 	0x0400														//
//					bit4			Condensor MI 2 On (not used)	 	0x0800														//
//					bit3			Heater On (optional)				0x1000														//
//					bit2			Condensor MI On						0x2000														//
//					bit1			Evap MI On						 	0x4000														//
//					bit0			Compressor On						0x8000														//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showWorkingStatus2(void)
{
	int workingStatus2 = modbus_ro_reg_rcv[WORKING_STATUS_2].ivalue;

	//	0 - Frost Alarm
	if (workingStatus2 & 0x0001)
	{
		alarmOutput[FROST] = 1;
	}
	else
	{
		alarmOutput[FROST] = 0;
	}

	//	1 - Degraded mode
	if (workingStatus2 & 0x0002)
	{
		isInDegradedMode = true;
	}
	else
	{
		isInDegradedMode = false;
	}
	//	2 - Not used

	//	3 - Diagnostics Mode

	//	4 - Low Temp Alarm
	if (workingStatus2 & 0x0010)
	{
		alarmOutput[LOW_TEMP] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[LOW_TEMP] = 0;
	}

	//	5 - High Temp Alarm
	if (workingStatus2 & 0x0020)
	{
		alarmOutput[HIGH_TEMP] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[HIGH_TEMP] = 0;
	}

	//	6 - Disable Switch Alarm
	if (workingStatus2 & 0x0040)
	{
		alarmOutput[DISABLE_SWITCH_ALARM] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[DISABLE_SWITCH_ALARM] = 0;
	}

	//	7 - Over Current
	if (workingStatus2 & 0x0080)
	{
		alarmOutput[OVER_CURRENT] = 1;
		drawIcon(alarmIcon, 14, 2, BELL_ICON_PAGE, BELL_ICON_COLUMN);
	}
	else
	{
		alarmOutput[OVER_CURRENT] = 0;
	}

	//	8 - Cooling On
	if ((workingStatus2 & 0x0100) && modbus_ro_reg_rcv[COMP_DELAY_TIMER].ivalue > 0)
	{
		alarmOutput[COMPRESSOR_COUNT_DOWN] = 1;
	}
	else
	{
		alarmOutput[COMPRESSOR_COUNT_DOWN] = 0;
	}

	//	9 - Not Used

	//	10 - Not Used

	//	11 - Not Used

	//	12 - Heater On
	if (workingStatus2 & 0x1000)
	{
		drawIcon(heaterIcon, 14, 2, HEATER_ICON_PAGE, HEATER_ICON_COLUMN);
	}
	else
	{
		clearArea(HEATER_ICON_PAGE, HEATER_ICON_COLUMN, HEATER_ICON_PAGE+1, HEATER_ICON_COLUMN+14);
	}

	//	13 - Condensor MI On
	if (workingStatus2 & 0x2000)
	{
		drawIcon(condensorIcon, 14, 2, CONDENSOR_ICON_PAGE, CONDENSOR_ICON_COLUMN);
	}
	else
	{
		clearArea(CONDENSOR_ICON_PAGE, CONDENSOR_ICON_COLUMN, CONDENSOR_ICON_PAGE+1, CONDENSOR_ICON_COLUMN+14);
	}

	//	14 - Evap MI On
	if (workingStatus2 & 0x4000)
	{
		drawIcon(evapIcon, 14, 2, EVAPORATOR_ICON_PAGE, EVAPORATOR_ICON_COLUMN);
	}
	else
	{
		clearArea(EVAPORATOR_ICON_PAGE, EVAPORATOR_ICON_COLUMN, EVAPORATOR_ICON_PAGE+1, EVAPORATOR_ICON_COLUMN+14);
	}

	//	15 - Compressor On
	if (workingStatus2 & 0x8000)
	{
		drawIcon(compIcon, 14, 2, COMPRESSOR_ICON_PAGE, COMPRESSOR_ICON_COLUMN);
	}
	else
	{
		clearArea(COMPRESSOR_ICON_PAGE, COMPRESSOR_ICON_COLUMN, COMPRESSOR_ICON_PAGE+1, COMPRESSOR_ICON_COLUMN+14);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	showGroupContStatus																							//
//	Returned Value	: 	None																										//
//																																	//
//	Same as showWorkingStatus1 & showWorkingStatus2																										//
//																																	//
//  				Bit Number		Register							Masking Flag												//
//					****************************************************************												//
//					bit15			Group Control Lost Communication	0x0001														//
//					bit14			Unit 1 Present						0x0002														//
//					bit13			Unit 2 Present						0x0004														//
//					bit12			Unit 3 Present						0x0008														//
//					bit11			Unit 4 Present						0x0010														//
//					bit10			Unit 5 Present						0x0020														//
//					bit9			Unit 6 Present			 			0x0040															//
//					bit8			Unit 7 Present					 	0x0080														//
//					bit7			Unit 8 Present						0x0100														//
//					bit6			Unit 9 Present						0x0200														//
//					bit5			Unit 10 Present						0x0400														//
//					bit4			Unit 11 Present	 					0x0800														//
//					bit3			Unit 12 Present						0x1000														//
//					bit2			Unit 13 Present						0x2000														//
//					bit1			Unit 14 Present						0x4000														//
//					bit0			Unit 15 Present						0x8000														//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showGroupContStatus(void)
{
	int groupContStatus = modbus_ro_reg_rcv[GROUP_CONT_STATUS].ivalue;
		//	0 - Unit 1 Present
		if (groupContStatus & 0x0001)
		{	alarmOutput[GROUP_CONTROL] = 1;	}
		else
		{	alarmOutput[GROUP_CONTROL] = 0;	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		:	showDegree																								//
//	Returned Value		: 	None																									//
// 	Passing Parameters	:	page and column																							//
//																																	//
//		page			: 	row position	(only have 4 pages, each pages has a height of 2 bits)									//
//		column			:	column position																							//
//																																	//
//		DU can display a 2-D character, the variable page and column are used for this supposes. The variable page is				//
//		used to sinify the row location of where the degree icon supposed to be drawed. The column signifies the					//
//		position of the column. This routine will check for degree F and degree C before drawing the icon.							//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showDegree(unsigned char page, unsigned char column)
{
	if (modbus_rw_coil_rcv[UNIT_OF_MEASURE/8] & UNIT_OF_MEASURE_F) 	//degree F
	{	drawIcon(degreeF, 8, 2, page, column);	}
	else															//degree C
	{	drawIcon(degreeC, 8, 2, page, column);	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		: 	showTempReading																							//
//	Returned Value		:	None																									//
// 	Passing Parameters	:	tempReading, page, column																				//
//																																	//
//		TempReading		:	temperature value to be display																			//
//		page			: 	row positon																								//
//		column			: 	column position																							//
//																																	//
//		This routine will display the temperature value store in the parameter TempReading using the location						//
//		specifies by page and column parameters. It will check the upper and lower bound, 10000 and -10000 respectively.			//
//		It checks for positive and negative temperature values and then call the appropriate drawing routine to						//
//		do the display.																												//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showTempReading(int tempReading, unsigned char page, unsigned char column)
{
	clearOneLine(page);
	clearOneLine(page+2);
	if (tempReading >= 10000) //maximum
	{
		drawIntegerRightAligned(digitCalibriLight4x28, 999, 4, 28, page, column);
	}
	else if (tempReading <= -10000) //minimum
	{
		drawIntegerRightAligned(digitCalibriLight4x28, -999, 4, 28, page, column);
	}
	else if ((tempReading < 10) && (tempReading > -10)) //two digits
	{
		drawIntegerRightAligned(digitCalibriLight4x28, tempReading, 4, 28, page, column);
		clearArea(page+1, column-28-NEGATIVE_SIGN_WIDTH, page+2, column-28);
		drawIcon(decimal_point, DECIMAL_POINT_WIDTH, 1, page+3, column-28);
		drawDigit(digitCalibriLight4x28, 0, 4, 28, page, column-28*2);
		if (tempReading < 0)
		{
			drawIcon(negativeSign, NEGATIVE_SIGN_WIDTH, 2, page+1, column-28*2-DECIMAL_POINT_WIDTH-NEGATIVE_SIGN_WIDTH);
		}
	}
	else if ((tempReading >= 1000) || (tempReading <= -1000))
	{
		drawIntegerRightAligned(digitCalibriLight4x28, tempReading/10, 4, 28, page, column);
	}
	else
	{
		drawIntegerRightAligned(digitCalibriLight4x28, tempReading, 4, 28, page, column);
		if (tempReading != 0)
		{
			drawIcon(decimal_point, DECIMAL_POINT_WIDTH, 1, page+3, column-28);
		}
	}
	showDegree(page,column+5);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	 	:	showTimer																								//
//	Return Value		: 	None																									//
//	Passing Parmeters	:	arial14[], minute,	second,	heightInPages, widthInRows, page, and column								//
//		arial14[]		: 	A predefined arial of digits in frontTable																//
//		minute			:	Number in minute position																				//
//		second			:	Number in second position																				//
//		heightInPages	:	The height of the pages or number of bits used to form a page											//
//		widthInRows		: 	The width of each rows to be used																		//
//		page			: 	Row position																							//
//		column			:	Column positon																							//
//																																	//
//	The height of the page will be determine by heightInPages (need this because display on the main page need the					//
//	temperature to be bigger than 2 bits page). The width of the column will be determined by widthInRows.							//
//	Both the parameters, minute and second, will be checking for upper and lower bound before calling to drawing					//
//	routine.																														//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showTimer(const char arial14[], unsigned char minute, unsigned char second, unsigned char heightInPages, unsigned char widthInRows, unsigned char page, unsigned char column)
{
	if (minute >= 100)		{	minute = 99;	}
	else if (minute <= -1)	{	minute = 0;		}

	if (second >= 60)		{	second = 59;	}
	else if (second <= -1)	{	second = 0;		}

	// Minute
	drawDigit(arial14, minute/10, heightInPages, widthInRows, page, column);
	drawDigit(arial14, minute%10, heightInPages, widthInRows, page, column+widthInRows);
	// :
	drawIcon(decimal_point, DECIMAL_POINT_WIDTH, 1, page, column + widthInRows*2+2);
	drawIcon(decimal_point, DECIMAL_POINT_WIDTH, 1, page+3, column + widthInRows*2+2);
	// second
	drawDigit(arial14, second/10, heightInPages, widthInRows, page, column + widthInRows*2+6);
	drawDigit(arial14, second%10, heightInPages, widthInRows, page, column + widthInRows*3+6);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		:	displayChineseAlarm																						//
//	Returned Value		:	None																									//
//	Passing Parameter	:	alarmIndex																								//
//		alarmIndex		:	use to determine which alarm is currently being presented in the switch statement.						//
//																																	//
//	Given the passing parameter alarmIndex, alarm message can be determine using case in a switch statment. Each case				//
//	correspondes to a different alarm message. This routine is only for Chinese.													//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayChineseAlarm(int alarmIndex)
{
	switch(alarmIndex)
	{
	// Note: Display attention icon after display a line of chinese characters, because displayChineseTextInOneLine() will first clear the line.
		case HIGH_TEMP:
			displayChineseTextInOneLine(highTempChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case LOW_TEMP:
			displayChineseTextInOneLine(lowTempChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case DOOR_SMOKE:
			displayChineseTextInOneLine(doorSmokeAlarmChinese, 0, 5, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case PHASE_MISSING:
			displayChineseTextInOneLine(phaseMissingChinese, 0, 1, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case HIGH_TEMP_SHUTDOWN:
			displayChineseTextInOneLine(highTempShutDownChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case COMPRESSOR_THERMAL_OVERLOAD:
			displayChineseTextInOneLine(compThermalOverloadChinese, 0, 6, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case OVER_VOLTAGE:
			displayChineseTextInOneLine(overVoltageChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case UNDER_VOLTAGE:
			displayChineseTextInOneLine(underVoltageChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case BAD_BOARD:
			displayChineseTextInOneLine(badBoardChinese, 0, 5, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case HIGH_PRESSURE:
			displayChineseTextInOneLine(highPressureChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case FROST:
			displayChineseTextInOneLine(frostAlarmChinese, 0, 5, 0, 1, 22);
			drawIcon(attentionIcon, 16, 2, 1, 5);
			displayChineseTextInOneLine(frostAlarmChinese, 6, 11, 0, 3, 22);
			displayChineseTextInOneLine(frostAlarmChinese, 12, 13, 0, 5, 22);
			break;

		case COIL_TEMP_SENSOR_FAULT:
			displayChineseTextInOneLine(coilTempSensorFaultChinese, 0, 4, 0, 2, 22);
			drawIcon(attentionIcon, 16, 2, 2, 5);
			displayChineseTextInOneLine(coilTempSensorFaultChinese, 5, 9, 0, 4, 22);
			break;

		case INLET_TEMP_SENSOR_FAULT:
			displayChineseTextInOneLine(inletTempSensorFaultChinese, 0, 5, 0, 2, 22);
			drawIcon(attentionIcon, 16, 2, 2, 5);
			displayChineseTextInOneLine(inletTempSensorFaultChinese, 6, 12, 0, 4, 22);
			break;

		case OUTLET_TEMP_SENSOR_FAULT:
			displayChineseTextInOneLine(outletTempSensorFaultChinese, 0, 5, 0, 2, 22);
			drawIcon(attentionIcon, 16, 2, 2, 5);
			displayChineseTextInOneLine(outletTempSensorFaultChinese, 6, 12, 0, 4, 22);
			break;

		case OVER_CURRENT:
			displayChineseTextInOneLine(overCurrentChinese, 0, 6, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case DISABLE_SWITCH_ALARM:
			displayChineseTextInOneLine(disableSwitchAlarmChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		case LOST_COMMUNICATION:
			displayChineseTextInOneLine(lostCommunicationChinese, 0, 3, 0, 3, 22);
			drawIcon(attentionIcon, 16, 2, 3, 5);
			break;

		/*case DEGRADED_MODE:
			displayChineseTextInOneLine(degradedModeChinese, 0, 6, 0, 2, 22);
			displayChineseTextInOneLine(degradedModeChinese, 7, 13, 0, 4, 22);
			drawIcon(attentionIcon, 16, 2, 2, 5);
			break;*/

		default: break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		:	alarmDisplayMemory																						//
//	Returned Value		: 	None																									//
//	Passing Parameter	:	alarmPosition																							//
//		alarmPostion	: 	Parameter used to determine which of the alarm message it is											//
//																																	//
//	This routine is for all other language that is not Chinese. It work the same way as for Chinese version above as well.			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alarmDisplayMemory(int alarmPosition)
{
	//Restart Delay Active Alarm
	int alarmTextIndex = alarmPosition - HIGH_TEMP_ALARM_POSITION;
	if (alarmTextIndex == 16) // restart delay, the 16 is a pre-defined macro in displayPosition.h for alarm relay
	{
		drawIcon(attentionIcon, 16, 2, 2, 5);
		if(currentLanguage != CHINESE)
		{
			cat(currentTextTable[TEXT_INDEX_RestartDelayActive],
				"\0",
				MIN_SEC_COLON,
				modbus_ro_reg_rcv[COMP_DELAY_TIMER].ivalue);
		}
		else
		{
//			displayChineseTextInOneLine(compressorCountDownChinese, 0, 4, 0, 2, column);
//			displayChineseTextInOneLine(compressorCountDownChinese, 5, 7, 0, 4, column + widthInRows*4+10);
		}
		// update timer every one second
		TI1_countDownTimer_Flag = true;
	}
	//Every other Alarm
	else
	{
		if (currentLanguage != CHINESE)
		{	displayTextInMultipleLines(currentTextTable[alarmTextIndex], arial14, &pgg, &ncol, true);	}
		else
		{	displayChineseAlarm(alarmTextIndex);	}
	}
	// Turn on alarm timer
	TI1_alarmTimer_Flag = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		:	showSelectArrow																							//
//	Returned Value		: 	None																									//
//																																	//
//	Variable used		:	SysSettingArrowPosition and SysIntArrowPosition															//
//			Both variable are used to make the text fit the DU screen nicely. SysSettingArrowPosition is used for System			//
//			Setting menu position. SysIntArrowPosition is used for System Interface menu position. Theses two variables				//
//			are needed since some of translations are longer than the standard space make for these menu display, thus these		//
//			two variables allow the program to adjust, move the column closer to column0, column to make the translated text		//
//			fit the screen nicely.																									//
//																																	//
//	This routine will draw the arrow that aid the selection of content. It will take the current lineNumber and mod 3 to get		//
//	determine the position of where the arrow suppose to be showing. Mod 3 is chosen since we can only display up to three			//
//	lines of contents. DU has 8 bits in height and 128 bits in width. This program is written in such that each page has a			//
//	height of 2 bits. With the 8 bits, we will have 4 pages. As for column, it depends on the width of each characters that			//
//	needed to be display.																											//
//																																	//
//	With the 4 pages, one page2, page4, and page6 are used for display the content of each menu. Page0 is used to display the		//
// 	of the menu currently being display on the screen. Consequently, there are three cases to consider and thus resulting in		//
//	a modulo of 3.																													//
//			1%3  = 1	: page 2																									//
//			2%3  = 2	: page 4																									//
//			3%3  = 0	: page 6																									//
//			4%3  = 1	: page 2																									//
//			5%3  = 2	: page 4																									//
//			6%3  = 0	: page 6																									//
//			7%3  = 1	:	.																										//
//			8%3  = 2	:  	.																										//
//			10%3 = 1	:	.																										//
//		From above, any number modulo 3 that result in the 0 will always be in page6. So so this give rise to the first case.		//
//		As for the second case, page2 and page4, there are need to calculate the correct page to draw the arrow. This can be		//
//		accomplished by (currentPosition.lineNumber%3)*2. Thus the second case has calculated page as this.							//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showSelectArrow(void)
{
	if (currentPosition.lineNumber%3 == 0)
	{
		if(SysSettingArrowPosition||SysIntArrowPosition)
		{
			SysSettingArrowPosition = false;
			drawIcon(selectArrow, 5, 2, 6, 1);
		}
		else
		{
			drawIcon(selectArrow, 5, 2, 6, 3);
		}
	}
	else
	{
		if(SysIntArrowPosition ||SysSettingArrowPosition)
		{
			SysIntArrowPosition = false;
			drawIcon(selectArrow, 5, 2, (currentPosition.lineNumber%3)*2, 1);
		}
		else
		{
			drawIcon(selectArrow, 5, 2, (currentPosition.lineNumber%3)*2, 3);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	mainMenuDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routine is used to display the main menu and its contents. It will call showSelectArrow to show the arrow when move		//
//	from one line to the next.																										//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenuDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_MainMenu], currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_SystemInterface], currentTextTable[TEXT_INDEX_About]);
	}
	else
	{
		// Title:
		displayChineseTextInOneLineHighlighted(mainMenuChinese, 0, 2, 0, 0, 10);
		// Line 1: User Interface
		displayChineseTextInOneLine(mainMenuChinese, 3, 6, 0, 2, MENU_ITEM_START_COLUMN);
		// Line 2: System Interface
		displayChineseTextInOneLine(mainMenuChinese, 7, 10, 0, 4, MENU_ITEM_START_COLUMN);
		// Line 3: About
		displayChineseTextInOneLine(mainMenuChinese, 11, 14, 0, 6, MENU_ITEM_START_COLUMN);
	}
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	enterPasswordDisplayMemory																					//
//	Returned Value 	:	None																										//
//																																	//
//	This routine will display the "Enter Passcode" in page0 with higghtlighted. It will draw two 00 in the middle screen on			//
// 	row 3 out of 7 (0, 1, 2, 3, 4, 5, 6, 7).																						//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void enterPasswordDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_EnterPasscode], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(enterPasswordChinese, 0, 4, 0, 0, 20);
	}

	if (userInput >= 0 && userInput <= 9) //drawing the two 00 in row 3
	{
		drawDigit(digitCalibriLight4x28, userInput, 4, 28, 3, 60); //drawing the second place 0
		drawDigit(digitCalibriLight4x28, 0, 4, 28, 3, 30); //drawing the first place 0
	}
	else
	{
		drawIntegerLeftAligned(digitCalibriLight4x28, userInput, 4, 28, 3, 30);
	}
}

/******************************************Helper Routine Used in User Interface Opening*********************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																															   	 	//
//	Function Name			:	displayDigitForConfirmationMessageEnglish													    	//
//	Returned Value			:	None																						    	//
//	Passing Parameter		:	digit																						   	 	//
//			digit			:	number to be display, positive or negative													    	//
//	Local variables			:	colNumDigit, colNumDecimal, and colNum                                                         		//
//			colNumDigit		:	The start column position of the highest place (tenth and hundredth) of the digit to be display 	//
//			colNumDecimal	:	The start column of decimal symbol. 														    	//
//			colNum			: 	The start column of the degree icon, degree F or degree C									    	//
//																															    	//
//	This routine take the digit to be displayed and adjust the space according to the type of digit. Each digit has different  	 	//
//	spacing number to make them look nice when display after a text message. There are positive and negative numbers which      	//
//	need to check. It is assumed that the range is from -99 to 999. For positve number, there are three case: 0.0-9.9,          	//
//	10.0-99.9, and 100-999. Each of these cases involves adjusting the spacing between the text message and digit differently.  	//
//	For negative number, there are two cases: -1.0 to -9.9 and -10.0 to -99.9. Just like the positive case, spacing is         	 	//
//	different from one cases to the next. The local variables listing above will be used to adjust to each cases.               	//
//                                                                                                                              	//
//	This is only for English language                                                                                           	//
// 	                                                                                                                            	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void displayDigitForConfirmationMessageEnglish(int digit)
//{
//	char colNumDigit;
//	char colNumDecimal;
//	char colNum;
//	negativeDecimalZero = false;
//	if(digit < 0) //negative temperature
//	{
//		//-10.0 to -99.9 range ------------------------------------------------------
//		if((digit/10) <= -10 && (digit/10) > -100)
//		{
//			colNumDigit = 65;
//			colNumDecimal = colNumDigit+11;
//			colNum = 90;
//		}
//		//-1.0- to -9.9 range -------------------------------------------------------
//		if((digit/10) < 0 && (digit/10) > -10 )
//		{
//			colNumDecimal = colNumDigit+5;
//			colNum = 82;
//		}
//
//		// -0.1 to -0.9 range -------------------------------------------------------
//		if (digit/10 == 0)
//		{
//			colNumDigit = 80;
//			negativeDecimalZero = true;
//			colNumDecimal = 68;
//			colNum = 85;
//		}
//		confirmDigitDisplay = true;
//		drawIntegerRightAligned(digitCourier2x7, digit/10, 2, 7, 4, colNumDigit);
//		confirmDigitDisplay = false;
//		drawCharacter('.', arial14, 4, colNumDecimal);
//		drawDigit(digitCourier2x7, -digit%10, 2, 7, 4, colNumDecimal+3);
//	}
//	else //positive temperature
//	{
//		//negativeDecimalZero = false;
//		_Bool threeDigit = false;
//		//100 to 999 range -------------------------------------------------------------
//		if(digit/10 >= 100 && digit/10 < 1000)
//		{
//			colNumDigit = 73;
//			colNum = colNumDigit+5;
//			colNumDecimal = colNumDigit-3;
//			threeDigit = true;
//		}
//
//		//10.0 to 99.99 range ---------------------------------------------------------
//		if(digit/10 >= 10 && digit/10 < 100)
//		{
//			colNumDigit = 75;
//			colNum = colNumDigit+10;
//			colNumDecimal = colNumDigit-5;
//		}
//
//		//0.0 to 9.9 range ------------------------------------------------------------
//		if(digit/10 < 10)
//		{
//			colNumDigit = 70;
//			colNumDecimal = colNumDigit-5;
//			colNum = colNumDecimal+2+10;
//		}
//		confirmDigitDisplay = true;
//		drawIntegerRightAligned(digitCourier2x7, digit/10, 2, 7, 4, colNumDigit);
//		confirmDigitDisplay = false;
//		if(!threeDigit)
//		{
//			drawCharacter('.', arial14, 4, colNumDecimal);
//			drawDigit(digitCourier2x7, digit%10, 2, 7, 4, colNumDecimal+2);
//		}
//	}
//	showDegree(4, colNum);
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		: 	displayDigitForConfirmationMessage															//
// 	Returned Value		:	None																									//
//	Passing Parameter	:	page, col, digit																						//
//			page		:	page to display the digit																				//
//			col			: 	start column to display the digit																		//
//			digit		: 	digit to be display																						//
//																																	//
//	This routine be used to adjust the spacing between text message and digit. This works the same as the one used for English		//
//	language. The functionality is the same. However, two more parameters are added. These two parameters are used in figuring 		//
//	out what page and column to start displaying the digit.																			//
//																																	//
//	Page and column paramters are the last page and column used by the text message. 												//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void displayDigitForConfirmationMessageOtherLanguage(int page, int col, int digit)
//{
//	char colNumDigit;
//	char colNumDecimal;
//	char colNum;
//	int checkColNum = col;
//	negativeDecimalZero = false;
//	if(digit < 0) //negative temperature
//	{
//		//-10.0 to -99.9 range ------------------------------------------------------
//		if((digit/10) <= -10 && (digit/10) > -100 )
//		{
//			colNumDigit = col+13;
//			colNumDecimal = colNumDigit+11;
//			colNum = colNumDigit+23;
//			checkColNum = col + 4*7 + 3 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			if(checkColNum > 127)
//			{
//				colNumDigit = 14;
//				colNumDecimal = colNumDigit+12;
//				colNum = colNumDecimal + 12;
//				page += 2;
//			}
//		}
//		//-1.0 to -9.9 range -------------------------------------------------------
//		if((digit/10) < 0 && (digit/10) > -10 )
//		{
//			checkColNum = col + 3*7 + 3 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			colNumDigit = col+13;
//			colNumDecimal = colNumDigit+6;
//			colNum = colNumDecimal+13;
//			if(checkColNum > 127)
//			{
//				colNumDigit = 13;
//				colNumDecimal = colNumDigit+6;
//				colNum = colNumDecimal + 12;
//				page += 2;
//			}
//		}
//
//		// -0.1 to -0.9 range -------------------------------------------------------
//		if (digit/10 == 0)
//		{
//			checkColNum = col + 3*7 + 3 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			colNumDigit = col+27;
//			negativeDecimalZero = true;
//			colNumDecimal = colNumDigit-12;
//			colNum = colNumDecimal+14;
//			if(checkColNum > 127)
//			{
//				colNumDigit = 27;
//				colNumDecimal = colNumDigit-12;
//				colNum = colNumDecimal + 13;
//				page +=2;
//			}
//		}
//		confirmDigitDisplay = true;
//		drawIntegerRightAligned(digitCourier2x7, digit/10, 2, 7, page, colNumDigit);
//		confirmDigitDisplay = false;
//		drawCharacter('.', arial14, page, colNumDecimal);
//		drawDigit(digitCourier2x7, -digit%10, 2, 7, page, colNumDecimal+3);
//	}
//	else //positive temperature
//	{
//		//negativeDecimalZero = false;
//		_Bool threeDigit = false;
//		//100 to 999 range -------------------------------------------------------------
//		if(digit/10 >= 100 && digit/10 < 1000)
//		{
//			checkColNum = col + 3*7 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			colNumDigit = col+19;
//			colNum = colNumDigit+7;
//			colNumDecimal = colNumDigit-3;
//			threeDigit = true;
//			if(checkColNum >127)
//			{
//				colNumDigit = 20;
//				colNumDecimal = colNumDigit-5;
//				colNum = colNumDecimal+10;
//				page += 2;
//			}
//		}
//
//		//10.0 to 99.9 range ---------------------------------------------------------
//		if(digit/10 >= 10 && digit/10 < 100)
//		{
//			checkColNum = col + 3*7 + 3 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			colNumDigit = col+23;
//			colNum = colNumDigit+10;
//			colNumDecimal = col+18;
//			if(checkColNum >127)
//			{
//				colNumDigit = 24;
//				colNumDecimal = colNumDigit-4;
//				colNum = colNumDecimal+11;
//				page += 2;
//			}
//		}
//
//		//0.0 to 9.9 range ------------------------------------------------------------
//		if(digit/10 < 10)
//		{
//			checkColNum = col + 2*7 + 3 + 3 + 3 +8; //all space taken for display of digit number and the degree icon
//			colNumDigit = col+17;
//			colNumDecimal = colNumDigit-5;
//			colNum = colNumDecimal+2+10;
//			if(checkColNum >127)
//			{
//				colNumDigit = 17;
//				colNumDecimal = colNumDigit-5;
//				colNum = colNumDecimal+2+10;
//				page += 2;
//			}
//		}
//		confirmDigitDisplay = true;
//		drawIntegerRightAligned(digitCourier2x7, digit/10, 2, 7, page, colNumDigit);
//		confirmDigitDisplay = false;
//		if(!threeDigit)
//		{
//			drawCharacter('.', arial14, page, colNumDecimal);
//			drawDigit(digitCourier2x7, digit%10, 2, 7, page, colNumDecimal+3);
//		}
//	}
//	showDegree(page, colNum);
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name		:	doScrolling																								//
//	Returned Value		:	None																									//
//																																	//
//																																	//
//	This routine takes 4 menu inputs and determines whether or not they should scroll based on each total col length.				//
// 	This routine also prints all 4 menu inputs and ensures that only the cursor selected menu option and menu title (if necessary)	//
//	are scrolling.																													//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void doScrolling(const unsigned char textZero[], const unsigned char textOne[], const unsigned char textTwo[], const unsigned char textThree[])
{
	int len[4] = {strlen(textOne), strlen(textTwo), strlen(textThree), strlen(textZero)};
	int col[4] = {0,0,0,0};//number of columns the string takes
	const unsigned char *text[4] = {textOne, textTwo, textThree, textZero};
	int tloop = 4;//loop counter
	_Bool scroll[4];//scrolling "flag"
	//determine total col width and scrolling
	for(int k = 0; k < 4; k++)
	{
		for(int i = 0; i <= len[k]; i++)
		{
			col[k] += charWidth(text[k][i], arial14);
		}
		if(col[k] > 121){scroll[k] = TRUE;}
		else			{scroll[k] = FALSE;}
	}

	//If it is not Chinese
	if(scroll[3])
	{
		if(scrollingDelay[3] == tloop)
		{
			scrollingDelay[3] = 0;
			scrollingIndex[3]++;
			if(scrollingIndex[3] == col[3]-120){scrollingIndex[3] = 0;}
		}
		displayTextInOneLineScrolling(textZero, arial14, 0, 10, scrollingIndex[3], 1);
		scrollingDelay[3]++;
	}
	else
	{
		displayTextInOneLineHighlighted(textZero, arial14, 0, 10);
		scrollingIndex[3] = 0;
	}

	switch(currentPosition.lineNumber%3)
	{
		case 1:
			scrollingIndex[1] = 0;
			scrollingIndex[2] = 0;
			if(scroll[0])//only scroll on selected page
			{
				if(scrollingDelay[0] == tloop)
				{
					scrollingDelay[0] = 0;
					scrollingIndex[0]++;
					if(scrollingIndex[0] > col[0]-120){scrollingIndex[0] = 0;}
				}
				displayTextInOneLineScrolling(textOne, arial14, 2, 10, scrollingIndex[0], 0);
				scrollingDelay[0]++;
			}
			else//deal with lines without options.
			{
				if(strcmp(textOne, "\0") != 0){displayTextInOneLine(textOne, arial14, 2, 10);}
				else{clearOneLine(2);}
				scrollingIndex[0] = 0;
			}
			displayTextInOneLine(textTwo, arial14, 4, 10);
			displayTextInOneLine(textThree, arial14, 6, 10);
			break;
		case 2:
			scrollingIndex[0] = 0;
			scrollingIndex[2] = 0;
			if(scroll[1])//only scroll on selected page
			{
				if(scrollingDelay[1] == tloop)
				{
					scrollingDelay[1] = 0;
					scrollingIndex[1]++;
					if(scrollingIndex[1] > col[1]-120){scrollingIndex[1] = 0;} //can make it circular display
				}
				displayTextInOneLineScrolling(textTwo, arial14, 4, 10, scrollingIndex[1], 0);
				scrollingDelay[1]++;
			}
			else//deal with lines without options.
			{
				if(strcmp(textTwo, "\0") != 0){displayTextInOneLine(textTwo, arial14, 4, 10);}
				else{clearOneLine(4);}
				scrollingIndex[1] = 0;
			}
			displayTextInOneLine(textOne, arial14, 2, 10);
			displayTextInOneLine(textThree, arial14, 6, 10);
			break;
		case 0:
			scrollingIndex[0] = 0;
			scrollingIndex[1] = 0;
			if(scroll[2])//only scroll on selected page
			{
				if(scrollingDelay[2] == tloop)
				{
					scrollingDelay[2] = 0;
					scrollingIndex[2]++;
					if(scrollingIndex[2] > col[2]-120){scrollingIndex[2] = 0;}
				}
				displayTextInOneLineScrolling(textThree, arial14, 6, 10, scrollingIndex[2], 0);
				scrollingDelay[2]++;
			}
			else//deal with lines without options.
			{
				if(strcmp(textThree, "\0") != 0){displayTextInOneLine(textThree, arial14, 6, 10);}
				else{clearOneLine(6);}
				scrollingIndex[2] = 0;
			}
			displayTextInOneLine(textOne, arial14, 2, 10);
			displayTextInOneLine(textTwo, arial14, 4, 10);
			break;
		default: break;
	}
	updateDisplayMemory = true;
}

//Combine up to two strings and a formatted value to print with displayTextInMultipleLines
void cat(const unsigned char first[], const unsigned char second[], int type, int value)
{
	char prelim[6], result[100];
	int a, b;

	strcpy(result, first);
	if(!strcmp(second,"\0"))	{strcat(result, second);}
	switch(type)
	{
		case TEMPERATURE:
			a = value/10;
			b = value%10;
			if(value < 0)
			{
				if(value > 999)	{sprintf(prelim, "-%d  ", a);}
				else			{sprintf(prelim, "-%d.%d  ", a, b);}
			}
			else
			{
				if(value > 999)	{sprintf(prelim, "%d  ", a);}
				else			{sprintf(prelim, "%d.%d  ", a, b);}
			}
			break;

		case MIN_SEC:
			a = value/60;
			b = value%60;
			if(b < 10)	{sprintf(prelim, "%dm 0%ds ", a, b);}
			else		{sprintf(prelim, "%dm %ds ", a, b);}
			break;

		case HOUR:
			sprintf(prelim, "%d h ", value);
			break;

		case MIN_SEC_COLON:
			a = value/60;
			b = value%60;
			if(b < 10)	{sprintf(prelim, "%d:0%d ", a, b);}
			else		{sprintf(prelim, "%d:%d ", a, b);}
			break;

		case INTEGER:
			sprintf(prelim, "%d ", value);
			break;

		default: break;
	}
	strcat(result, prelim);
	if(!strcmp(second,"\0") && type == 0){strcat(result, " ");}
	displayTextInMultipleLines(result, arial14, &pgg, &ncol, false);
	if(type == 1){showDegree(pgg, ncol-3);}
}

/******************************************Helper Routine Used in User Interface Closing*********************************************/

/**********************************************Displaying User Interface Opening*****************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	displayChineseUserInterface																					//
//	Returned Value	:	None																										//
//																																	//
//	This routine will display the contents of "User Interface" in Chinese															//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayChineseUserInterface(int lineNumber)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	// Title:
	displayChineseTextInOneLineHighlighted(userInterfaceChinese, 0, 3, 0, 0, 10);
	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F) //heater present
	{
		switch (lineNumber)
		{
			case 1: case 2: case 3:
			// Line 1: Cooling Set Point
			displayChineseTextInOneLine(userInterfaceChinese, 4, 8, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 2: Cooling Differential
			displayChineseTextInOneLine(userInterfaceChinese, 9, 12, 0, 4, MENU_ITEM_START_COLUMN);
			// Line 3: Heating Set Point
			displayChineseTextInOneLine(userInterfaceChinese, 13, 17, 0, 6, MENU_ITEM_START_COLUMN);
			break;

			case 4: case 5: case 6:
			// Line 4: Heating Differential
			displayChineseTextInOneLine(userInterfaceChinese, 18, 21, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 5: High Temp Alarm
			displayChineseTextInOneLine(userInterfaceChinese, 22, 25, 0, 4, MENU_ITEM_START_COLUMN);
			// Line 6: Low Temp Alarm
			displayChineseTextInOneLine(userInterfaceChinese, 26, 29, 0, 6, MENU_ITEM_START_COLUMN);
			break;

			case 7: case 8: case 9:
			// Line 7: Temp Scale
			displayChineseTextInOneLine(userInterfaceChinese, 30, 33, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 8: Hysteresis
			displayChineseTextInOneLine(userInterfaceChinese, 34, 35, 0, 4, MENU_ITEM_START_COLUMN);
			// Line 9: Language
			displayChineseTextInOneLine(userInterfaceChinese, 36, 39, 0, 6, MENU_ITEM_START_COLUMN);
			break;

			case 10:
			// Line 10: Password
			displayChineseTextInOneLine(userInterfaceChinese, 40, 41, 0, 2, MENU_ITEM_START_COLUMN);
			break;

			default: break;
		}
	}
	else //heater not present
	{
		switch (lineNumber)
		{
			case 1: case 2: case 3:
			// Line 1: Cooling Set Point
			displayChineseTextInOneLine(userInterfaceChinese, 4, 8, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 2: Cooling Differential
			displayChineseTextInOneLine(userInterfaceChinese, 9, 12, 0, 4, MENU_ITEM_START_COLUMN);
			// Line 3: High Temp Alarm
			displayChineseTextInOneLine(userInterfaceChinese, 22, 25, 0, 6, MENU_ITEM_START_COLUMN);
			break;

			case 4: case 5: case 6:
			// Line 4: Low Temp Alarm
			displayChineseTextInOneLine(userInterfaceChinese, 26, 29, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 5: Temp Scale
			displayChineseTextInOneLine(userInterfaceChinese, 30, 33, 0, 4, MENU_ITEM_START_COLUMN);
			// Line 6: Hysteresis
			displayChineseTextInOneLine(userInterfaceChinese, 34, 35, 0, 6, MENU_ITEM_START_COLUMN);
			break;

			case 7: case 8:
			// Line 7: Language
			displayChineseTextInOneLine(userInterfaceChinese, 36, 39, 0, 2, MENU_ITEM_START_COLUMN);
			// Line 8: Password
			displayChineseTextInOneLine(userInterfaceChinese, 40, 41, 0, 4, MENU_ITEM_START_COLUMN);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																															    	//
//	Function Name	: 	userInterfaceDisplayMemory																			   	 	//
//	Returned Value	:	None																								    	//
//																															    	//
//	This routine will display the contents of "User Interface" in languages that are not Chinese							    	//
//																															    	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void userInterfaceDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(currentLanguage != CHINESE)
	{
		if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F) //heater present
		{
			switch (currentPosition.lineNumber)
			{
				case 1: case 2: case 3:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_CoolingSetPoint], currentTextTable[TEXT_INDEX_CoolingDifferential], currentTextTable[TEXT_INDEX_HeatingSetPoint]); break;
				case 4: case 5: case 6:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_HeatingDifferential], currentTextTable[TEXT_INDEX_HighTempAlarm], currentTextTable[TEXT_INDEX_LowTempAlarm]); break;
				case 7: case 8: case 9:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_TemperatureScale], currentTextTable[TEXT_INDEX_Hysteresis], currentTextTable[TEXT_INDEX_Language]); break;
				case 10:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_Passcode], "\0", "\0"); break;
				default: break;
			}
		}
		else //heater not present, don't display heating setpoint and heating differential in user interface
		{
			if( notHeaterDisplay && (currentPosition.lineNumber >= 5 && currentPosition.lineNumber <= 10))
			{
				currentPosition.lineNumber -= 2; //have to do this unskip the case 3 when pressed OK button after adjustment in each menu in user interface
				notHeaterDisplay = false;
			}
			switch (currentPosition.lineNumber)
			{
				case 1: case 2: case 3:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_CoolingSetPoint], currentTextTable[TEXT_INDEX_CoolingDifferential], currentTextTable[TEXT_INDEX_HighTempAlarm]); break;
				case 4: case 5: case 6:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_LowTempAlarm], currentTextTable[TEXT_INDEX_TemperatureScale], currentTextTable[TEXT_INDEX_Hysteresis]); break;
				case 7: case 8:
					doScrolling(currentTextTable[TEXT_INDEX_UserInterface], currentTextTable[TEXT_INDEX_Language], currentTextTable[TEXT_INDEX_Passcode], "\0"); break;
				default: break;
			}
		}
	}
	else
	{
		if( notHeaterDisplay && (currentPosition.lineNumber >= 5 && currentPosition.lineNumber <= 10))
		{
			currentPosition.lineNumber -= 2; //have to do this unskip the case 3 when pressed OK button after adjustment in each menu in user interface
			notHeaterDisplay = false;
		}
		displayChineseUserInterface(currentPosition.lineNumber);
	}
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																															    	//
//	Function Name 	: 	coolingSetPointDisplayMemory																		    	//
//	Returned Value	:	None																								    	//
//																															    	//
//	This routine is used to display the text message and digit number to confirm any adjustment that user make using DU.	    	//
//	This routine will call displayTextInOneLine or displayChineseTextInOneLine or displayTextInMultipleLines to do the text	    	//
//	display for English, Chinese, and  other languages that are not Chinese and English respectively. After this is called,     	//
//	English language will call displayDigitForConfirmationMessageEnglish to display the digit, temperature number. As for 	   	 	//
// 	other languages that are not English or Chinese, a call to displayDigitForConfirmationMessage will do the 	    	//
//	display of digit. 																										    	//
//																															    	//
//	This is a confirmation text for cooling setpoint changing 																    	//
//																															    	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void coolingSetPointDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen) //these parameter are set to true in displayPosition.c
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
				cat(
					currentTextTable[TEXT_INDEX_CoolingSetPointIsSetTo],
					"\0",
					TEMPERATURE,
					modbus_rw_reg_rcv[COOLING_SP].ivalue);
				break;

			case CHINESE:
				displayChineseTextInOneLine(userInterfaceChinese, 4, 8, 0, 2, 10);
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 80);
				int num = modbus_rw_reg_rcv[COOLING_SP].ivalue;
				char col = 35;
				if(num)
				drawIntegerRightAligned(digitCourier2x7,num/10, 2, 7, 4, 30);
				if(num < 1000) // less than 100
				{
					drawCharacter('.', arial14, 4, 30);
					drawDigit(digitCourier2x7, num%10, 2, 7, 4, 33);
					col = 45;
				}
				showDegree(4, col);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_CoolingSetPoint], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 4, 8, 0, 0, 10);
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																															    	//
//	Function Name	: 	coolingDifferentialDisplayMemory																	    	//
//	Returned Value	:	None																								    	//
//																															    	//
//	This routine has the same functionality as coolingSetPointDisplayMemory.												    	//
//																															    	//
//	This is confirmation text for Cooling Differential 																		   	 	//
//																														     		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void coolingDifferentialDisplayMemory(void)
{
	//highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
				cat(currentTextTable[TEXT_INDEX_CoolingDifferentialIsSetTo],
					"\0",
					TEMPERATURE,
					modbus_rw_reg_rcv[COOLING_DIF_1].ivalue);
				break;
			case CHINESE:
				displayChineseTextInOneLine(userInterfaceChinese, 9, 12, 0, 2, 10);
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
				drawIntegerRightAligned(digitCourier2x7, modbus_rw_reg_rcv[COOLING_DIF_1].ivalue/10, 2, 7, 4, 25);
				drawCharacter('.', arial14, 4, 25);
				drawDigit(digitCourier2x7, modbus_rw_reg_rcv[COOLING_DIF_1].ivalue%10, 2, 7, 4, 28);
				showDegree(4, 40);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_CoolingDifferential], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 9, 12, 0, 0, 10);
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	heatingSetPointDisplayMemory																				//
//	Returned Value	:	None																										//
//																																	//
//	This routine has the same functionality as coolingSetPointDisplayMemory.														//
//																																	//
//	This is confirmation text for Heating Set Point 																				//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void heatingSetPointDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
				cat(currentTextTable[TEXT_INDEX_HeatingSetPointIsSetTo],
					"\0",
					TEMPERATURE,
					modbus_rw_reg_rcv[HEATING_SP].ivalue);
				break;
			case CHINESE:
				displayChineseTextInOneLine(userInterfaceChinese, 13, 17, 0, 2, 10);
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 80);
				drawIntegerRightAligned(digitCourier2x7, modbus_rw_reg_rcv[HEATING_SP].ivalue/10, 2, 7, 4, 30);
				drawCharacter('.', arial14, 4, 30);
				drawDigit(digitCourier2x7, modbus_rw_reg_rcv[HEATING_SP].ivalue%10, 2, 7, 4, 33);
				showDegree(4, 40);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			char lineNumTemp = 3; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 5;
			}
			doScrolling(currentTextTable[TEXT_INDEX_HeatingSetPoint], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 13, 17, 0, 0, 10);
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	heatingDifferentialDisplayMemory																			//
//	Returned Value	:	None																										//
//																																	//
//	This routine has the same functionality as coolingSetPointDisplayMemory.														//
//																																	//
//	This is confirmation text for Heating Differential  																			//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void heatingDifferentialDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
					cat(currentTextTable[TEXT_INDEX_HeatingDifferentialIsSetTo],
						"\0",
						TEMPERATURE,
						modbus_rw_reg_rcv[HEATING_DIF].ivalue);
					break;
				case CHINESE:
					displayChineseTextInOneLine(userInterfaceChinese, 9, 12, 0, 2, 10);
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
					drawIntegerRightAligned(digitCourier2x7, modbus_rw_reg_rcv[HEATING_DIF].ivalue/10, 2, 7, 4, 25);
					drawCharacter('.', arial14, 4, 25);
					drawDigit(digitCourier2x7, modbus_rw_reg_rcv[HEATING_DIF].ivalue%10, 2, 7, 4, 28);
					showDegree(4, 40);
					break;
				default: break;
			}
	    	TI1_validationScreenTimer_Flag = true;
		}
		else
		{
			highlightOneLine(0);
			if (currentLanguage != CHINESE)
			{
				char lineNumTemp = 4; // heater not present
				if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
				{
					lineNumTemp = 6;
				}
				doScrolling(currentTextTable[TEXT_INDEX_HeatingDifferential], "\0", "\0", "\0");
			}
			else
			{
				displayChineseTextInOneLineHighlighted(userInterfaceChinese, 9, 12, 0, 0, 10);
			}
			showTempReading(userInput, 3, 100);
		}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	highTempAlarmDisplayMemory																					//
//	Returned Value	:	None																										//
//																																	//
//	This routine has the same functionality as coolingSetPointDisplayMemory.														//
//																																	//
//	This is confirmation text for High Temperature Alarm																			//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void highTempAlarmDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
				cat(currentTextTable[TEXT_INDEX_HighTempAlarmIsSetTo],
					"\0",
					TEMPERATURE,
					modbus_rw_reg_rcv[HIGH_TEMP_ALARM_SP].ivalue);
				break;
			case CHINESE:
				displayChineseTextInOneLine(userInterfaceChinese, 22, 25, 0, 2, 10);
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
				drawIntegerRightAligned(digitCourier2x7, modbus_rw_reg_rcv[HIGH_TEMP_ALARM_SP].ivalue/10, 2, 7, 4, 30);
				drawCharacter('.', arial14, 4, 30);
				drawDigit(digitCourier2x7, modbus_rw_reg_rcv[HIGH_TEMP_ALARM_SP].ivalue%10, 2, 7, 4, 33);
				showDegree(4, 40);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			char lineNumTemp = 3; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 5;
			}
			doScrolling(currentTextTable[TEXT_INDEX_HighTempAlarm], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 22, 25, 0, 0, 10);
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	lowTempAlarmDisplayMemory																					//
//	Returned Value	:	None																										//
//																																	//
//	This routine has the same functionality as coolingSetPointDisplayMemory.														//
//																																	//
//	This is confirmation text for Low Temperature Alarm																				//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void lowTempAlarmDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
				cat(currentTextTable[TEXT_INDEX_LowTempAlarmIsSetTo],
					"\0",
					TEMPERATURE,
					modbus_rw_reg_rcv[LOW_TEMP_ALARM_SP].ivalue);
				break;
			case CHINESE:
				displayChineseTextInOneLine(userInterfaceChinese, 26, 29, 0, 2, 10);
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
				drawIntegerRightAligned(digitCourier2x7, modbus_rw_reg_rcv[LOW_TEMP_ALARM_SP].ivalue/10, 2, 7, 4, 30);
				drawCharacter('.', arial14, 4, 30);
				drawDigit(digitCourier2x7, modbus_rw_reg_rcv[LOW_TEMP_ALARM_SP].ivalue%10, 2, 7, 4, 33);
				showDegree(4, 40);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if(currentLanguage != CHINESE)
		{
			char lineNumTemp = 4; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 6;
			}
			doScrolling(currentTextTable[TEXT_INDEX_LowTempAlarm], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 26, 29, 0, 0, 10);
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	tempScaleDisplayMemory																						//
//	Returned Value	:	None																										//
//																																	//
//	This routine will allow the user to adjust between degree F and degree C. 														//
//																																	//
//	This is confirmation text for Temperature Scale																					//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tempScaleDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[UNIT_OF_MEASURE/8] & UNIT_OF_MEASURE_F) // degree F
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
			    	cat(currentTextTable[TEXT_INDEX_TempScaleIsSetTo],
						currentTextTable[TEXT_INDEX_Fahrenheit],
						NOVALUE,
						NOVALUE);
					break;
				case CHINESE:
					displayChineseTextInOneLine(userInterfaceChinese, 30, 33, 0, 2, 10);
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
					displayChineseTextInOneLine(tempScaleChinese, 0, 2, 0, 4, 10);
					break;
				default: break;
			}
		}
		else// degree C
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
					cat(currentTextTable[TEXT_INDEX_TempScaleIsSetTo],
						currentTextTable[TEXT_INDEX_Celsius],
						NOVALUE,
						NOVALUE);
					break;
				case CHINESE:
					displayChineseTextInOneLine(userInterfaceChinese, 30, 33, 0, 2, 10);
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 66);
					displayChineseTextInOneLine(tempScaleChinese, 3, 5, 0, 4, 10);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			char lineNumTemp = 5; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 7;
			}
			doScrolling(currentTextTable[TEXT_INDEX_TemperatureScale], currentTextTable[TEXT_INDEX_Fahrenheit], currentTextTable[TEXT_INDEX_Celsius], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 30, 33, 0, 0, 10);
			displayChineseTextInOneLine(tempScaleChinese, 0, 2, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(tempScaleChinese, 3, 5, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name 	: 	hysteresisDisplayMemory																						//
//	Returned Value	:	None																										//
//																																	//
//	This routine allow the user to change the differential +/- to positive or negative. This will affect the maximum and 			//
// 	minimum of cooling set point as it need to be adjusted to keep the CoolOn and CoolOff constant. 								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hysteresisDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[COOL_HYSTERESIS/8] & COOL_HYSTERESIS_F) // Negative
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
					cat(currentTextTable[TEXT_INDEX_DifferentialIsSetTo],
						currentTextTable[TEXT_INDEX_Negative],
						NOVALUE,
						NOVALUE);
					break;
				case CHINESE:
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 3, 5);
					displayChineseTextInOneLine(hysteresisChinese, 4, 7, 0, 3, 47);
					break;
				default: break;
			}
		}
		else
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
					cat(currentTextTable[TEXT_INDEX_DifferentialIsSetTo],
						currentTextTable[TEXT_INDEX_Positive],
						NOVALUE,
						NOVALUE);
					break;
				case CHINESE:
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 3, 5);
					displayChineseTextInOneLine(hysteresisChinese, 0, 3, 0, 3, 47);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			char lineNumTemp = 6; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 8;
			}
			doScrolling(currentTextTable[TEXT_INDEX_Hysteresis], currentTextTable[TEXT_INDEX_Positive], currentTextTable[TEXT_INDEX_Negative], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 34, 35, 0, 0, 10);
			displayChineseTextInOneLine(hysteresisChinese, 0, 3, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(hysteresisChinese, 4, 7, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name 	:	languageDisplayMemory																						//
//	Returned Value	:	None																										//
//																																	//
//	This routine will display language selection. In this screen page, user can select the language that they want to have 			//
//	the DU to be in. Once a language is selected, a confirmation in that selected language will be display. That message in 		//
//	English is "Updating Language Setting ... "																						//
//																																	//
//	This routine will call displayTextInMultipleLines routine to do the display of the text. This routine is called and not 		//
//	displayTextInOneLine routine because the confirmation text message can justt pass long text message instead of having   		//
//	to pinpoint the correct column to start a text to make use of the available column when calling displayTextInOneLine. 			//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void languageDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
		    	displayTextInMultipleLines(currentTextTable[TEXT_INDEX_UpdatingLanguageSettings], arial14, &pgg, &ncol, false);
				break;
			case CHINESE:
				displayChineseTextInOneLine(languageConformationChinese, 0, 6, 0, 3, 10);
				break;
			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			char lineNumTemp = 7; // heater not present
			if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
			{
				lineNumTemp = 9;
			}
			displayTextInOneLineHighlighted(currentTextTable[TEXT_INDEX_Language], arial14, 0, MENU_ITEM_START_COLUMN);
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 36, 39, 0, 0, 10);
		}
		// For language menu, the menu items are same for all languages
	    switch (currentPosition.lineNumber)
	    {
	        case 1: case 2: case 3:
	    		displayTextInOneLine(currentTextTable[TEXT_INDEX_English], arial14, 2, MENU_ITEM_START_COLUMN);
				displayChineseTextInOneLine(languageMenuChinese, 0, 1, 0, 4, MENU_ITEM_START_COLUMN);
	    		displayTextInOneLine(currentTextTable[TEXT_INDEX_French], arial14, 6, MENU_ITEM_START_COLUMN);
	            break;
	        case 4: case 5: case 6:
	        	displayTextInOneLine(currentTextTable[TEXT_INDEX_Deutsche], arial14, 2, MENU_ITEM_START_COLUMN);
	        	displayTextInOneLine(currentTextTable[TEXT_INDEX_Spanish], arial14, 4, MENU_ITEM_START_COLUMN);
	        	displayTextInOneLine(currentTextTable[TEXT_INDEX_italiano], arial14, 6, MENU_ITEM_START_COLUMN);
	            break;
	        /*case 7:
	    		displayTextInOneLine(currentTextTable[TEXT_INDEX_Polskie], arial14, 2, MENU_ITEM_START_COLUMN);
	    		break;*/
	        default: break;
	    }
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	passwordDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display a text confirmation when user choose to have passcode for "User Interface". For Chinese  				//
//	language, this routine will call displayChineseTextInOneLine. The other languages will call displayTextInMultipleLines  		//
//	to do the display. 																												//
//																																	//
//	This confirmation message will display when adjusted fron having passcode and not having passcode.								//
//																																	//
//	This is a "User Interface passcode ON/OFf" confirmation message																	//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void passwordDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[PASSWORD_ENABLED/8] & PASSWORD_ENABLED_F) // ON
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_PasscodeForUserInterfaceIsOn], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(userInterfaceChinese, 40, 41, 0, 3, 10); //change 15 to 10 - Youa
					displayChineseTextInOneLine(passcodeChinese, 2, 3, 0, 3, 43);
					break;
				default: break;
			}
		}
		else // OFF
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:case POLISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_PasscodeForUserInterfaceIsOff], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(userInterfaceChinese, 40, 41, 0, 3, 10);
					displayChineseTextInOneLine(passcodeChinese, 0, 1, 0, 3, 43);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_Passcode], currentTextTable[TEXT_INDEX_OFF], currentTextTable[TEXT_INDEX_ON], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(userInterfaceChinese, 40, 41, 0, 0, 10);
			displayChineseTextInOneLine(passcodeChinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(passcodeChinese, 2, 3, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

/**********************************************Displaying User Interface Closing*****************************************************/

/************************************************Displaying System Interface Opening*************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	displayChineseSystemInterface																				//
//	Returned Value	:	None																										//
//	Passing Parameter	:	lineNumber																								//
//			lineNumber 	:	A number used in a switch statement to display three lines contents 									//
//																																	//
//	This routines display the System Interface Menu in Chinese																		//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayChineseSystemInterface(int lineNumber)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	// Title:
	displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 0, 3, 0, 0, 10);

	switch (lineNumber)
	{
		case 1: case 2: case 3:
		// Line 1: Door Switch
		displayChineseTextInOneLine(systemInterfaceChinese, 4, 7, 0, 2, MENU_ITEM_START_COLUMN);
		// Line 2: Disable Switch
		displayChineseTextInOneLine(systemInterfaceChinese, 8, 11, 0, 4, MENU_ITEM_START_COLUMN);
		// Line 3: Alarm relay output
		displayChineseTextInOneLine(systemInterfaceChinese, 12, 16, 0, 6, MENU_ITEM_START_COLUMN);
		break;

		case 4: case 5: case 6:
		// Line 4: Compressor Restart Delay
		displayChineseTextInOneLine(systemInterfaceChinese, 17, 23, 0, 2, MENU_ITEM_START_COLUMN);
		// Line 5: Compressor Current
		displayChineseTextInOneLine(systemInterfaceChinese, 24, 28, 0, 4, MENU_ITEM_START_COLUMN);
		// Line 6: System Settings
		displayChineseTextInOneLine(systemInterfaceChinese, 29, 32, 0, 6, MENU_ITEM_START_COLUMN);
		break;

		default: break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	systemInterfaceDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This routine has the same functionality as the one used to display System Interface Menu in Chinese. It only differ in			//
// 	the sense that this content is displayed in other languges that is not Chinese. 												//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void systemInterfaceDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		switch (currentPosition.lineNumber)
		{
			case 1: case 2: case 3:
				doScrolling(currentTextTable[TEXT_INDEX_SystemInterface], currentTextTable[TEXT_INDEX_GroupControlMode], currentTextTable[TEXT_INDEX_DoorSmokeAlarmSetting], currentTextTable[TEXT_INDEX_DisableSwitch]); break;
			case 4: case 5: case 6:
				doScrolling(currentTextTable[TEXT_INDEX_SystemInterface], currentTextTable[TEXT_INDEX_AlarmRelayOutput], currentTextTable[TEXT_INDEX_CompRestartDelay], currentTextTable[TEXT_INDEX_CompressorCurrent]); break;
			case 7: case 8:
				doScrolling(currentTextTable[TEXT_INDEX_SystemInterface], currentTextTable[TEXT_INDEX_EvapOutTempSensor], currentTextTable[TEXT_INDEX_SystemSettings], "\0"); break;
			default: break;
		}
	}
	else
	{
		displayChineseSystemInterface(currentPosition.lineNumber);
	}
	showSelectArrow();
	//SysIntArrowPosition = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	:	GroupControlModeDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This menu shall provide means to program the following:Mode and Mode Settings													//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void groupControlModeDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_GroupControlMode], currentTextTable[TEXT_INDEX_Mode], currentTextTable[TEXT_INDEX_ModeSettings], "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(groupControlModeChinese, 0, 1, 0, 0, MENU_ITEM_START_COLUMN);	//group control mode
		displayChineseTextInOneLine(groupControlModeChinese, 2, 3, 0, 2, MENU_ITEM_START_COLUMN);				//mode
		displayChineseTextInOneLine(groupControlModeChinese, 4, 5, 0, 4, MENU_ITEM_START_COLUMN);				//mode settings
	}
	SysIntArrowPosition = true;
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	modeDisplayMemory																			//
//	Returned Value 					: 	None																						//
//																																	//
//	This menu show display the mode settings, and the current mode setting is selected. User can also toggle to change mode settings//
//	between stand-alone, lead/lag and master/slave. The default mode is stand-alone mode											//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void modeDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(parameterIsSet && showValidationScreen)
	{
		if(currentLanguage != CHINESE)
		{
			switch(currentPosition.lineNumber)
			{
				case INLET_LINENUM:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_GCMIsSetToSA], arial14, &pgg, &ncol, false);
					break;
				case OUTLET_LINENUM:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_GCMIsSetToLL], arial14, &pgg, &ncol, false);
					break;
				case COIL_LINENUM:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_GCMIsSetToMS], arial14, &pgg, &ncol, false);
					break;
				default: break;
			}
			//add German case
		}
		else
		{
			switch(currentPosition.lineNumber)
			{
				case INLET_LINENUM:
					displayChineseTextInOneLine(groupControlModeChinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);	//group control mode
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
					displayChineseTextInOneLine(groupControlModeChinese, 6, 7, 0, 6, MENU_ITEM_START_COLUMN);	//stand-alone
					break;
				case OUTLET_LINENUM:
					displayChineseTextInOneLine(groupControlModeChinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);	//group control mode
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
					displayChineseTextInOneLine(groupControlModeChinese, 8, 9, 0, 6, MENU_ITEM_START_COLUMN);	//lead/lag
					break;
				case COIL_LINENUM:
					displayChineseTextInOneLine(groupControlModeChinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);	//group control mode
					displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
					displayChineseTextInOneLine(groupControlModeChinese, 10, 11, 0, 6, MENU_ITEM_START_COLUMN);	// master/slave
					break;
				default: break;
			}
		}
		TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		showSelectArrow();
		if(currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_Mode], currentTextTable[TEXT_INDEX_StandAlone], currentTextTable[TEXT_INDEX_LeadLag], currentTextTable[TEXT_INDEX_MasterSlave]);
		}
		else
		{
			displayChineseTextInOneLineHighlighted(groupControlModeChinese, 2, 3, 0, 0, MENU_ITEM_START_COLUMN);	//mode
			displayChineseTextInOneLine(groupControlModeChinese, 6, 7, 0, 2, MENU_ITEM_START_COLUMN);	//stand-alone
			displayChineseTextInOneLine(groupControlModeChinese, 8, 9, 0, 4, MENU_ITEM_START_COLUMN);	//lead/lag
			displayChineseTextInOneLine(groupControlModeChinese, 10, 11, 0, 6, MENU_ITEM_START_COLUMN);	//master/slave
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	standAloneDisplayMemory																		//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu shall be displayed when the device is in stand-alone mode															//
//		and there will be no settings in this mode																					//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void standAloneModeDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_StandAloneMode], currentTextTable[TEXT_INDEX_NoSettings], "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(groupControlModeChinese, 12, 13, 0, 0, 10);	//stand alone mode
		displayChineseTextInOneLine(groupControlModeChinese, 14, 15, 0, 2, MENU_ITEM_START_COLUMN);	//no settings
	}
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	leadLagModeDisplayMemory																	//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu shall be displayed when the device is in lead-lag mode															//
//		and user can choose to select from unit identification, differential two and control strategy								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void leadLagModeDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_LeadLagMode], currentTextTable[TEXT_INDEX_UnitIdentification], currentTextTable[TEXT_INDEX_DifferentialTwo], currentTextTable[TEXT_INDEX_ControlStrategy]);
	}
	else
	{
		displayChineseTextInOneLineHighlighted(groupControlModeChinese, 16, 17, 0, 0, 10);	//lead/lag mode
		displayChineseTextInOneLine(groupControlModeChinese, 18, 19, 0, 2, 10);	//unit identification
		displayChineseTextInOneLine(groupControlModeChinese, 20, 21, 0, 4, 10);	//differential two
		displayChineseTextInOneLine(groupControlModeChinese, 22, 23, 0, 6, 10);	//control strategy
	}
	showSelectArrow();
	SysIntArrowPosition = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	masterSlaveModeDisplayMemory																//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu shall be displayed when the device is in master-slave mode														//
//		and user can select from number of units and unit identification															//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void masterSlaveModeDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_MasterSlave], currentTextTable[TEXT_INDEX_UnitIdentification], currentTextTable[TEXT_INDEX_NumberOfUnits], "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(groupControlModeChinese, 24, 25, 0, 0, 10);	//master/slave mode
		displayChineseTextInOneLine(groupControlModeChinese, 18, 19, 0, 2, 10);	//unit identification
		displayChineseTextInOneLine(groupControlModeChinese, 26, 27, 0, 4, 10);	//number of units
	}
	SysIntArrowPosition = true;
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	unitIdentificationDisplayMemory																//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu will display the unit identification number																		//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void unitIdentificationDisplayMemory(void)
{
	//highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(parameterIsSet && showValidationScreen)
	{
		if (currentLanguage != CHINESE)
		{
			cat(currentTextTable[TEXT_INDEX_UnitIdentification],
				currentTextTable[TEXT_INDEX_IsSetTo],
				INTEGER,
				modbus_rw_reg_rcv[UNIT_ID].ivalue);
//			displayTextInOneLine(, arial14, 2, MENU_ITEM_START_COLUMN);
//			displayTextInOneLine(, arial14, pgg, ncol);
//			drawIntegerLeftAligned(digitCourier2x7, userInput, 2, 7, 3, 10);
		}
		else
		{
			displayChineseTextInOneLine(groupControlModeChinese, 18, 19, 0, 2, 10);	//unit identification
			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
			drawIntegerRightAligned(digitCourier2x7, userInput, 2, 7, 4, 60);
		}
		TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if(currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_UnitIdentification], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(groupControlModeChinese, 18, 19, 0, 0, 10);	//unit identification
		}
		drawIntegerLeftAligned(digitCalibriLight4x28, userInput, 4, 28, 3, 36);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	differential2DisplayMemory																	//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu will set the differential 2																						//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void differential2DisplayMemory(void)
{
	//highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(parameterIsSet && showValidationScreen) //these parameter are set to true in displayPosition.c
	{
		if(currentLanguage != CHINESE)
		{
			cat(currentTextTable[TEXT_INDEX_DifferentialTwo],
				currentTextTable[TEXT_INDEX_IsSetTo],
				TEMPERATURE,
				modbus_rw_reg_rcv[COOLING_DIF_2].ivalue);
		}
		else
		{
			displayChineseTextInOneLine(groupControlModeChinese, 20, 21, 0, 2, 10); 	//differential two
			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
//			displayChineseTextInOneLine(userInterfaceChinese, 4, 8, 0, 2, 10);
//			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 80);
//			int num = modbus_rw_reg_rcv[COOLING_SP].ivalue;
//			char col = 35;
//			if(num)
//			drawIntegerRightAligned(digitCourier2x7,num/10, 2, 7, 4, 30);
//			if(num < 1000) // less than 100
//			{
//				drawCharacter('.', arial14, 4, 30);
//				drawDigit(digitCourier2x7, num%10, 2, 7, 4, 33);
//				col = 45;
//			}
//			showDegree(4, col);
//			break;
			int digit = modbus_rw_reg_rcv[COOLING_DIF_2].ivalue;
			//displayDigitForConfirmationMessageOtherLanguage( 5, 4, digit);
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_DifferentialTwo], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(groupControlModeChinese, 20, 21, 0, 0, 10);	//differential two
		}
		showTempReading(userInput, 3, 100);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	controlStrategyDisplayMemory																//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu will set the control strategy hours																				//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void controlStrategyDisplayMemory(void)
{
	//highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen) //these parameter are set to true in displayPosition.c
	{
		if(currentLanguage != CHINESE)
		{
			cat(currentTextTable[TEXT_INDEX_ControlStrategy],
				currentTextTable[TEXT_INDEX_IsSetTo],
				HOUR,
				modbus_rw_reg_rcv[CONTROL_STRATEGY].ivalue);
//			displayTextInOneLine(currentTextTable[TEXT_INDEX_ControlStrategy], arial14, 1, 10);
//			displayTextInOneLine(currentTextTable[TEXT_INDEX_IsSetTo], arial14, 3, 10);
//			int digit = modbus_rw_reg_rcv[CONTROL_STRATEGY].ivalue;
//			drawDigit(digitCourier2x7, digit/10, 2, 7, 5, 10);
//			drawDigit(digitCourier2x7, digit%10, 2, 7, 5, 17);
//			displayTextInOneLine("h", arial14, 5, 24);
		}
		else
		{
			displayChineseTextInOneLine(groupControlModeChinese, 22, 23, 0, 2, 10);	//control strategy
			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
			int digit = modbus_rw_reg_rcv[CONTROL_STRATEGY].ivalue;
			drawDigit(digitCourier2x7, digit/10, 2, 7, 4, 60);
			drawDigit(digitCourier2x7, digit%10, 2, 7, 4, 67);
			displayChineseTextInOneLine(groupControlModeChinese, 28, 29, 0, 4, 10);	//hours
//			displayChineseTextInOneLine(userInterfaceChinese, 4, 8, 0, 2, 10);
//			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 2, 80);
//			int num = modbus_rw_reg_rcv[COOLING_SP].ivalue;
//			char col = 35;
//			if(num)
//			drawIntegerRightAligned(digitCourier2x7,num/10, 2, 7, 4, 30);
//			if(num < 1000) // less than 100
//			{
//				drawCharacter('.', arial14, 4, 30);
//				drawDigit(digitCourier2x7, num%10, 2, 7, 4, 33);
//				col = 45;
//			}
//			showDegree(4, col);
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_ControlStrategy], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(groupControlModeChinese, 22, 23, 0, 0, 10);	//control strategy
		}
		drawIntegerLeftAligned(digitCalibriLight4x28, userInput, 4, 28, 3, 36);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	numberOfUnitsDisplayMemory																	//
//	Returned Value 					: 	None																						//
// 	Variable and other routine call	:																								//
//																																	//
//		This menu will display the number of units in the system																	//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void numberOfUnitsDisplayMemory(void)
{
	//highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if(parameterIsSet && showValidationScreen)
	{
		if(currentLanguage != CHINESE)
		{
			cat(currentTextTable[TEXT_INDEX_NumberOfUnits],
				currentTextTable[TEXT_INDEX_IsSetTo],
				INTEGER,
				modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue);
//			displayTextInOneLine(currentTextTable[TEXT_INDEX_NumberOfUnits], arial14, 1, 10);
//			displayTextInOneLine(currentTextTable[TEXT_INDEX_IsSetTo], arial14, 3, 10);
//			int digit = modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue;
//			drawDigit(digitCourier2x7, digit/10, 2, 7, 5, 10);
//			drawDigit(digitCourier2x7, digit%10, 2, 7, 5, 17);
		}
		else
		{
			displayChineseTextInOneLine(groupControlModeChinese, 26, 27, 0, 2, 10);	//number of units
			displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, MENU_ITEM_START_COLUMN);	//is set to
			int digit = modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue;
			drawDigit(digitCourier2x7, digit/10, 2, 7, 4, 60);
			drawDigit(digitCourier2x7, digit%10, 2, 7, 4, 67);
		}
		TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_NumberOfUnits], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(groupControlModeChinese, 26, 27, 0, 0, 10);	//number of units
		}
		drawIntegerLeftAligned(digitCalibriLight4x28, userInput, 4, 28, 3, 36);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	doorSwitchDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routines will do the text comfirmation when toggle between door Switch normally open to normall closed and vice			//
// 	versa. It call displayTextInMultipleLines routine to do the display in all the language that is not Chineseã€If the				//
//	current language is	Chinese, then it will call displayChineseTextInOneLine twice to do Chinese display. 						//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void doorSwitchDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[DOOR_SWITCH/8] & DOOR_SWITCH_F) // Normally Close
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_DoorSwitchIsSetToNC], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 4, 5, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 3, 38);
					break;
				default: break;
			}
		}
		else // Normally Open
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_DoorSwitchIsSetToNO], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 4, 5, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 3, 38);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if(currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_DoorSmokeAlarmSetting], currentTextTable[TEXT_INDEX_NormallyOpen], currentTextTable[TEXT_INDEX_NormallyClose], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 4, 7, 0, 0, 10);
			displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	disableSwitchAlarmDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This routines will do the text comfirmation when toggle between disable Switch normally open to normall closed and vice			//
// 	versa. It call displayTextInMultipleLines routine to do the display in all the language that is not Chineseã€If the				//
//	current language is	Chinese, then it will call displayChineseTextInOneLine twice to do Chinese display.							//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void disableSwitchAlarmDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[DISABLE_SWITCH/8] & DISABLE_SWITCH_F) // Normally Close
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case ITALIAN: case SPANISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_DisableSwitchIsSetToNC], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 8, 9, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 3, 38);
					break;
				default: break;
			}
		}
		else // Normally Open
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case ITALIAN: case SPANISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_DisableSwitchIsSetToNO], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 8, 9, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 3, 38);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_DisableSwitch], currentTextTable[TEXT_INDEX_NormallyOpen], currentTextTable[TEXT_INDEX_NormallyClose], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 8, 11, 0, 0, 10);
			displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	disableSwitchAlarmDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This routines will do the text comfirmation when toggle between disable Switch normally open to normall closed and vice			//
// 	versa. It call displayTextInMultipleLines routine to do the display in all the language that is not Chineseã€If the				//
//	current language is	Chinese, then it will call displayChineseTextInOneLine twice to do Chinese display.							//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alarmRelayOutputDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		if (modbus_rw_coil_rcv[ALARM_OUTPUT/8] & ALARM_OUTPUT_F) // Normally Closed
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case ITALIAN: case SPANISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_AlarmRelayOutputIsSetToNC], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 12, 16, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 3, 80);
					break;
				default: break;
			}
		}
		else // Normally Open
		{
			switch(currentLanguage)
			{
				case ENGLISH: case FRENCH: case GERMAN: case ITALIAN: case SPANISH:
					displayTextInMultipleLines(currentTextTable[TEXT_INDEX_AlarmRelayOutputIsSetToNO], arial14, &pgg, &ncol, false);
					break;
				case CHINESE:
					displayChineseTextInOneLine(systemInterfaceChinese, 12, 16, 0, 3, 10);
					displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 3, 80);
					break;
				default: break;
			}
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_AlarmRelayOutput], currentTextTable[TEXT_INDEX_NormallyOpen], currentTextTable[TEXT_INDEX_NormallyClose], "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 12, 16, 0, 0, 10);
			displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);
			displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 4, MENU_ITEM_START_COLUMN);
		}
		showSelectArrow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	compressorRestartDelayDisplayMemory																			//
//	Returned Value	: 	None																										//
//																																	//
//	This routine display Compressor Restart Delay, time wait before restart compressor. It will call displayTextInOneLine to do the	//
//	text display. For the digit, it call drawIntegerLeftAligned. The page and column used are figured by guess and check. There is 	//
//	may be or may not be a systematic way of doing page and column calculating. Right now, it is just guess and check with the 		//
//	display in DU. 																													//
//																																	//
//	The time will be given by the Controller board. it can be accessed through an array of registers called modbus_rw_reg_rcv.		//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compressorRestartDelayDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
	{
		switch(currentLanguage)
		{
			case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:

				cat(currentTextTable[TEXT_INDEX_CompRestartDelayIsSetTo],
					"\0",
					MIN_SEC,
					modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue);
				break;

			case CHINESE:
				displayChineseTextInOneLine(systemInterfaceChinese, 17, 23, 0, 2, 10);
				if (modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60 == 0) // minute = 0
				{
			    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, 4, 52);
		    		displayChineseTextInOneLine(timeTextChinese, 2, 2, 0, 4, 66);
				}
				else
				{
			    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60, 2, 7, 4, 52);
			    	if (modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60 != 0) // second != 0
			    	{
				    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, 4, 80);
						// When displaying a number, it clears space for sign and value. Here numbers are all positive. \
				    	   Thus, put the function for displaying currentTextTable after number to avoid the last char in the currentTextTable to be cleared.
			    		displayChineseTextInOneLine(timeTextChinese, 0, 0, 0, 4, 66);
			    		displayChineseTextInOneLine(timeTextChinese, 2, 2, 0, 4, 94);
			    	}
			    	else
			    	{
			    		displayChineseTextInOneLine(timeTextChinese, 0, 1, 0, 4, 60);
			    	}
				}
				// When displaying a number, it clears space for sign and value. Here numbers are all positive. \
		    	   Thus, put the function for displaying currentTextTable after number to avoid the last char in the currentTextTable to be cleared.
				displayChineseTextInOneLine(confirmationTextChinese, 0, 2, 0, 4, 10);
				break;
//
//
//				//DÃ©lai de redÃ©marrage du compresseur est rÃ©glÃ© sur 3 min 30 sec
//				displayTextInMultipleLines(frenchText[TEXT_INDEX_CompRestartDelayIsSetTo], arial14, &pgg, &ncol, false);
//		    	//displayTextInOneLine("Comp Restart Delay", arial14, 2, 5);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60, 2, 7, 6, 5);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, 6, 35);
//		    	displayTextInOneLine("min", arial14, 6, 15);
//		    	displayTextInOneLine("sec", arial14, 6, 55);
//				break;
//
//
//				displayTextInOneLine(germanText[TEXT_INDEX_CompRestartDelay], arial14, 0, 3);
//				displayTextInOneLine("Neustart des Komp", arial14, 2, 3);
//				displayTextInOneLine("ist eingestellt auf", arial14, 4, 3);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60, 2, 7, 6, 3);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, 6, 35);
//		    	displayTextInOneLine("min", arial14, 6, 15);
//		    	displayTextInOneLine("sec", arial14, 6, 55);
//				break;
//
//
//		    	displayTextInMultipleLines(spanishText[TEXT_INDEX_CompRestartDelayIsSetTo], arial14, &pgg, &ncol, false);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60, 2, 7, pgg, ncol+3);
//		    	drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, pgg, ncol+6+7+3+16);
//		    	displayTextInOneLine("min", arial14, pgg, ncol+6+7);
//		    	displayTextInOneLine("sec", arial14, pgg, ncol+6+7+3+16+7+7+3);
//				break;
//
//
//				displayTextInMultipleLines(italianText[TEXT_INDEX_CompRestartDelayIsSetTo], arial14, &pgg, &ncol, false);
//				drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue/60, 2, 7, pgg, ncol+3);
//				drawIntegerLeftAligned(digitCourier2x7, modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue%60, 2, 7, pgg, ncol+6+7+3+16);
//				displayTextInOneLine("min", arial14, pgg, ncol+6+7);
//				displayTextInOneLine("sec", arial14, pgg, ncol+6+7+3+16+7+7+3);

			default: break;
		}
    	TI1_validationScreenTimer_Flag = true;
	}
	else
	{
		highlightOneLine(0);
		if (currentLanguage != CHINESE)
		{
			doScrolling(currentTextTable[TEXT_INDEX_CompRestartDelay], "\0", "\0", "\0");
		}
		else
		{
			displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 17, 23, 0, 0, 10);
		}
		showTimer(digitCalibriLight4x28, userInput/60, userInput%60, 4, 28, 3, 5);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	compressorCurrentDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This routine display the current for each phase. There are three phases and these current can be reading through 				//
//	modbus_ro_reg_rcv array. 																									 	//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compressorCurrentDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_CompressorCurrent], currentTextTable[TEXT_INDEX_Phase1], currentTextTable[TEXT_INDEX_Phase2], currentTextTable[TEXT_INDEX_Phase3]);
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 24, 28, 0, 0, 10);
		displayTextInOneLine("1:", arial14, 2, 35);
		displayTextInOneLine("2:", arial14, 4, 35);
		displayTextInOneLine("3:", arial14, 6, 35);

		displayChineseTextInOneLine(phase_Chinese, 0, 1, 0, 2, 5);
		displayChineseTextInOneLine(phase_Chinese, 0, 1, 0, 4, 5);
		displayChineseTextInOneLine(phase_Chinese, 0, 1, 0, 6, 5);
	}
	int x[4] = {0,
				modbus_ro_reg_rcv[PHASE_1_CURRENT].ivalue,
				modbus_ro_reg_rcv[PHASE_2_CURRENT].ivalue,
				modbus_ro_reg_rcv[PHASE_3_CURRENT].ivalue};
	int a,b,c,d,e;
	char result[10];
	for(int i=1; i<4; i++)
	{
		a = x[i]/10000;
		b = x[i]%10000/1000;
		c = x[i]%10000%1000/100;
		d = x[i]%10000%1000%100/10;
		e = x[i]%10000%1000%100%10;
		//current value
		if(x[i] <= 999)
		{
			sprintf(result, "%d%d%d", c,d,e);
		}
		else if(x[i] > 999 && x[i] <= 9999)
		{
			sprintf(result, "%d.%d%d", b,c,d);
		}
		else
		{
			sprintf(result, "%d%d.%d", a,b,c);
		}
		displayTextInOneLine(result, arial14, i*2, 70);
		//unit of measure
		if(x[i] <= 999)
		{
			drawCharacter('m', arial14, i*2, 105);
			drawCharacter('A', arial14, i*2, 115);
		}
		else
		{
			drawCharacter('A', arial14, i*2, 110);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	evapOutTempSensorDisplayMemory																				//
//	Returned Value	: 	None																										//
//																																	//
//	This menu selection shall allow the user to set whether or not the Evap Out Temp Sensor is present.  							//
//Present and Not Present shall be listed as selectable items on this menu.  The current active setting shall be highlighted.		//																							 	//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void evapOutTempSensorDisplayMemory(void)
{
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (parameterIsSet && showValidationScreen)
		{
			if (modbus_rw_coil_rcv[PROBE2_PRESENT/8] & PROBE2_PRESENT_F) // Probe 2 present
			{
				switch(currentLanguage)
				{
					case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
						displayTextInMultipleLines(currentTextTable[TEXT_INDEX_EvapOutTempSensorIsSetToPresent], arial14, &pgg, &ncol, false);
						break;
					case CHINESE:
//						displayChineseTextInOneLine(systemInterfaceChinese, 4, 5, 0, 3, 10);
//						displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 3, 38);
						break;
					default: break;
				}
			}
			else // Probe 2 absent
			{
				switch(currentLanguage)
				{
					case ENGLISH: case FRENCH: case GERMAN: case SPANISH: case ITALIAN:
						displayTextInMultipleLines(currentTextTable[TEXT_INDEX_EvapOutTempSensorIsSetToAbsent], arial14, &pgg, &ncol, false);
						break;
					case CHINESE:
//						displayChineseTextInOneLine(systemInterfaceChinese, 4, 5, 0, 3, 10);
//						displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 3, 38);
						break;
					default: break;
				}
			}
	    	TI1_validationScreenTimer_Flag = true;
		}
		else
		{
			highlightOneLine(0);
			if (currentLanguage != CHINESE)
			{
				doScrolling(currentTextTable[TEXT_INDEX_EvapOutTempSensor], currentTextTable[TEXT_INDEX_Present], currentTextTable[TEXT_INDEX_NotPresent], "\0");
			}
			else
			{
//				displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 4, 7, 0, 0, 10);
//				displayChineseTextInOneLine(NO_NC_Chinese, 0, 1, 0, 2, MENU_ITEM_START_COLUMN);
//				displayChineseTextInOneLine(NO_NC_Chinese, 2, 3, 0, 4, MENU_ITEM_START_COLUMN);
			}
			showSelectArrow();
		}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	systemSettingsDisplayMemory																					//
//	Returned Value	: 	None																										//
//																																	//
//	This routine allows the user to see the setting for System Interface. It will contain,											//
//		Current Limit Setting																										//
//		MALF ACtivate																												//
//		MALF Deactivate																												//
//		HPC Fan On																													//
//		HPC Fan Off 																												//
//																																	//
//	All of these are not unchangeable. That is, that are set in the production line and the user cannot change them. 				//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void systemSettingsDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		SysSettingArrowPosition = false;
		if(currentPosition.lineNumber == 1 || currentPosition.lineNumber == 2 || currentPosition.lineNumber == 3)
		{
			SysSettingArrowPosition = true;
		}
		switch (currentPosition.lineNumber)
		{
			case 1: case 2: case 3:
				doScrolling(currentTextTable[TEXT_INDEX_SystemSettings], currentTextTable[TEXT_INDEX_CurrentLimitSetting], currentTextTable[TEXT_INDEX_MALFActivate], currentTextTable[TEXT_INDEX_MALFDeactivate]);	break;
			case 4: case 5:
				doScrolling(currentTextTable[TEXT_INDEX_SystemSettings], currentTextTable[TEXT_INDEX_HPCFanOn], currentTextTable[TEXT_INDEX_HPCFanOff], "\0");	break;
			default: break;
		}
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemInterfaceChinese, 29, 32, 0, 0, 10);
		switch (currentPosition.lineNumber)
		{
			case 1: case 2: case 3:
			// Line 1: Current Limit Setting
			displayChineseTextInOneLine(systemSettingsChinese, 0, 3, 0, 2, 10);
			// Line 2: MALF Activate
			displayChineseTextInOneLine(systemSettingsChinese, 4, 11, 0, 4, 10);
			// Line 3: MALF Deactivate
			displayChineseTextInOneLine(systemSettingsChinese, 12, 19, 0, 6, 10);
			break;

			case 4: case 5:
			// Line 4: HPC Fan ON
			displayChineseTextInOneLine(systemSettingsChinese, 20, 27, 0, 2, 10);
			// Line 5: HPC Fan OFF
			displayChineseTextInOneLine(systemSettingsChinese, 28, 35, 0, 4, 10);
			break;

			default: break;
		}
	}
	showSelectArrow();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	currentLimitSettingsDisplayMemory																			//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the current limiting of the compressor. It will read the current setpoint, which is set by production //
// 	line, in modbus_ero_reg_rcv array. Once it is read, it will display the current in DU. This current will not be adjustable. 	//
//	So, pressing up and down button will not do anything. Press the OK or back button will go back to the previous screen. 			//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void currentLimitSettingsDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_CurrentLimitSetting], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemSettingsChinese, 0, 3, 0, 0, 10);
	}
	int currentLimit = modbus_ero_reg_rcv[CURRENT_SETPOINT_INDEX].ivalue;
	if(currentLimit <= 999)
	{
		drawIntegerRightAligned(digitCalibri4x15, currentLimit, 4, 15, 3, 80);
		displayTextInOneLine("mA", arial14, 4, 85);
	}
	else if(currentLimit > 999 && currentLimit <= 9999)
	{
		drawDigit(digitCalibri4x15, currentLimit/1000, 4, 15, 3, 30);
		drawDigit(digitCalibri4x15, DECIMAL_POINT_INDEX, 4, 15, 3, 45);
		drawDigit(digitCalibri4x15, currentLimit%1000/100, 4, 15, 3, 55);
		drawDigit(digitCalibri4x15, currentLimit%1000%100/10, 4, 15, 3, 70);
		displayTextInOneLine("A", arial14, 4, 90);
	}
	else
	{
		drawDigit(digitCalibri4x15, currentLimit/10000, 4, 15, 3, 30);
		drawDigit(digitCalibri4x15, currentLimit%10000/1000, 4, 15, 3, 45);
		drawDigit(digitCalibri4x15, DECIMAL_POINT_INDEX, 4, 15, 3, 60);
		drawDigit(digitCalibri4x15, currentLimit%10000%1000/100, 4, 15, 3, 70);
		displayTextInOneLine("A", arial14, 4, 90);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	malfActivateDisplayMemory																					//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the temperature that activate Pressure Malfunction. This allow user to see the temperature but they 	//
//	cannot change it. No adjustment. 																								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void malfActivateDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_MALFActivate], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemSettingsChinese, 4, 11, 0, 0, 15);
	}
	drawIntegerRightAligned(digitCalibri4x15, modbus_ero_reg_rcv[MALF_ACTIVATE].ivalue/10, 4, 15, 3, 80);
	showDegree(3, 80);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	malfDeactivateDisplayMemory																					//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the temperature that deactivate Pressure Malfunction. This allow user to see the temperature but they //
//	cannot change it. No adjustment. 																								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void malfDeactivateDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_MALFDeactivate], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemSettingsChinese, 12, 19, 0, 0, 8);
	}
	drawIntegerRightAligned(digitCalibri4x15, modbus_ero_reg_rcv[MALF_DEACTIVATE].ivalue/10, 4, 15, 3, 80);
	showDegree(3, 80);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	hpcFanOnDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the temperature that turn on Pressure Control. This allow user to see the temperature but they 		//
//	cannot change it. No adjustment. 																								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hpcFanOnDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_HPCFanOn], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemSettingsChinese, 20, 27, 0, 0, 10);
	}
	drawIntegerRightAligned(digitCalibri4x15, modbus_ero_reg_rcv[HPC_ACTIVATE].ivalue/10, 4, 15, 3, 80);
	showDegree(3, 80);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	hpcFanOffDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the temperature that turn off Pressure Control. This allow user to see the temperature but they 		//
//	cannot change it. No adjustment. 																								//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hpcFanOffDisplayMemory(void)
{
	highlightOneLine(0);
	clearOneLine(2);
	clearOneLine(4);
	clearOneLine(6);
	if (currentLanguage != CHINESE)
	{
		doScrolling(currentTextTable[TEXT_INDEX_HPCFanOff], "\0", "\0", "\0");
	}
	else
	{
		displayChineseTextInOneLineHighlighted(systemSettingsChinese, 28, 35, 0, 0, 10);
	}
	drawIntegerRightAligned(digitCalibri4x15, modbus_ero_reg_rcv[HPC_DEACTIVATE].ivalue/10, 4, 15, 3, 80);
	//drawDigit(digitCalibri4x15, DECIMAL_POINT_INDEX, 4, 15, 3, 60);
	//drawIntegerRightAligned(digitCalibri4x15, (numDigit%10)/10, 4, 15, 3, 80);
	showDegree(3, 80);
}

/************************************************Displaying System Interface Closing*************************************************/

/***************************************************Display the About Menu Opening***************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	showVersionNumber																							//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display the the hardware and firmware version and display hardware and display firmware revision. Since 		//																							//
//	firmware and hardware keep changing until launching, keeping track of how many version and revision have the firmware and 		//
//	hardware go through is important. 																								//
//																																	//
//	This routine will display the current version and revision. 																	//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showVersionNumber(int versionNumber, char page, char col)
{
	unsigned char minor = versionNumber & 0xff;
	unsigned char major = (versionNumber & 0xff00) >> 8;

	// Note: Ascii code for digit 0-9 is used. Thus the range is from 48 to 57 (or 30h to 39h)(inclusive) for 0-9 and 97 to 122 (or 61h to 7Ah)(inclusive) for a-z
	/*if ( (major<48) || ((major>57) && (major<97)) || (major>122) )
	{
		major = 45; // ascii code for '-'
	}

	if ( (minor<48) || ((minor>57) && (minor<97)) || (minor>122) )
	{
		minor = 45; // ascii code for '-'
	}*/

	drawCharacter(major, arial14, page, col);
	drawCharacter('.', arial14, page, endOfMajorColumn-1);
	drawCharacter(minor, arial14, page, endOfMajorColumn);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																	//
//	Function Name	: 	aboutMenuDisplayMemory																						//
//	Returned Value	: 	None																										//
//																																	//
//	This routine will display text and then call showVersionNumber to display the version and revision number the firmware and 		//
//	hardware are currently. 																										//
//																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int static cc = 0;
int static bb = 0;
void aboutMenuDisplayMemory(void)  //About tab in main menu
{
	aboutMenuItems = TRUE;
	char first = 2;
	char second = 2;
	char third = 2;
	char four = 2;
	if (currentLanguage != CHINESE)
	{
		if(currentLanguage == FRENCH)
		{
			first = 0;
			second = 0;
			third = 0;
			four = 0;
		}
		displayTextInOneLine(currentTextTable[TEXT_INDEX_ControlHardware], arial14, 0, first);
		displayTextInOneLine(currentTextTable[TEXT_INDEX_ControlFirmware], arial14, 2, second);
		displayTextInOneLine(currentTextTable[TEXT_INDEX_DisplayHardware], arial14, 4, third);
		displayTextInOneLine(currentTextTable[TEXT_INDEX_DisplayFirmware], arial14, 6, four);

		showVersionNumber(modbus_ro_reg_rcv[HARDWARE_VERSION].ivalue, 0, pgOneCol+2);
		showVersionNumber(modbus_ro_reg_rcv[FIRMWARE_VERSION].ivalue, 2, pgTwoCol+3);
		/*if(justClickOK)
		{
			justClickOK = FALSE;
			clearArea(2,pgTwoCol+2, 3, 127);
			showVersionNumber(0x617a +cc - bb, 2, pgTwoCol+2);//+ cc
			bb += 1;
			if(bb == 65)
			{
				cc += 0x100;
				bb = 0;
			}

		}*/
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_HARDWARE_REV].ivalue, 4, pgThreeCol+2);
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_FIRMWARE_REV].ivalue, 6, pgFourCol+2);
	}
	else
	{
		displayChineseTextInOneLine(aboutChinese, 0, 3, 0, 0, 2);
		displayChineseTextInOneLine(aboutChinese, 6, 9, 0, 2, 2);
		displayChineseTextInOneLine(aboutChinese, 12, 16, 0, 4, 2);
		displayChineseTextInOneLine(aboutChinese, 19, 23, 0, 6, 2);

		showVersionNumber(modbus_ro_reg_rcv[HARDWARE_VERSION].ivalue, 0, 70);
		showVersionNumber(modbus_ro_reg_rcv[FIRMWARE_VERSION].ivalue, 2, 70);
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_HARDWARE_REV].ivalue, 4, 80);
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_FIRMWARE_REV].ivalue, 6, 80);
	}
	/*if(justClickOK)
	{
		justClickOK = FALSE;
		showVersionNumber(modbus_ro_reg_rcv[HARDWARE_VERSION].ivalue, 0, 112);
		showVersionNumber(modbus_ro_reg_rcv[FIRMWARE_VERSION].ivalue, 2, 112);
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_HARDWARE_REV].ivalue, 4, 112);
		showVersionNumber(modbus_wo_reg_snd[DISPLAY_FIRMWARE_REV].ivalue, 6, 112);
	}*/
	/*showVersionNumber(modbus_ro_reg_rcv[HARDWARE_VERSION].ivalue, 0, pgOneCol+3);
	showVersionNumber(modbus_ro_reg_rcv[FIRMWARE_VERSION].ivalue, 2, pgTwoCol+3);
	showVersionNumber(modbus_wo_reg_snd[DISPLAY_HARDWARE_REV].ivalue, 4, pgThreeCol+3);
	showVersionNumber(modbus_wo_reg_snd[DISPLAY_FIRMWARE_REV].ivalue, 6, pgFourCol+3);*/
}
void displayBootloadMessage()
{
	//"Go_to_Bootload_Mode..._"
	displayTextInMultipleLines("Going into Bootloader Mode... ", arial14, &pgg, &ncol, false);
}

/***************************************************Display the About Menu Closing***************************************************/
/*** END OF FILE ***/
/*----------------------------------------------------------------------------Function Definitions Closing----------------------------------------------------------------------------------------*/
