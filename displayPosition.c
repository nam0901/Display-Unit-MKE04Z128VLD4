#include "displayPosition.h"
#include "OLED.h"
#include "PE_Types.h"
#include "button.h"
#include "Events.h"
#include "display_UART.h"
#include "stdbool.h"
#include "displayMemory.h"
#include "textTable.h"
#include "drawFunctions.h"

uint16_union modbus_ro_reg_rcv[RO_REG_LEN];		//read only registers controller copy
byte modbus_rw_coil_rcv[RW_COIL_LEN/8+1];		//read/write coils controller copy
uint16_union modbus_ero_reg_rcv[ERO_REG_LEN+1]; //extended read only registers, modbus 501-509 599

#define USER_LEVEL_PASSWORD		22
#define SYSTEM_LEVEL_PASSWORD	66

/* Function Prototypes */
static void mainScreenPosition(void);
static void mainMenuPosition(void);
static void userInterfacePosition(void);
static void systemInterfacePosition(void);
static void enterPasswordPosition(void);
static void aboutMenuPosition(void);
static void versionNumberMenuPosition(void);

static void turnOnAllPixelsPosition(void);

static void alarmPosition(void);

//static int coolingSetPointMax(void);
//static int coolingSetPointMin(void);
//static int coolingDifferentialMax(void);
//static int coolingDifferentialMin(void);
static void coolingSetPointPosition(void);
static void coolingDifferentialPosition(void);
static void heatingSetPointPosition(void);
static void heatingDifferentialPosition(void);
static void highTempAlarmPosition(void);
static void lowTempAlarmPosition(void);
static void tempScalePosition(void);
static void hysteresisPosition(void);
static void languagePosition(void);
static void passwordPosition(void);

static void groupControlModePosition(void);
static void modePosition(void);
static void standAloneModePosition(void);
static void leadLagModePosition(void);
static void unitIdentificationPosition(void);
static void differential2Position(void);
static void controlStrategyPosition(void);
static void masterSlaveModePosition(void);
static void numberOfUnitsPosition(void);
static void doorSwitchPosition(void);
static void disableSwitchAlarmPosition(void);
static void alarmRelayOutputPosition(void);
static void compressorRestartDelayPosition(void);
static void compressorCurrentPosition(void);
static void evapOutTempSensorPosition(void);
static void systemSettingsPosition(void);
static void currentLimitSettingsPosition(void);
static void malfDeactivatePosition(void);
static void malfActivatePosition(void);
static void hpcFanOnPosition(void);
static void hpcFanOffPosition(void);

void checkDisplayPosition(void)
{
	if (updateDisplayPosition)
	{
		updateDisplayPosition = false;
		updateDisplayMemory = TRUE; //true;
		switch (currentPosition.displayLevel)
		{
			/******************** Startup Screens **************************/
			/* Display Company logo for 1 second at Start-up */
			case COMPANY_ICON_POSITION:
				refreshScreen = true;
				currentPosition.displayLevel = ICON_ALL_ON_POSITION;
				break;

				/* Display All Icons for 1 second at Start-up */
			case ICON_ALL_ON_POSITION:
				refreshScreen = true;
				currentPosition.displayLevel = MAIN_SCREEN_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

				/******************** Main Screen **************************/
			case MAIN_SCREEN_POSITION:
				mainScreenPosition();
				break;

			case HIGH_TEMP_ALARM_POSITION: case LOW_TEMP_ALARM_POSITION:
			case DOOR_ALARM_POSITION: case PHASE_MISSING_ALARM_POSITION:
			case HIGH_TEMP_SHUTDOWN_POSITION: case COMPRESSOR_THERMAL_OVERLOAD_ALARM_POSITION:
			case OVER_VOLTAGE_ALARM_POSITION: case UNDER_VOLTAGE_ALARM_POSITION:
			case BAD_BOARD_ALARM_POSITION: case HIGH_PRESSURE_ALARM_POSITION:
			case FROST_ALARM_POSITION: case DISABLE_SWITCH_ALARM_POSITION:
			case COIL_TEMP_SENSOR_FAULT_ALARM_POSITION: case INLET_TEMP_SENSOR_FAULT_ALARM_POSITION:
			case OUTLET_TEMP_SENSOR_FAULT_ALARM_POSITION: case OVER_CURRENT_ALARM_POSITION:
			case COMPRESSOR_RESTART_DELAY_COUNT_DOWN_POSITION: case LOST_COMMUNICATION_POSITION:
			case GROUP_CONTROL_ALARM_POSITION:
				alarmPosition();
				break;

				/******************** Main Menu **************************/
			case MAIN_MENU_POSITION:				mainMenuPosition();				break;
			case USER_INTERFACE_POSITION:			userInterfacePosition();		break;
			case SYSTEM_INTERFACE_POSITION:			systemInterfacePosition();		break;
			case USER_PASSWORD_POSITION:
			case SYSTEM_PASSWORD_POSITION:			enterPasswordPosition();		break;
			case ABOUT_POSITION:					aboutMenuPosition();			break;
				/******************** User Interface **************************/
			case COOLING_SET_POINT_POSITION:		coolingSetPointPosition();		break;
			case COOLING_DIF_POSITION:				coolingDifferentialPosition();	break;
			case HEATING_SET_POINT_POSITION:		heatingSetPointPosition();		break;
			case HEATING_DIF_POSITION:				heatingDifferentialPosition();	break;
			case HIGH_TEMP_ALARM_SP_POSITION:		highTempAlarmPosition();		break;
			case LOW_TEMP_ALARM_SP_POSITION:		lowTempAlarmPosition();			break;
			case TEMP_SCALE_POSITION:				tempScalePosition();			break;
			case HYSTERESIS_POSITION:				hysteresisPosition();			break;
			case PASSWORD_POSITION:					passwordPosition();				break;
			case LANGUAGE_MENU_POSITION:			languagePosition();				break;
				/******************** System Interface ************************/
			case GROUP_CONTROL_MODE_POSITION:		groupControlModePosition();		break;
			case MODE_POSITION:						modePosition();					break;
			case STAND_ALONE_MODE_POSITION:			standAloneModePosition();		break;
			case LEAD_LAG_MODE_POSITION:			leadLagModePosition();			break;
			case MASTER_SLAVE_MODE_POSITION:		masterSlaveModePosition();		break;
			case UNIT_IDENTIFICATION_POSITION:		unitIdentificationPosition();	break;
			case DIFFERENTIAL_2_POSITION:			differential2Position();		break;
			case NUMBER_OF_UNITS_POSITION:			numberOfUnitsPosition();		break;
			case CONTROL_STRATEGY_POSITION:			controlStrategyPosition();		break;

			case DOOR_SWITCH_POSITION:				doorSwitchPosition();			break;
			case DISABLE_SWITCH_POSITION:			disableSwitchAlarmPosition();	break;
			case ALARM_RELAY_OUTPUT_POSITION:		alarmRelayOutputPosition();		break;
			case COMPRESSOR_RESTART_DELAY_POSITION:	compressorRestartDelayPosition();break;
			case COMPRESSOR_CURRENT_POSITION:		compressorCurrentPosition();	break;
			case EVAP_OUT_TEMP_SENSOR_POSITION:		evapOutTempSensorPosition();	break;

			case DISPLAY_SYS_SETTINGS_MENU_POSITION:systemSettingsPosition();		break;
			case CURRENT_LIMIT_SETTINGS_POSITION:	currentLimitSettingsPosition();	break;
			case MALF_DEACTIVATE_POSITION:			malfDeactivatePosition();		break;
			case MALF_ACTIVATE_POSITION:			malfActivatePosition();			break;
			case HPC_FAN_ON_POSITION:				hpcFanOnPosition();				break;
			case HPC_FAN_OFF_POSITION:				hpcFanOffPosition();			break;
			case CHECK_ALL_PIXELS:					turnOnAllPixelsPosition();		break;//Push four buttons at same time
			default: break;
		}
	}
 }

