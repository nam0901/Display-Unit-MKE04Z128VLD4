/*
 * textTable.h
 *
 *  Created on: Apr 10, 2017
 *      Author: E1218968
 */

#ifndef SOURCES_TEXTTABLE_H_
#define SOURCES_TEXTTABLE_H_

#define TEXT_INDEX_HighTempAlarm                                  0
#define TEXT_INDEX_LowTempAlarm                                   1
#define TEXT_INDEX_DoorSmokeAlarm                                 2
#define TEXT_INDEX_PhaseMissing                                   3
#define TEXT_INDEX_HighTempShutdown                               4
#define TEXT_INDEX_CompressorThermalOverload                      5
#define TEXT_INDEX_OverVoltage                                    6
#define TEXT_INDEX_UnderVoltage                                   7
#define TEXT_INDEX_ControllerBoardMalfunction                     8
#define TEXT_INDEX_UnitMalfunctionHighPressure                    9
#define TEXT_INDEX_FrostAlarm                                     10
#define TEXT_INDEX_CondenserCoilTempSensorMalfunction             11
#define TEXT_INDEX_EnclosureInletAirTempSensorMalfunction         12
#define TEXT_INDEX_EnclosureOutletAirTempSensorMalfunction        13
#define TEXT_INDEX_CompressorOvercurrentAlarm                     14
#define TEXT_INDEX_DisableSwitchActivated                         15
#define TEXT_INDEX_RestartDelayActive                             16
#define TEXT_INDEX_DisplayError                                   17
#define TEXT_INDEX_DegradedMode                                   18
#define TEXT_INDEX_GroupContError								  19
#define TEXT_INDEX_EnclosureOutletTemp                            20
#define TEXT_INDEX_CondenserCoilTemp                              21
#define TEXT_INDEX_MainMenu                                       22
#define TEXT_INDEX_EnterPasscode                                  23
#define TEXT_INDEX_UserInterface                                  24
#define TEXT_INDEX_CoolingSetPoint                                25
#define TEXT_INDEX_CoolingDifferential                            26
#define TEXT_INDEX_HeatingSetPoint                                27
#define TEXT_INDEX_HeatingDifferential                            28
#define TEXT_INDEX_TemperatureScale                               29
#define TEXT_INDEX_Hysteresis                                     30
#define TEXT_INDEX_Language                                       31
#define TEXT_INDEX_Passcode                                       32
#define TEXT_INDEX_Fahrenheit                                     33
#define TEXT_INDEX_Celsius                                        34
#define TEXT_INDEX_Positive                                       35
#define TEXT_INDEX_Negative                                       36
#define TEXT_INDEX_English                                        37
#define TEXT_INDEX_French                                         38
#define TEXT_INDEX_Deutsche                                       39
#define TEXT_INDEX_italiano                                       40
#define TEXT_INDEX_Polskie                                        41
#define TEXT_INDEX_Spanish                                        42
#define TEXT_INDEX_OFF                                            43
#define TEXT_INDEX_ON                                             44
#define TEXT_INDEX_SystemInterface                                45
#define TEXT_INDEX_DoorSmokeAlarmSetting                          46
#define TEXT_INDEX_DisableSwitch                                  47
#define TEXT_INDEX_AlarmRelayOutput                               48
#define TEXT_INDEX_CompRestartDelay                               49
#define TEXT_INDEX_CompressorCurrent                              50
#define TEXT_INDEX_SystemSettings                                 51
#define TEXT_INDEX_CurrentLimitSetting                            52
#define TEXT_INDEX_MALFActivate                                   53
#define TEXT_INDEX_MALFDeactivate                                 54
#define TEXT_INDEX_HPCFanOn                                       55
#define TEXT_INDEX_HPCFanOff                                      56
#define TEXT_INDEX_NormallyOpen                                   57
#define TEXT_INDEX_NormallyClose                                  58
#define TEXT_INDEX_Phase1                                         59
#define TEXT_INDEX_Phase2                                         60
#define TEXT_INDEX_Phase3                                         61
#define TEXT_INDEX_About                                          62
#define TEXT_INDEX_ControlHardware                                63
#define TEXT_INDEX_ControlFirmware                                64
#define TEXT_INDEX_DisplayHardware                                65
#define TEXT_INDEX_DisplayFirmware                                66
#define TEXT_INDEX_IsSetTo										  67
#define TEXT_INDEX_PasscodeForUserInterfaceIsOn					  68
#define TEXT_INDEX_PasscodeForUserInterfaceIsOff				  69
#define TEXT_INDEX_DoorSwitchIsSetToNO							  70
#define TEXT_INDEX_DoorSwitchIsSetToNC							  71
#define TEXT_INDEX_DisableSwitchIsSetToNO						  72
#define TEXT_INDEX_DisableSwitchIsSetToNC						  73
#define TEXT_INDEX_AlarmRelayOutputIsSetToNO					  74
#define TEXT_INDEX_AlarmRelayOutputIsSetToNC					  75
#define TEXT_INDEX_CompRestartDelayIsSetTo						  76
#define TEXT_INDEX_Delay										  77
#define TEXT_INDEX_CoolingSetPointIsSetTo						  78
#define TEXT_INDEX_CoolingDifferentialIsSetTo					  79
#define TEXT_INDEX_HeatingSetPointIsSetTo						  80
#define TEXT_INDEX_HeatingDifferentialIsSetTo					  81
#define TEXT_INDEX_HighTempAlarmIsSetTo							  82
#define TEXT_INDEX_LowTempAlarmIsSetTo						  	  83
#define TEXT_INDEX_TempScaleIsSetTo								  84
#define TEXT_INDEX_DifferentialIsSetTo							  85
#define TEXT_INDEX_UpdatingLanguageSettings						  86
#define TEXT_INDEX_Bootload										  87
#define TEXT_INDEX_EvapOutTempSensor							  88
#define TEXT_INDEX_EvapOutTempSensorIsSetToPresent				  89
#define TEXT_INDEX_EvapOutTempSensorIsSetToAbsent				  90
#define TEXT_INDEX_Present										  91
#define TEXT_INDEX_NotPresent									  92
#define TEXT_INDEX_GroupControlMode								  93
#define TEXT_INDEX_Mode											  94
#define TEXT_INDEX_ModeSettings									  95
#define TEXT_INDEX_StandAlone									  96
#define TEXT_INDEX_LeadLag										  97
#define TEXT_INDEX_MasterSlave									  98
#define TEXT_INDEX_StandAloneMode								  99
#define TEXT_INDEX_NoSettings									  100
#define TEXT_INDEX_LeadLagMode									  101
#define TEXT_INDEX_UnitIdentification							  102
#define TEXT_INDEX_DifferentialTwo								  103
#define TEXT_INDEX_ControlStrategy								  104
#define TEXT_INDEX_MastrSlaveMode								  105
#define TEXT_INDEX_NumberOfUnits								  106
#define TEXT_INDEX_GCMIsSetToSA									  107
#define TEXT_INDEX_GCMIsSetToLL									  108
#define TEXT_INDEX_GCMIsSetToMS									  109

extern const unsigned char *englishText[];
extern const unsigned char *frenchText[];
extern const unsigned char *germanText[];
extern const unsigned char *italianText[];
extern const unsigned char *polishText[];
extern const unsigned char *spanishText[];

#endif /* SOURCES_TEXTTABLE_H_ */
