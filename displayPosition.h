#ifndef __DISPLAY_POSITION_H
#define __DISPLAY_POSITION_H

/********************* Positions ***********************/
#define COMPANY_ICON_POSITION					-1
#define ICON_ALL_ON_POSITION					-2
#define MAIN_SCREEN_POSITION					0
#define MAIN_MENU_POSITION						1
#define LANGUAGE_MENU_POSITION					2
#define USER_INTERFACE_POSITION					3
#define SYSTEM_INTERFACE_POSITION				4
#define USER_PASSWORD_POSITION					5
#define SYSTEM_PASSWORD_POSITION				6
/*************** User Interface *****************/
#define COOLING_SET_POINT_POSITION  			7
#define COOLING_DIF_POSITION					8
#define HEATING_SET_POINT_POSITION  			9
#define HEATING_DIF_POSITION					10
#define TEMP_SCALE_POSITION						11
#define HYSTERESIS_POSITION						12
#define PASSWORD_POSITION						13
/*************** System Interface ****************/
#define GROUP_CONTROL_MODE_POSITION				30
#define MODE_POSITION							31
#define STAND_ALONE_MODE_POSITION				32
#define LEAD_LAG_MODE_POSITION					33
#define MASTER_SLAVE_MODE_POSITION				34
#define UNIT_IDENTIFICATION_POSITION			35
#define DIFFERENTIAL_2_POSITION					36
#define CONTROL_STRATEGY_POSITION				37
#define NUMBER_OF_UNITS_POSITION				38
#define DOOR_SWITCH_POSITION   					14
#define DISABLE_SWITCH_POSITION					15
#define ALARM_RELAY_OUTPUT_POSITION				16
#define ABOUT_POSITION							17
#define COMPRESSOR_RESTART_DELAY_POSITION    	18
#define HIGH_TEMP_ALARM_SP_POSITION 			19
#define LOW_TEMP_ALARM_SP_POSITION  			20
#define COMPRESSOR_CURRENT_POSITION   			21
#define DISPLAY_SYS_SETTINGS_MENU_POSITION   	22
#define EVAP_OUT_TEMP_SENSOR_POSITION			23

/************ Display System Settings ***************/
#define CURRENT_SETPOINT_INDEX					9
#define CURRENT_LIMIT_SETTINGS_POSITION			24
#define MALF_ACTIVATE_POSITION					25
#define MALF_DEACTIVATE_POSITION				26
#define HPC_FAN_ON_POSITION						27
#define HPC_FAN_OFF_POSITION					28
#define THREE_PHASE_CURRENT_MENU_POSITION       29

//#define VERSION_NUM_POSITION					30

#define EVAP_MI_TEST				1
#define COND_MI_TEST				2
#define COMP_MI_TEST				3
#define HEATER_MI_TEST				4

/******************** Alarm Display **********************/
#define HIGH_TEMP_ALARM_POSITION						50
#define LOW_TEMP_ALARM_POSITION							51
#define DOOR_ALARM_POSITION								52
#define PHASE_MISSING_ALARM_POSITION					53
#define HIGH_TEMP_SHUTDOWN_POSITION						54
#define COMPRESSOR_THERMAL_OVERLOAD_ALARM_POSITION		55
#define OVER_VOLTAGE_ALARM_POSITION						56
#define UNDER_VOLTAGE_ALARM_POSITION					57
#define BAD_BOARD_ALARM_POSITION						58
#define HIGH_PRESSURE_ALARM_POSITION					59
#define FROST_ALARM_POSITION							60
#define COIL_TEMP_SENSOR_FAULT_ALARM_POSITION			61
#define INLET_TEMP_SENSOR_FAULT_ALARM_POSITION			62
#define OUTLET_TEMP_SENSOR_FAULT_ALARM_POSITION			63
#define OVER_CURRENT_ALARM_POSITION						64
#define DISABLE_SWITCH_ALARM_POSITION					65
#define COMPRESSOR_RESTART_DELAY_COUNT_DOWN_POSITION	66
#define LOST_COMMUNICATION_POSITION						67
#define DEGRADED_MODE_POSITION							68
#define GROUP_CONTROL_ALARM_POSITION					69

#define CHECK_ALL_PIXELS								70

/**************** Alarm Output ******************/
#define HIGH_TEMP							0
#define LOW_TEMP							1
#define DOOR_SMOKE							2
#define PHASE_MISSING						3
#define HIGH_TEMP_SHUTDOWN					4
#define COMPRESSOR_THERMAL_OVERLOAD			5
#define OVER_VOLTAGE						6
#define UNDER_VOLTAGE						7
#define BAD_BOARD							8
#define HIGH_PRESSURE						9
#define FROST								10
#define COIL_TEMP_SENSOR_FAULT				11
#define INLET_TEMP_SENSOR_FAULT				12
#define OUTLET_TEMP_SENSOR_FAULT			13
#define OVER_CURRENT						14
#define DISABLE_SWITCH_ALARM				15
#define COMPRESSOR_COUNT_DOWN				16
#define LOST_COMMUNICATION					17
#define DEGRADED_MODE						18
#define GROUP_CONTROL						19

#define NUMBER_OF_ALARMS					20

/******************** Line Numbers **************************/
#define INLET_LINENUM	1
#define OUTLET_LINENUM	2
#define COIL_LINENUM	3

/*
 * The struct Location can hold two variables. In the display
 * application, the location is used to track the position of
 * the display (currentPosition). displayLevel corresponds to
 * different screens, such as MAIN_MENU_POSITION. While lineNumber
 * corresponds to different lines in the menu, for example, if
 * currentPosition.displayLevel = MAIN_MENU_POSITION and
 * currentPosition.lineNumber = 1, then the screen is showing
 * the main menu, select on the first line of the main menu,
 * which is user interface.
 */
struct Location {
	short displayLevel;
	short lineNumber;
};

extern struct Location currentPosition;
extern int alarmOutput[];
extern int alarmOutputIndex;
extern int alarmStartPosition;

extern _Bool updateDisplayPosition;
extern _Bool lostCommunication;

_Bool notHeaterDisplay; //variable used to help the display of heating setpoint and heating differential when heater present or not

extern _Bool isInDegradedMode;

_Bool justClickOK; //use in About menut item

//extern int coolingSPBeforeEnterDegradedModeInDegreeF;
//extern int coolingSPBeforeEnterDegradedModeInDegreeC;

void checkDisplayPosition(void);

#endif /* __DISPLAY_POSITION_H */