void mainScreenPosition(void)
{
	if (releasedOK)
	{
		releasedOK = false;
		// Turn off alarm timer and clear its counter
		TI1_alarmTimer_Flag = false;
		TI1_alarmTimer = 0;
		if (currentPosition.lineNumber == INLET_LINENUM)
		{
			currentPosition.displayLevel = MAIN_MENU_POSITION;
			currentPosition.lineNumber   = INLET_LINENUM;
		}
		else
		{
			currentPosition.lineNumber   = INLET_LINENUM;
			refreshScreen = true;
		}
	}
	else if (releasedBack)
	{
		releasedBack = false;
		if (currentPosition.lineNumber != INLET_LINENUM)
		{
			currentPosition.lineNumber   = INLET_LINENUM;
			refreshScreen = true;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		refreshScreen = true;
		if (currentPosition.lineNumber == INLET_LINENUM)
		{
			currentPosition.lineNumber = COIL_LINENUM;
		}
		else if (currentPosition.lineNumber == COIL_LINENUM)
		{
			if (modbus_rw_coil_rcv[PROBE2_PRESENT/8] & PROBE2_PRESENT_F) //probe2 present
			{
				currentPosition.lineNumber = OUTLET_LINENUM;
			}
			else
			{
				currentPosition.lineNumber = INLET_LINENUM;
			}
		}
		else if (currentPosition.lineNumber == OUTLET_LINENUM)
		{
			currentPosition.lineNumber = INLET_LINENUM;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		refreshScreen = true;
		if (currentPosition.lineNumber == COIL_LINENUM)
		{
			currentPosition.lineNumber = INLET_LINENUM;
		}
		else if (currentPosition.lineNumber == INLET_LINENUM)
		{
			if (modbus_rw_coil_rcv[PROBE2_PRESENT/8] & PROBE2_PRESENT_F) //probe2 present
			{
				currentPosition.lineNumber = OUTLET_LINENUM;
			}
			else
			{
				currentPosition.lineNumber = COIL_LINENUM;
			}
		}
		else if (currentPosition.lineNumber == OUTLET_LINENUM)
		{
			currentPosition.lineNumber = COIL_LINENUM;
		}
	}
}

void mainMenuPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_SCREEN_POSITION;
		currentPosition.lineNumber   = INLET_LINENUM;
		refreshScreen = true;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		switch (currentPosition.lineNumber)
		{
			case 1:
				if (modbus_rw_coil_rcv[PASSWORD_ENABLED/8] & PASSWORD_ENABLED_F)
				{
					userInput = 0;
					currentPosition.displayLevel = USER_PASSWORD_POSITION;
					refreshScreen = true;
				}
				else
				{
					currentPosition.displayLevel = USER_INTERFACE_POSITION;
				}
				break;
			case 2:
				userInput = 0;
				currentPosition.displayLevel = SYSTEM_PASSWORD_POSITION;
				refreshScreen = true;
				break;

			case 3:
				currentPosition.displayLevel = ABOUT_POSITION;
				refreshScreen = true;
				break;

			default: break;
		}
		currentPosition.lineNumber = INLET_LINENUM;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		switch (currentPosition.lineNumber)
		{
		case 1: case 2:
			currentPosition.lineNumber = INLET_LINENUM;
			break;

		case 3:
			currentPosition.lineNumber = OUTLET_LINENUM;
			break;

		default: break;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		switch (currentPosition.lineNumber)
		{
		case 1:
			currentPosition.lineNumber = OUTLET_LINENUM;
			break;

		case 2: case 3:
			currentPosition.lineNumber = COIL_LINENUM;
			break;

		default: break;
		}
	}
}

void enterPasswordPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_MENU_POSITION;
		currentPosition.lineNumber   = 1;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		if ((userInput == USER_LEVEL_PASSWORD) && (currentPosition.displayLevel == USER_PASSWORD_POSITION))
		{
			currentPosition.displayLevel = USER_INTERFACE_POSITION;
			currentPosition.lineNumber   = INLET_LINENUM;
		}
		else if ((userInput == SYSTEM_LEVEL_PASSWORD) && (currentPosition.displayLevel == SYSTEM_PASSWORD_POSITION))
		{
			currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
			currentPosition.lineNumber   = INLET_LINENUM;
		}
		else
		{
			currentPosition.displayLevel = MAIN_MENU_POSITION;
			currentPosition.lineNumber   = INLET_LINENUM;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput >= 99)
		{
			userInput = 0;
		}
		else
		{
			userInput++;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput <= 0)
		{
			userInput = 99;
		}
		else
		{
			userInput--;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			updateScreenTimerDone = false;

			if (userInput >= 99)
			{
				userInput = 0;
			}
			else
			{
				userInput++;
			}
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			updateScreenTimerDone = false;

			if (userInput <= 0)
			{
				userInput = 99;
			}
			else
			{
				userInput--;
			}
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void userInterfacePosition(void)
{
	//notHeaterDisplay = false;
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_MENU_POSITION;
		currentPosition.lineNumber   = INLET_LINENUM;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		refreshScreen = true;
		parameterIsSet = false;
		if(!(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F) && (currentPosition.lineNumber >= 3))//heater not present
		{
			notHeaterDisplay = true;
			currentPosition.lineNumber += 2; //add two to skip heating set point and heating differential.
			if(currentPosition.lineNumber > 10)
			{
				currentPosition.lineNumber = 10;
			}
		}
		switch (currentPosition.lineNumber)
		{
			case 1:
				currentPosition.displayLevel = COOLING_SET_POINT_POSITION;
				userInput = modbus_rw_reg_rcv[COOLING_SP].ivalue;
				break;

			case 2:
				currentPosition.displayLevel = COOLING_DIF_POSITION;
				userInput = modbus_rw_reg_rcv[COOLING_DIF_1].ivalue;
				break;

			case 3:
				currentPosition.displayLevel = HEATING_SET_POINT_POSITION;
				userInput = modbus_rw_reg_rcv[HEATING_SP].ivalue;
				break;

			case 4:
				currentPosition.displayLevel = HEATING_DIF_POSITION;
				userInput = modbus_rw_reg_rcv[HEATING_DIF].ivalue;
				break;

			case 5:
				currentPosition.displayLevel = HIGH_TEMP_ALARM_SP_POSITION;
				userInput = modbus_rw_reg_rcv[HIGH_TEMP_ALARM_SP].ivalue;
				break;

			case 6:
				currentPosition.displayLevel = LOW_TEMP_ALARM_SP_POSITION;
				userInput = modbus_rw_reg_rcv[LOW_TEMP_ALARM_SP].ivalue;
				break;

			case 7:
				currentPosition.displayLevel = TEMP_SCALE_POSITION;
				if (modbus_rw_coil_rcv[UNIT_OF_MEASURE/8] & UNIT_OF_MEASURE_F)
				{
					currentPosition.lineNumber = 1; // Degree F
				}
				else
				{
					currentPosition.lineNumber = 2; // Degree C
				}
				break;

			case 8:
				currentPosition.displayLevel = HYSTERESIS_POSITION;
				if (modbus_rw_coil_rcv[COOL_HYSTERESIS/8] & COOL_HYSTERESIS_F)
				{
					currentPosition.lineNumber = 2; // Negative
				}
				else
				{
					currentPosition.lineNumber = 1; // Positive
				}
				break;

			case 9:
				currentPosition.displayLevel = LANGUAGE_MENU_POSITION;
				currentPosition.lineNumber = 1;
				break;

			case 10:
				currentPosition.displayLevel = PASSWORD_POSITION;
				if (modbus_rw_coil_rcv[PASSWORD_ENABLED/8] & PASSWORD_ENABLED_F)
				{
					currentPosition.lineNumber = 2; // ON
				}
				else
				{
					currentPosition.lineNumber = 1;
				}
				break;

			default:
				break;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (currentPosition.lineNumber > 1)
		{
			currentPosition.lineNumber -= 1;
		}
		else
		{
			currentPosition.lineNumber = 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
		{
			if (currentPosition.lineNumber < 10)
			{
				currentPosition.lineNumber += 1;
			}
			else
			{
				currentPosition.lineNumber = 10;
			}
		}
		else //heater not present
		{
			if (currentPosition.lineNumber < 8)
			{
				currentPosition.lineNumber += 1;
			}
			else
			{
				currentPosition.lineNumber = 8;
			}
		}
	}
}

void systemInterfacePosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_MENU_POSITION;
		currentPosition.lineNumber   = OUTLET_LINENUM;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		refreshScreen = true;
		parameterIsSet = false;
		switch (currentPosition.lineNumber)
		{
			case 1:
				currentPosition.displayLevel = GROUP_CONTROL_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

			case 2:
				currentPosition.displayLevel = DOOR_SWITCH_POSITION;
				if (modbus_rw_coil_rcv[DOOR_SWITCH/8] & DOOR_SWITCH_F)
				{
					currentPosition.lineNumber = OUTLET_LINENUM;
				}
				else
				{
					currentPosition.lineNumber = INLET_LINENUM;
				}
				break;

			case 3:
				currentPosition.displayLevel = DISABLE_SWITCH_POSITION;
				if (modbus_rw_coil_rcv[DISABLE_SWITCH/8] & DISABLE_SWITCH_F)
				{
					currentPosition.lineNumber = OUTLET_LINENUM;
				}
				else
				{
					currentPosition.lineNumber = INLET_LINENUM;
				}
				break;

			case 4:
				currentPosition.displayLevel = ALARM_RELAY_OUTPUT_POSITION;
				if (modbus_rw_coil_rcv[ALARM_OUTPUT/8] & ALARM_OUTPUT_F)
				{
					currentPosition.lineNumber = OUTLET_LINENUM;
				}
				else
				{
					currentPosition.lineNumber = INLET_LINENUM;
				}
				break;

			case 5:
				currentPosition.displayLevel = COMPRESSOR_RESTART_DELAY_POSITION;
				userInput = modbus_rw_reg_rcv[COMP_DELAY_TIME].ivalue;
				break;

			case 6:
				currentPosition.displayLevel = COMPRESSOR_CURRENT_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

			case 7:
				currentPosition.displayLevel = EVAP_OUT_TEMP_SENSOR_POSITION;
				if(modbus_rw_coil_rcv[PROBE2_PRESENT/8] & PROBE2_PRESENT_F)  //Probe 2 present
				{
					currentPosition.lineNumber = INLET_LINENUM;
				}
				else  //Probe 2 absent
				{
					currentPosition.lineNumber = OUTLET_LINENUM;
				}
				break;

			case 8:
				currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

			default:
				break;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (currentPosition.lineNumber > 1)
		{
			currentPosition.lineNumber -= 1;
		}
		else
		{
			currentPosition.lineNumber = 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (currentPosition.lineNumber < 8)
		{
			currentPosition.lineNumber += 1;
		}
		else
		{
			currentPosition.lineNumber = 8;
		}
	}
}

void aboutMenuPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_MENU_POSITION;
		currentPosition.lineNumber   = 3;
	}
	else if (releasedOK)
	{
		releasedOK = false;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void turnOnAllPixelsPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
	}
	else if (releasedOK)
	{
		releasedOK = false;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void alarmPosition(void)
{
	if (releasedOK)
	{
		releasedOK = false;
		// Turn off alarm timer and clear its counter
		TI1_alarmTimer_Flag = false;
		TI1_alarmTimer = 0;
		currentPosition.displayLevel = MAIN_MENU_POSITION;
		currentPosition.lineNumber   = INLET_LINENUM;
		refreshScreen = true;
	}
	else if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MAIN_SCREEN_POSITION;
		currentPosition.lineNumber   = INLET_LINENUM;
		refreshScreen = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		// Turn off alarm timer and clear its counter
		TI1_alarmTimer_Flag = false;
		TI1_alarmTimer = 0;
		currentPosition.displayLevel = MAIN_SCREEN_POSITION;
		currentPosition.lineNumber   = OUTLET_LINENUM;
		refreshScreen = true;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		// Turn off alarm timer and clear its counter
		TI1_alarmTimer_Flag = false;
		TI1_alarmTimer = 0;
		currentPosition.displayLevel = MAIN_SCREEN_POSITION;
		currentPosition.lineNumber   = COIL_LINENUM;
		refreshScreen = true;
	}
}

/*int coolingSetPointMax(void)
{
	int result;

	// if differential +/- is negative
	if (modbus_rw_coil_rcv[COOL_HYSTERESIS/8] & COOL_HYSTERESIS_F)
	{
		result = modbus_ero_reg_rcv[COOL_ON_MAX].ivalue;
	}
	else // if differential +/- is positive
	{
		/* by definition, CoolOn = CoolingSP+Cooling Dif
		 * So, CoolingSP = CoolOn - Cooling Dif
		 * CoolingSP : result
		 * CoolOn : modbus_ero_reg_rcv[COOL_ON_MAX].ivalue
		 * Cooling Dif : modbus_rw_reg_rcv[COOLING_DIF_1].ivalue*/
		/*	result = modbus_ero_reg_rcv[COOL_ON_MAX].ivalue - modbus_rw_reg_rcv[COOLING_DIF_1].ivalue;
	}

	return result;
}*/

/*int coolingSetPointMin(void)
{
	int result;

	// if differential +/- is negative
	if (modbus_rw_coil_rcv[COOL_HYSTERESIS/8] & COOL_HYSTERESIS_F)
	{
		result = modbus_ero_reg_rcv[COOL_OFF_MIN].ivalue + modbus_rw_reg_rcv[COOLING_DIF_1].ivalue;
	}
	else // if differential +/- is positive
	{
		result = modbus_ero_reg_rcv[COOL_OFF_MIN].ivalue;
	}

	return result;
}*/

/*int coolingDifferentialMax(void)
{
	int result;

	// if differential +/- is negative
	if (modbus_rw_coil_rcv[COOL_HYSTERESIS/8] & COOL_HYSTERESIS_F)
	{
		result = modbus_rw_reg_rcv[COOLING_SP].ivalue - modbus_ero_reg_rcv[COOL_OFF_MIN].ivalue;
	}
	else // if differential +/- is positive
	{
		result = modbus_ero_reg_rcv[COOL_ON_MAX].ivalue - modbus_rw_reg_rcv[COOLING_SP].ivalue;
	}

	if (result > modbus_rw_reg_rcv[COOLING_DIF_1_MAX].ivalue)
	{
		return modbus_rw_reg_rcv[COOLING_DIF_1_MAX].ivalue;
	}
	else
	{
		return result;
	}
}*/

void coolingSetPointPosition(void)
{
	int coolingMax = modbus_ro_reg_rcv[COOL_SP_MAX].ivalue;
	int coolingMin = modbus_ro_reg_rcv[COOL_SP_MIN].ivalue; //do this to solve compilation time and stack call

	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = 1;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		// if not in degraded mode, allow the adjustment
		if ((!isInDegradedMode))
		{
			uint16 uart_write_return;
			// write to main board to update the coolingSP
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + COOLING_SP, false, userInput, COOLING_SP_F, 0);

			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (userInput < coolingMax)
				{
					if ((userInput < 1000) && (userInput >= -1000))
					{
						userInput += 1;
					}
					else
					{
						userInput += 10;
					}

					if (userInput > coolingMax)
					{
						userInput = coolingMax;
					}
				}
				else
				{
					userInput = coolingMax;
				}
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (userInput > coolingMin)
				{
					if (userInput <= 1000 && userInput > -1000)
					{
						userInput -= 1;
					}
					else
					{
						userInput -= 10;
					}

					if (userInput < coolingMin)
					{
						userInput = coolingMin;
					}
				}
				else
				{
					userInput = coolingMin;
				}
		}
	}
	else if (heldUp)
	{
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (updateScreenTimerDone)
				{
					if (userInput < coolingMax)
					{
						if (userInput < 1000 && userInput >= -1000)
						{
							userInput += 1;
						}
						else
						{
							userInput += 10;
						}

						if (userInput > coolingMax)
						{
							userInput = coolingMax;
						}
					}
					else
					{
						userInput = coolingMax;
					}
					updateScreenTimerDone = false;
				}
				else
				{
					if (!TI1_updateScreenTimerIsOn)
					{
						updateScreenRate = BUTTON_HELD_REFRESH_TIME;
						TI1_updateScreenTimerIsOn = true;
					}
				}
		}
	}
	else if (heldDown)
	{
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (updateScreenTimerDone)
				{
					if (userInput > coolingMin)
					{
						if (userInput <= 1000 && userInput > -1000)
						{
							userInput -= 1;
						}
						else
						{
							userInput -= 10;
						}

						if (userInput < coolingMin)
						{
							userInput = coolingMin;
						}
					}
					else
					{
						userInput = coolingMin;
					}
					updateScreenTimerDone = false;
				}
				else
				{
					if (!TI1_updateScreenTimerIsOn)
					{
						updateScreenRate = BUTTON_HELD_REFRESH_TIME;
						TI1_updateScreenTimerIsOn = true;
					}
				}
		}
	}
}

void coolingDifferentialPosition(void)
{
	int coolingDiffMax = modbus_ro_reg_rcv[COOL_DIF_1_MAX].ivalue;
	int coolingDifMin =  modbus_ro_reg_rcv[COOL_DIF_1_MIN].ivalue;
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = 2;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				uint16 uart_write_return;
				uart_write_return = display_uart_update(REGISTER, RW_REG_START + COOLING_DIF_1, false, userInput, COOLING_DIF_1_F, 0);
				// Validation Screen
				parameterIsSet = true;
				// A delay for UART to write the value to the ECU
				TI1_UARTWriteTimer_Flag = true;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (userInput < coolingDiffMax)
				{
					if ((userInput < 1000) && (userInput >= -1000))
					{
						userInput += 1;
					}
					else
					{
						userInput += 10;
					}

					if (userInput > coolingDiffMax)
					{
						userInput = coolingDiffMax;
					}
				}
				else
				{
					userInput = coolingDiffMax;
				}
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (userInput > coolingDifMin)
				{
					if (userInput <= 1000 && userInput > -1000)
					{
						userInput -= 1;
					}
					else
					{
						userInput -= 10;
					}

					if (userInput < coolingDifMin)
					{
						userInput = coolingDifMin;
					}
				}
				else
				{
					userInput = coolingDifMin;
				}
		}
	}
	else if (heldUp)
	{
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (updateScreenTimerDone)
				{
					if (userInput < coolingDiffMax)
					{
						if ((userInput < 1000) && (userInput >= -1000))
						{
							userInput += 1;
						}
						else
						{
							userInput += 10;
						}

						if (userInput > coolingDiffMax)
						{
							userInput = coolingDiffMax;
						}
					}
					else
					{
						userInput = coolingDiffMax;
					}
					updateScreenTimerDone = false;
				}
				else
				{
					if (!TI1_updateScreenTimerIsOn)
					{
						updateScreenRate = BUTTON_HELD_REFRESH_TIME;
						TI1_updateScreenTimerIsOn = true;
					}
				}
		}
	}
	else if (heldDown)
	{
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
				if (updateScreenTimerDone)
				{
					if (userInput > coolingDifMin)
					{
						if (userInput <= 1000 && userInput > -1000)
						{
							userInput -= 1;
						}
						else
						{
							userInput -= 10;
						}

						if (userInput < coolingDifMin)
						{
							userInput = coolingDifMin;
						}
					}
					else
					{
						userInput = coolingDifMin;
					}
					updateScreenTimerDone = false;
				}
				else
				{
					if (!TI1_updateScreenTimerIsOn)
					{
						updateScreenRate = BUTTON_HELD_REFRESH_TIME;
						TI1_updateScreenTimerIsOn = true;
					}
				}
		}
	}
}

void heatingSetPointPosition(void)
{
	int heatingSPMax = modbus_rw_reg_rcv[HEATING_SP_MAX].ivalue;
	int heatingSPMin = modbus_rw_reg_rcv[HEATING_SP_MIN].ivalue;
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = 3;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		uart_write_return = display_uart_update(REGISTER, RW_REG_START + HEATING_SP, false, userInput, HEATING_SP_F, 0);
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput < heatingSPMax)
		{
			if (userInput < 1000 && userInput >= -1000)
			{
				userInput += 1;
			}
			else
			{
				userInput += 10;
			}

			if (userInput > heatingSPMax)
			{
				userInput = heatingSPMax;
			}
		}
		else
		{
			userInput = heatingSPMax;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput > heatingSPMin)
		{
			if (userInput <= 1000 && userInput > -1000)
			{
				userInput -= 1;
			}
			else
			{
				userInput -= 10;
			}

			if (userInput < heatingSPMin)
			{
				userInput = heatingSPMin;
			}
		}
		else
		{
			userInput = heatingSPMin;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < heatingSPMax)
			{
				if (userInput < 1000 && userInput >= -1000)
				{
					userInput += 1;
				}
				else
				{
					userInput += 10;
				}

				if (userInput > heatingSPMax)
				{
					userInput = heatingSPMax;
				}
			}
			else
			{
				userInput = heatingSPMax;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > heatingSPMin)
			{
				if (userInput <= 1000 && userInput > -1000)
				{
					userInput -= 1;
				}
				else
				{
					userInput -= 10;
				}

				if (userInput < heatingSPMin)
				{
					userInput = heatingSPMin;
				}
			}
			else
			{
				userInput = heatingSPMin;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void heatingDifferentialPosition(void)
{
	int heatingDiffMax = modbus_rw_reg_rcv[HEATING_DIF_MAX].ivalue;
	int heatingDiffMin = modbus_rw_reg_rcv[HEATING_DIF_MIN].ivalue;
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = 4;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		uart_write_return = display_uart_update(REGISTER, RW_REG_START + HEATING_DIF, false, userInput, HEATING_DIF_F, 0);

		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput < heatingDiffMax)
		{
			if (userInput < 1000 && userInput >= -1000)
			{
				userInput += 1;
			}
			else
			{
				userInput += 10;
			}
			if (userInput > heatingDiffMax)
			{
				userInput = heatingDiffMax;
			}
		}
		else
		{
			userInput = heatingDiffMax;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput > heatingDiffMin)
		{
			if (userInput <= 1000 && userInput > -1000)
			{
				userInput -= 1;
			}
			else
			{
				userInput -= 10;
			}

			if (userInput < heatingDiffMin)
			{
				userInput = heatingDiffMin;
			}
		}
		else
		{
			userInput = heatingDiffMin;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < heatingDiffMax)
			{
				if (userInput < 1000 && userInput >= -1000)
				{
					userInput += 1;
				}
				else
				{
					userInput += 10;
				}

				if (userInput > heatingDiffMax)
				{
					userInput = heatingDiffMax;
				}
			}
			else
			{
				userInput = heatingDiffMax;
			}
			updateScreenTimerDone = false;
		}

		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}

	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > heatingDiffMin)
			{
				if (userInput <= 1000 && userInput > -1000)
				{
					userInput -= 1;
				}
				else
				{
					userInput -= 10;
				}

				if (userInput < heatingDiffMin)
				{
					userInput = heatingDiffMin;
				}
			}
			else
			{
				userInput = heatingDiffMin;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void highTempAlarmPosition(void)
{
	int highTempMax = modbus_rw_reg_rcv[HIGH_TEMP_SP_MAX].ivalue;
	int highTempMin = modbus_rw_reg_rcv[HIGH_TEMP_SP_MIN].ivalue;

	char lineNumTemp = 3; //heater not present
	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F) //heater present
	{
		lineNumTemp = 5;
	}
	if (releasedBack)
	{
		//heater not present
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		uart_write_return = display_uart_update(REGISTER, RW_REG_START + HIGH_TEMP_ALARM_SP, false, userInput, HIGH_TEMP_ALARM_SP_F, 0);
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput < highTempMax)
		{
			if ((userInput < 1000) && (userInput >= -1000))
			{
				userInput += 1;
			}
			else
			{
				userInput += 10;
			}

			if (userInput > highTempMax)
			{
				userInput = highTempMax;
			}
		}
		else
		{
			userInput = highTempMax;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput > highTempMin)
		{
			if ((userInput < 1000) && (userInput >= -1000))
			{
				userInput -= 1;
			}
			else
			{
				userInput -= 10;
			}

			if (userInput < highTempMin)
			{
				userInput = highTempMin;
			}
		}
		else
		{
			userInput = highTempMin;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < highTempMax)
			{
				if ((userInput < 1000) && (userInput >= -1000))
				{
					userInput += 1;
				}
				else
				{
					userInput += 10;
				}

				if (userInput > highTempMax)
				{
					userInput = highTempMax;
				}
			}
			else
			{
				userInput = highTempMax;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > highTempMin)
			{
				if ((userInput < 1000) && (userInput >= -1000))
				{
					userInput -= 1;
				}
				else
				{
					userInput -= 10;
				}

				if (userInput < highTempMin)
				{
					userInput = highTempMin;
				}
			}
			else
			{
				userInput = highTempMin;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void lowTempAlarmPosition(void)
{
	int lowTempMax = modbus_rw_reg_rcv[LOW_TEMP_SP_MAX].ivalue;
	int lowTempMin = modbus_rw_reg_rcv[LOW_TEMP_SP_MIN].ivalue;
	char lineNumTemp = 4; // heater not present

	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
	{
		lineNumTemp = 6;
	}
	if (releasedBack)
	{
		//heater not presents
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		// write to main board to update the coolingSP
		uart_write_return = display_uart_update(REGISTER, RW_REG_START + LOW_TEMP_ALARM_SP, false, userInput, LOW_TEMP_ALARM_SP_F, 0);
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput < lowTempMax)
		{
			if ((userInput < 1000) && (userInput >= -1000))
			{
				userInput += 1;
			}
			else
			{
				userInput += 10;
			}

			if (userInput > lowTempMax)
			{
				userInput = lowTempMax;
			}
		}
		else
		{
			userInput = lowTempMax;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput > lowTempMin)
		{
			if ((userInput < 1000) && (userInput >= -1000))
			{
				userInput -= 1;
			}
			else
			{
				userInput -= 10;
			}

			if (userInput < lowTempMin)
			{
				userInput = lowTempMin;
			}
		}
		else
		{
			userInput = lowTempMin;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < lowTempMax)
			{
				if ((userInput < 1000) && (userInput >= -1000))
				{
					userInput += 1;
				}
				else
				{
					userInput += 10;
				}

				if (userInput > lowTempMax)
				{
					userInput = lowTempMax;
				}
			}
			else
			{
				userInput = lowTempMax;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > lowTempMin)
			{
				if ((userInput < 1000) && (userInput >= -1000))
				{
					userInput -= 1;
				}
				else
				{
					userInput -= 10;
				}

				if (userInput < lowTempMin)
				{
					userInput = lowTempMin;
				}
			}
			else
			{
				userInput = lowTempMin;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void tempScalePosition(void)
{
	int lineNumTemp = 5; //heater not present


	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)//heater present
	{
		lineNumTemp = 7;
	}
	if (releasedBack)
	{
		//heater not presents
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		if(!isInDegradedMode)
		{
			uint16 uart_write_return;
			switch (currentPosition.lineNumber)
			{
			case 1: // degree F
				uart_write_return = display_uart_update(COIL, RW_COIL_START + UNIT_OF_MEASURE, true, 0, 0, UNIT_OF_MEASURE_F);
				break;

			case 2: // degree C
				uart_write_return = display_uart_update(COIL, RW_COIL_START + UNIT_OF_MEASURE, false, 0, 0, UNIT_OF_MEASURE_F);
				break;

			default: break;
			}
			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
			//updateWrtRd = 1;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if(!isInDegradedMode)
		{
			currentPosition.lineNumber = 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if(!isInDegradedMode)
		{
			currentPosition.lineNumber = 2;
		}
	}
}

void hysteresisPosition(void)
{
	char lineNumTemp = 6; //heater not present

	//heater present
	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)
	{
		lineNumTemp = 8;
	}

	if (releasedBack)
	{
		//heater not presents
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		// if not in degraded mode, allow users to adjust
		if (!isInDegradedMode)
		{
			uint16 uart_write_return;
			switch (currentPosition.lineNumber)
			{
				case 1: // positive
					uart_write_return = display_uart_update(COIL, RW_COIL_START + COOL_HYSTERESIS, false, 0, 0, COOL_HYSTERESIS_F);
					break;
				case 2: // negative
					uart_write_return = display_uart_update(COIL, RW_COIL_START + COOL_HYSTERESIS, true, 0, 0, COOL_HYSTERESIS_F);
					break;
				default: break;
			}
			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
			//updateWrtRd = 2;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
    	// if not in degraded mode, allow users to adjust
    	if (!isInDegradedMode)
    	{
    		currentPosition.lineNumber = 1;
    	}
	}
	else if (releasedDown)
	{
		releasedDown = false;
    	// if not in degraded mode, allow users to adjust
    	if (!isInDegradedMode)
    	{
    		currentPosition.lineNumber = 2;
    	}
	}
}

void languagePosition(void)
{
	char lineNumTemp = 7; //no heater present
	//heater present
	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)
	{
		lineNumTemp = 9;
	}
	if (releasedBack)
	{
		//heater not presents
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber   = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentLanguage = currentPosition.lineNumber;
		switch(currentLanguage)
		{
		case ENGLISH:
			currentTextTable = englishText;
			break;

		case FRENCH:
			currentTextTable = frenchText;
			break;

		case GERMAN:
			currentTextTable = germanText;
			break;

		case SPANISH:
			currentTextTable = spanishText;
			break;
		case ITALIAN:
			currentTextTable = italianText;
			break;
		case POLISH:
			currentTextTable = polishText;
			break;

		default: break;
		}
		//  write to main board to update language
		uint16 uart_write_return;
		uart_write_return = display_uart_update(REGISTER, ERO_REG_START + DISPLAY_LANGUAGE, false, currentLanguage, DISPLAY_LANGUAGE_F, 0);
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
		//updateWrtRd = 3;

	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (currentPosition.lineNumber > 1)
		{
			currentPosition.lineNumber -= 1;
		}
		else
		{
			currentPosition.lineNumber = 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (currentPosition.lineNumber < 6) //only have 6 languages
		{
			currentPosition.lineNumber += 1;
		}
		else
		{
			currentPosition.lineNumber = 6;
		}
	}
}

void passwordPosition(void)
{
	char lineNumTemp = 8; // heater not present
	//heater present
	if(modbus_rw_coil_rcv[HEATER_PRESENT/8] & HEATER_PRESENT_F)
	{
		lineNumTemp = 10;
	}
	if (releasedBack)
	{
		//heater not presents
		releasedBack = false;
		currentPosition.displayLevel = USER_INTERFACE_POSITION;
		currentPosition.lineNumber = lineNumTemp;
		notHeaterDisplay = false; //need to do this so that in User Interface display memory, don't do subtract of 2
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		switch (currentPosition.lineNumber)
		{
		case 1:
			uart_write_return = display_uart_update(COIL, RW_COIL_START + PASSWORD_ENABLED, false, 0, 0, PASSWORD_ENABLED_F);
			break;

		case 2:
			uart_write_return = display_uart_update(COIL, RW_COIL_START + PASSWORD_ENABLED, true, 0, 0, PASSWORD_ENABLED_F);
			break;

		default: break;
		}
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
		//updateWrtRd = 4;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = 1;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = 2;
	}
}

void groupControlModePosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 1;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		switch(currentPosition.lineNumber)
		{
		case 1: //Mode
			switch(modbus_rw_reg_rcv[GROUP_CONTROL_MODE].ivalue)
			{
			case 1:				//stand-alone mode
				currentPosition.lineNumber = INLET_LINENUM;
				currentPosition.displayLevel = MODE_POSITION;
				break;

			case 2:			//lead/lag mode
				currentPosition.lineNumber = OUTLET_LINENUM;
				currentPosition.displayLevel = MODE_POSITION;
				break;

			case 3:			//master/slave
				currentPosition.lineNumber = COIL_LINENUM;
				currentPosition.displayLevel = MODE_POSITION;
				break;
			default: break;
			}
			break;

		case 2: //Mode settings
			switch(modbus_rw_reg_rcv[GROUP_CONTROL_MODE].ivalue)
			{
			case 1:		//stand-alone mode
				currentPosition.displayLevel = STAND_ALONE_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

			case 2:	//lead/lag mode
				currentPosition.displayLevel = LEAD_LAG_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;

			case 3:	//master/slave mode
				currentPosition.displayLevel = MASTER_SLAVE_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;
			default: break;
			}
			break;
		default: break;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = INLET_LINENUM;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
}

void modePosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = GROUP_CONTROL_MODE_POSITION;
		currentPosition.lineNumber = INLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		switch(currentPosition.lineNumber)
		{
		case 1:
				// write to main board to update the group control mode
				uart_write_return = display_uart_update(REGISTER, RW_REG_START + GROUP_CONTROL_MODE, false, 1, GROUP_CONTROL_MODE_F, 0);
				// Validation Screen
				if(!uart_write_return)
				{
					parameterIsSet = true;
					// A delay for UART to write the value to the ECU
					TI1_UARTWriteTimer_Flag = true;
				}
			break;
		case 2:
				// write to main board to update the group control mode
				uart_write_return = display_uart_update(REGISTER, RW_REG_START + GROUP_CONTROL_MODE, false, 2, GROUP_CONTROL_MODE_F, 0);
				if(!uart_write_return)
				{
					// Validation Screen
					parameterIsSet = true;
					// A delay for UART to write the value to the ECU
					TI1_UARTWriteTimer_Flag = true;
				}
				uart_write_return = display_uart_update(REGISTER, RW_REG_START + GROUP_CONTROL_SIZE, false, 2, GROUP_CONTROL_SIZE_F, 0);
			break;
		case 3:
				// write to main board to update the group control mode
				uart_write_return = display_uart_update(REGISTER, RW_REG_START + GROUP_CONTROL_MODE, false, 3, GROUP_CONTROL_MODE_F, 0);
				if(!uart_write_return)
				{
					// Validation Screen
					parameterIsSet = true;
					// A delay for UART to write the value to the ECU
					TI1_UARTWriteTimer_Flag = true;
				}
			break;
		default: break;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		switch(currentPosition.lineNumber)
		{
		case 1: case 2:
			currentPosition.lineNumber = INLET_LINENUM;
			break;

		case 3:
			currentPosition.lineNumber = OUTLET_LINENUM;
			break;

		default: break;
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		switch(currentPosition.lineNumber)
		{
		case 1:
			currentPosition.lineNumber = OUTLET_LINENUM;
			break;

		case 2: case 3:
			currentPosition.lineNumber = COIL_LINENUM;
			break;

		default: break;
		}
	}
}

void standAloneModePosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = GROUP_CONTROL_MODE_POSITION;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
	}
	else if(releasedUp)
	{
		releasedUp = false;
	}
	else if(releasedDown)
	{
		releasedDown = false;
	}
}

void leadLagModePosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = GROUP_CONTROL_MODE_POSITION;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		switch(currentPosition.lineNumber)
		{
		case 1:
			currentPosition.displayLevel = UNIT_IDENTIFICATION_POSITION;
			userInput = modbus_rw_reg_rcv[UNIT_ID].ivalue;
			break;
		case 2:
			currentPosition.displayLevel = DIFFERENTIAL_2_POSITION;
			userInput = modbus_rw_reg_rcv[COOLING_DIF_2].ivalue;
			uint16 uart_write_return;
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + COOLING_DIF_2_MAX, false, 20, COOLING_DIF_2_MAX_F, 0);
			break;
		case 3:
			currentPosition.displayLevel = CONTROL_STRATEGY_POSITION;
			userInput = modbus_rw_reg_rcv[CONTROL_STRATEGY].ivalue;
			break;
		default: break;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		switch(currentPosition.lineNumber)
		{
			case 1: case 2:
				currentPosition.lineNumber = INLET_LINENUM;
				break;
			case 3:
				currentPosition.lineNumber = OUTLET_LINENUM;
				break;
			default: break;
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		switch(currentPosition.lineNumber)
		{
			case 1:
				currentPosition.lineNumber = OUTLET_LINENUM;
				break;
			case 2: case 3:
				currentPosition.lineNumber = COIL_LINENUM;
				break;
			default: break;
		}
	}
}

void unitIdentificationPosition(void)
{
	int maxNumOfUnit;
	switch(modbus_ro_reg_rcv[GROUP_CONT_MODE_STATUS].ivalue)
	{
		case 2:
			maxNumOfUnit = 2;
			break;
		case 3:
			maxNumOfUnit = modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue;
			break;
		default: break;
	}

	if(releasedBack)
	{
		releasedBack = false;
		switch(modbus_rw_reg_rcv[GROUP_CONTROL_MODE].ivalue)
		{
			case 2:
				currentPosition.displayLevel = LEAD_LAG_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;
			case 3:
				currentPosition.displayLevel = MASTER_SLAVE_MODE_POSITION;
				currentPosition.lineNumber = INLET_LINENUM;
				break;
			default: break;
		}
	}
	else if(releasedOK)
	{
		releasedOK = false;
		if (!isInDegradedMode)
		{
			uint16 uart_write_return;
			// write to main board to update the coolingSP
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + UNIT_ID, false, userInput, UNIT_ID_F, 0);

			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		if (!isInDegradedMode)
		{
			if (userInput < maxNumOfUnit)
			{
				userInput += 1;
				if (userInput > maxNumOfUnit)
				{
					userInput = maxNumOfUnit;
				}
			}
			else
			{
				userInput = maxNumOfUnit;
			}
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		if(!isInDegradedMode)
		{
			if (userInput > 1)
			{
				userInput -= 1;
				if (userInput < 1)
				{
					userInput = 1;
				}
			}
			else
			{
				userInput = 1;
			}
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < maxNumOfUnit)
			{
				userInput += 1;
				if (userInput > maxNumOfUnit)
				{
					userInput = maxNumOfUnit;
				}
			}
			else
			{
				userInput = maxNumOfUnit;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > 1)
			{
				userInput -= 1;
			}
			else
			{
				userInput = 1;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void differential2Position(void)
{
	int differential2Max = modbus_rw_reg_rcv[COOLING_DIF_2_MAX].ivalue;
	int differential2Min = modbus_rw_reg_rcv[COOLING_DIF_2_MIN].ivalue;
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = LEAD_LAG_MODE_POSITION;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		// if not in degraded mode, allow the adjustment
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{
			uint16 uart_write_return;
			// write to main board to update the coolingSP
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + COOLING_DIF_2, false, userInput, COOLING_DIF_2_F, 0);

			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput < differential2Max)
			{
				if ((userInput < 1000) && (userInput >= -1000))
				{
					userInput += 1;
				}
				else
				{
					userInput += 10;
				}

				if (userInput > differential2Max)
				{
					userInput = differential2Max;
				}
			}
			else
			{
				userInput = differential2Max;
			}
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput > differential2Min)
			{
				if (userInput <= 1000 && userInput > -1000)
				{
					userInput -= 1;
				}
				else
				{
					userInput -= 10;
				}

				if (userInput < differential2Min)
				{
					userInput = differential2Min;
				}
			}
			else
			{
				userInput = differential2Min;
			}
		}
	}
	else if (heldUp)
	{
		// if not in degraded mode, allow users to adjust
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{
			if (updateScreenTimerDone)
			{
				if (userInput < differential2Max)
				{
					if (userInput < 1000 && userInput >= -1000)
					{
						userInput += 1;
					}
					else
					{
						userInput += 10;
					}

					if (userInput > differential2Max)
					{
						userInput = differential2Max;
					}
				}
				else
				{
					userInput = differential2Max;
				}
				updateScreenTimerDone = false;
			}
			else
			{
				if (!TI1_updateScreenTimerIsOn)
				{
					updateScreenRate = BUTTON_HELD_REFRESH_TIME;
					TI1_updateScreenTimerIsOn = true;
				}
			}
		}
	}
	else if (heldDown)
	{
		// if not in degraded mode, allow users to adjust
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{
			if (updateScreenTimerDone)
			{
				if (userInput > differential2Min)
				{
					if (userInput <= 1000 && userInput > -1000)
					{
						userInput -= 1;
					}
					else
					{
						userInput -= 10;
					}

					if (userInput < differential2Min)
					{
						userInput = differential2Min;
					}
				}
				else
				{
					userInput = differential2Min;
				}
				updateScreenTimerDone = false;
			}
			else
			{
				if (!TI1_updateScreenTimerIsOn)
				{
					updateScreenRate = BUTTON_HELD_REFRESH_TIME;
					TI1_updateScreenTimerIsOn = true;
				}
			}
		}
	}
}

void controlStrategyPosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = LEAD_LAG_MODE_POSITION;
		currentPosition.lineNumber = COIL_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		if((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{
			uint16 uart_write_return;
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + CONTROL_STRATEGY, false, userInput, CONTROL_STRATEGY_F, 0);
			if((!uart_write_return))
			{
				// Validation Screen
				parameterIsSet = true;
				// A delay for UART to write the value to the ECU
				TI1_UARTWriteTimer_Flag = true;
			}
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput < 24)
			{
				userInput += 1;
			}
			else
			{
				userInput = 24;
			}
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput > 0)
			{
				userInput -= 1;
			}
			else
			{
				userInput = 0;
			}
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < 24)
			{
				userInput += 1;
			}
			else
			{
				userInput = 24;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > 0)
			{
				userInput -= 1;
			}
			else
			{
				userInput = 0;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void masterSlaveModePosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = GROUP_CONTROL_MODE_POSITION;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		switch(currentPosition.lineNumber)
		{
		case 1:
			currentPosition.displayLevel = UNIT_IDENTIFICATION_POSITION;
			userInput = modbus_rw_reg_rcv[UNIT_ID].ivalue;
			break;

		case 2:
			currentPosition.displayLevel = NUMBER_OF_UNITS_POSITION;
			userInput = modbus_rw_reg_rcv[GROUP_CONTROL_SIZE].ivalue;
			break;
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = INLET_LINENUM;
	}
	else if(releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
}

void numberOfUnitsPosition(void)
{
	if(releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = MASTER_SLAVE_MODE_POSITION;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
	else if(releasedOK)
	{
		releasedOK = false;
		if((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{
			uint16 uart_write_return;
			uart_write_return = display_uart_update(REGISTER, RW_REG_START + GROUP_CONTROL_SIZE, false, userInput, GROUP_CONTROL_SIZE_F, 0);
			if((!uart_write_return))
			{
				// Validation Screen
				parameterIsSet = true;
				// A delay for UART to write the value to the ECU
				TI1_UARTWriteTimer_Flag = true;
			}
		}
	}
	else if(releasedUp)
	{
		releasedUp = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput < 15)
			{
				userInput += 1;
			}
			else
			{
				userInput = 15;
			}
		}
	}
	else if(releasedDown)
	{
		releasedDown = false;
		if ((!isInDegradedMode)&&(modbus_rw_reg_rcv[UNIT_ID].ivalue == 1))
		{

			if (userInput > 1)
			{
				userInput -= 1;
			}
			else
			{
				userInput = 1;
			}
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < 15)
			{
				userInput += 1;
			}
			else
			{
				userInput = 15;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > 1)
			{
				userInput -= 1;
			}
			else
			{
				userInput = 1;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void doorSwitchPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 2;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		switch (currentPosition.lineNumber)
		{
		case 1: // Normally open
			uart_write_return = display_uart_update(COIL, RW_COIL_START + DOOR_SWITCH, false, 0, 0, DOOR_SWITCH_F);
			break;

		case 2: // Normally close
			uart_write_return = display_uart_update(COIL, RW_COIL_START + DOOR_SWITCH, true, 0, 0, DOOR_SWITCH_F);
			break;

		default: break;
		}
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
		//updateWrtRd = 5;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = INLET_LINENUM;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = OUTLET_LINENUM;
	}
}

void disableSwitchAlarmPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 3;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		switch (currentPosition.lineNumber)
		{
		case 1: // Normally open
			uart_write_return = display_uart_update(COIL, RW_COIL_START + DISABLE_SWITCH, false, 0, 0, DISABLE_SWITCH_F);
			break;

		case 2: // Normally close
			uart_write_return = display_uart_update(COIL, RW_COIL_START + DISABLE_SWITCH, true, 0, 0, DISABLE_SWITCH_F);
			break;

		default: break;
		}
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
		//updateWrtRd = 6;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = 1;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = 2;
	}
}

void alarmRelayOutputPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 4;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		switch (currentPosition.lineNumber)
		{
		case 1: // Normally open
			uart_write_return = display_uart_update(COIL, RW_COIL_START + ALARM_OUTPUT, false, 0, 0, ALARM_OUTPUT_F);
			break;

		case 2: // Normally close
			uart_write_return = display_uart_update(COIL, RW_COIL_START + ALARM_OUTPUT, true, 0, 0, ALARM_OUTPUT_F);
			break;

		default: break;
		}
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
		//updateWrtRd = 7;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		currentPosition.lineNumber = 1;
	}
	else if (releasedDown)
	{
		releasedDown = false;
		currentPosition.lineNumber = 2;
	}
}

void compressorRestartDelayPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber = 5;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		uint16 uart_write_return;
		// write to main board to update the coolingSP
		uart_write_return = display_uart_update(REGISTER, RW_REG_START + COMP_DELAY_TIME, false, userInput, COMP_DELAY_TIME_F, 0);
		// Validation Screen
		parameterIsSet = true;
		// A delay for UART to write the value to the ECU
		TI1_UARTWriteTimer_Flag = true;
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (userInput < modbus_rw_reg_rcv[COMP_DELAY_MAX].ivalue)
		{
			userInput += 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (userInput > modbus_rw_reg_rcv[COMP_DELAY_MIN].ivalue)
		{
			userInput -= 1;
		}
	}
	else if (heldUp)
	{
		if (updateScreenTimerDone)
		{
			if (userInput < modbus_rw_reg_rcv[COMP_DELAY_MAX].ivalue)
			{
				userInput += 1;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
	else if (heldDown)
	{
		if (updateScreenTimerDone)
		{
			if (userInput > modbus_rw_reg_rcv[COMP_DELAY_MIN].ivalue)
			{
				userInput -= 1;
			}
			updateScreenTimerDone = false;
		}
		else
		{
			if (!TI1_updateScreenTimerIsOn)
			{
				updateScreenRate = BUTTON_HELD_REFRESH_TIME;
				TI1_updateScreenTimerIsOn = true;
			}
		}
	}
}

void compressorCurrentPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 6;
	}
	else if (releasedOK)
	{
		releasedOK = false;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void evapOutTempSensorPosition(void)
{
	if (releasedBack)
		{
			releasedBack = false;
			currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
			currentPosition.lineNumber   = 7;
		}
		else if (releasedOK)
		{
			releasedOK = false;
			uint16 uart_write_return;
			switch (currentPosition.lineNumber)
			{
				case 1: // Probe 2 present
					uart_write_return = display_uart_update(COIL, RW_COIL_START + PROBE2_PRESENT, true, 0, 0, PROBE2_PRESENT_F);
					break;
				case 2: // Probe 2 absent
					uart_write_return = display_uart_update(COIL, RW_COIL_START + PROBE2_PRESENT, false, 0, 0, PROBE2_PRESENT_F);
					break;
				default: break;
			}
			// Validation Screen
			parameterIsSet = true;
			// A delay for UART to write the value to the ECU
			TI1_UARTWriteTimer_Flag = true;
			//updateWrtRd = 5;
		}
		else if (releasedUp)
		{
			releasedUp = false;
			currentPosition.lineNumber = 1;
		}
		else if (releasedDown)
		{
			releasedDown = false;
			currentPosition.lineNumber = 2;
		}
}

void systemSettingsPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = SYSTEM_INTERFACE_POSITION;
		currentPosition.lineNumber   = 8;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		refreshScreen = true;
		switch (currentPosition.lineNumber)
		{
		case 1:
			currentPosition.displayLevel = CURRENT_LIMIT_SETTINGS_POSITION;
			break;

		case 2:
			currentPosition.displayLevel = MALF_ACTIVATE_POSITION;
			break;

		case 3:
			currentPosition.displayLevel = MALF_DEACTIVATE_POSITION;
			break;

		case 4:
			currentPosition.displayLevel = HPC_FAN_ON_POSITION;
			break;

		case 5:
			currentPosition.displayLevel = HPC_FAN_OFF_POSITION;
			break;

		default: break;
		}
	}
	else if (releasedUp)
	{
		releasedUp = false;
		if (currentPosition.lineNumber > 1)
		{
			currentPosition.lineNumber -= 1;
		}
	}
	else if (releasedDown)
	{
		releasedDown = false;
		if (currentPosition.lineNumber < 5)
		{
			currentPosition.lineNumber += 1;
		}
	}
}

void currentLimitSettingsPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 1;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 1;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void malfDeactivatePosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 3;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 3;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void malfActivatePosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 2;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 2;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void hpcFanOnPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 4;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 4;
	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}
}

void hpcFanOffPosition(void)
{
	if (releasedBack)
	{
		releasedBack = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 5;
	}
	else if (releasedOK)
	{
		releasedOK = false;
		currentPosition.displayLevel = DISPLAY_SYS_SETTINGS_MENU_POSITION;
		currentPosition.lineNumber   = 5;

	}
	else if (releasedUp)
	{
		releasedUp = false;
	}
	else if (releasedDown)
	{
		releasedDown = false;
	}

}
