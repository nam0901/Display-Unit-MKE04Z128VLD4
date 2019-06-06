/*
 * display_UART.h
 *
 *  Created on: Nov 16, 2016
 *      Author: e1125980
 */

#ifndef SOURCES_DISPLAY_UART_H_
#define SOURCES_DISPLAY_UART_H_

#include "PE_Types.h"

//since the reading of UART communication is randomly, must have varibale to tell that we need to update the reading
//of Temp Scale, Differential +/-, language, passcode, door switch, alarm relay output
//Problem: choose differential +/- to negative but confirmation message say positive.
// 1. Temp Scale, 2.Differential +/-, 3.language, 4.passcode, 5.door switch, 6.Disable Switch  7.alarm relay output
//static char updateWrtRd;


// Read/Write Parameter Registers
#define RW_REG_BOOTLOAD			605
#define RW_REG_BOOTLOAD_INDEX 	0

#define RW_REG_START			1
#define COOLING_SP_MIN			0
#define COOLING_SP_MIN_F		0x00000001
#define COOLING_DIF_2			1
#define COOLING_DIF_2_F			0x00000002
#define COOLING_SP				2
#define COOLING_SP_F			0x00000004
#define COOLING_DIF_1			3
#define COOLING_DIF_1_F			0x00000008
#define HIGH_TEMP_ALARM_SP		4
#define HIGH_TEMP_ALARM_SP_F	0x00000010
#define HIGH_TEMP_ALARM_DIF		5
#define HIGH_TEMP_ALARM_DIF_F	0x00000020
#define LOW_TEMP_ALARM_SP		6
#define LOW_TEMP_ALARM_SP_F		0x00000040
#define LOW_TEMP_ALARM_DIF		7
#define LOW_TEMP_ALARM_DIF_F	0x00000080
#define FROST_ALARM_SP			8
#define FROST_ALARM_SP_F		0x00000100
#define FROST_ALARM_DIF			9
#define FROST_ALARM_DIF_F		0x00000200
#define HEATING_SP_MIN			10
#define HEATING_SP_MIN_F		0x00000400
#define HEATING_SP_MAX			11
#define HEATING_SP_MAX_F		0x00000800
#define HEATING_SP				12
#define HEATING_SP_F			0x00001000
#define HEATING_DIF				13
#define HEATING_DIF_F			0x00002000
#define COMP_DELAY_TIME			14
#define COMP_DELAY_TIME_F		0x00004000
#define COMP_DELAY_MIN			15
#define COMP_DELAY_MIN_F 		0x00008000
#define COMP_DELAY_MAX			16
#define COMP_DELAY_MAX_F 		0x00010000
#define COOLING_DIF_2_MIN		17
#define COOLING_DIF_2_MIN_F 	0x00020000
#define COOLING_DIF_2_MAX		18
#define COOLING_DIF_2_MAX_F 	0x00040000
#define HIGH_TEMP_SP_MIN		19
#define HIGH_TEMP_SP_MIN_F 		0x00080000
#define HIGH_TEMP_SP_MAX		20
#define HIGH_TEMP_SP_MAX_F		0x00100000
#define LOW_TEMP_SP_MIN			21
#define LOW_TEMP_SP_MIN_F 		0x00200000
#define LOW_TEMP_SP_MAX			22
#define LOW_TEMP_SP_MAX_F 		0x00400000
#define HEATING_DIF_MIN			23
#define HEATING_DIF_MIN_F		0x00800000

#define HEATING_DIF_MAX			24			//Discovered error issues when DU App Rev B asks for registers from ECU Rev I (11/16/18)
#define HEATING_DIF_MAX_F		0x01000000
#define UNIT_ID					25
#define UNIT_ID_F				0x02000000
#define GROUP_CONTROL_MODE		26
#define GROUP_CONTROL_MODE_F	0x04000000
#define GROUP_CONTROL_SIZE		27
#define GROUP_CONTROL_SIZE_F	0x08000000
#define CONTROL_STRATEGY		28
#define CONTROL_STRATEGY_F		0x10000000
#define RW_REG_END				29
#define RW_REG_LEN				29

// Read Only Parameter Settings
#define RO_REG_START			201
#define CONTROL_GROUP			0
#define CONTROL_GROUP_F			0x00000001
#define CONTROL_TYPE			1
#define CONTROL_TYPE_F			0x00000002
#define FIRMWARE_VERSION		2
#define FIRMWARE_VERSION_F		0x00000004
#define HARDWARE_VERSION		3
#define HARDWARE_VERSION_F		0x00000008
#define PROBE_1					4
#define PROBE_1_F				0x00000010
#define PROBE_2					5
#define PROBE_2_F				0x00000020
#define PROBE_3					6
#define PROBE_3_F				0x00000040
#define PROBE_4					7
#define PROBE_4_F				0x00000080
#define WORKING_STATUS_1		8
#define WORKING_STATUS_1_F		0x00000100
#define WORKING_STATUS_2		9
#define WORKING_STATUS_2_F		0x00000200
#define DIGITAL_IO				10
#define DIGITAL_IO_F			0x00000400
#define FREQUENCY				11
#define FREQUENCY_F				0x00000800
#define DC_CURRENT				12
#define DC_CURRENT_F			0x00001000
#define PHASE_1_CURRENT			13
#define PHASE_1_CURRENT_F		0x00002000
#define PHASE_2_CURRENT			14
#define PHASE_2_CURRENT_F		0x00004000
#define PHASE_3_CURRENT			15
#define PHASE_3_CURRENT_F		0x00008000
#define LINE_1_2_VOLTAGE		16
#define LINE_1_2_VOLTAGE_F		0x00010000
#define LINE_2_3_VOLTAGE		17
#define LINE_2_3_VOLTAGE_F		0x00020000
#define LINE_3_1_VOLTAGE		18
#define LINE_3_1_VOLTAGE_F		0x00040000
#define COMP_DELAY_TIMER		19
#define COMP_DELAY_TIMER_F		0x00100000
#define RO_REG_END				220
#define RO_REG_LEN				31

#define RO_COOL_SP_MIN			237  //added recently due to change by supervisor. Victor calculate these value in controller code
#define COOL_SP_MIN				20
#define COOL_SP_MAX				21
#define COOL_DIF_1_MIN			22
#define COOL_DIF_1_MAX			23
#define GROUP_CONT_SIZE_MIN		24
#define GROUP_CONT_SIZE_MAX		25
#define GROUP_CONT_MODE_STATUS	26
#define COOL_ON_POINT_2			27
#define RO_COOL_ON_POINT_2		244

#define RO_GROUP_CONT_STATUS 	402
#define GROUP_CONT_STATUS		28

#define RW1_ID1_WORK_STATUS		407
#define ID1_WORK_STATUS			29
#define RW1_ID2_WORK_STATUS		412
#define ID2_WORK_STATUS			30

// Read/Write Coils
#define RW_COIL_START			1
#define UNIT_OF_MEASURE			0
#define UNIT_OF_MEASURE_F		0x01
#define COOL_HYSTERESIS			1
#define COOL_HYSTERESIS_F		0x02
#define DOOR_SWITCH				2
#define DOOR_SWITCH_F			0x04
#define DISABLE_SWITCH			3
#define DISABLE_SWITCH_F		0x08
#define ALARM_OUTPUT			4
#define ALARM_OUTPUT_F			0x10
#define PROBE2_PRESENT			5
#define PROBE2_PRESENT_F		0x20
#define PASSWORD_ENABLED		6
#define PASSWORD_ENABLED_F		0x40
#define HEATER_PRESENT			7
#define HEATER_PRESENT_F		0x80
#define COND_FAN2_PRESENT		8
#define COND_FAN2_PRESENT_F		0x01
#define RW_COIL_END				9
#define RW_COIL_LEN				9

// Read/Write Extended Parameter Registers
#define WO_REG_START			491
#define DISPLAY_FIRMWARE_REV	0
#define DISPLAY_FIRMWARE_REV_F	0x00000001
#define DISPLAY_HARDWARE_REV	1
#define DISPLAY_HARDWARE_REV_F	0x00000002
#define WO_REG_END				492
#define WO_REG_LEN				2

#define ERO_REG_START			501
#define HPC_ACTIVATE			0
#define HPC_ACTIVATE_F			0x00000001
#define HPC_DEACTIVATE			1
#define HPC_DEACTIVATE_F		0x00000002
#define MALF_ACTIVATE			2
#define MALF_ACTIVATE_F			0x00000004
#define MALF_DEACTIVATE			3
#define MALF_DEACTIVATE_F		0x00000008
#define HT_SHUTDOWN_ACT         4
#define HT_SHUTDOWN_ACT_F       0x00000010
#define HT_SHUTDOWN_DEACT       5
#define HT_SHUTDOWN_DEACT_F     0x00000020
#define COOL_ON_MAX				6
#define COOL_ON_MAX_F			0x00000040
#define COOL_OFF_MIN			7
#define COOL_OFF_MIN_F			0x00000080
#define DISPLAY_LANGUAGE	    8
#define DISPLAY_LANGUAGE_F  	0x00000100
#define ERO_REG_LEN				9
#define ERO_REG_END				509

#define CURRENT_SETPOINT		98
#define CURRENT_SETPOINT_F		0x00000010

#define TEST_DIGITAL_IO			601
#define TEST_DIGITAL_IO_F		0x00000080
#define VOLTAGE_SP_MIN			602
#define VOLTAGE_SP_MIN_F		0x00000100
#define VOLTAGE_SP_MAX			603
#define VOLTAGE_SP_MAX_F		0x00000200

// Extended Read/Write Coils
#define ERW_COIL_START			201
#define DIAGNOSTICS_ACTIVE		0
#define DIAGNOSTICS_ACTIVE_F	0x01
#define COMPRESSOR_ACTIVE		1
#define COMPRESSOR_ACTIVE_F		0x02
#define COND_FAN_MI_ACTIVE		2
#define COND_FAN_MI_ACTIVE_F	0x04
#define EVAP_FAN_MI_ACTIVE		3
#define EVAP_FAN_MI_ACTIVE_F	0x08
#define RELAYS_OFF				4
#define RELAYS_OFF_F			0x10
#define FORWARD_RELAY_ACT		5
#define FORWARD_RELAY_ACT_F		0x20
#define REVERSE_RELAY_ACT		6
#define REVERSE_RELAY_ACT_F		0x40
#define COND_FAN2_MI_ACTIVE		7
#define COND_FAN2_MI_ACTIVE_F	0x80
#define HEATER_ACTIVE			8
#define HEATER_ACTIVE_F			0x01
#define ERW_COIL_END			209
#define ERW_COIL_LEN			9

// Other Defines
#define COIL					0
#define REGISTER				1
#define COIL_TRUE				0xFF00
#define COIL_FALSE				0x0000


typedef union
{
	struct {
		uint16	bit0	:1;
		uint16	bit1	:1;
		uint16	bit2	:1;
		uint16	bit3	:1;
		uint16	bit4	:1;
		uint16	bit5	:1;
		uint16	bit6	:1;
		uint16	bit7	:1;
		uint16	bit8	:1;
		uint16	bit9	:1;
		uint16	bit10	:1;
		uint16	bit11	:1;
		uint16	bit12	:1;
		uint16	bit13	:1;
		uint16	bit14	:1;
		uint16	bit15	:1;
	} bits;
	struct {
		uint8	byte0	:8;
		uint8	byte1	:8;
	} bytes;

	uint8 	array[2];
	int16	ivalue;
	uint16 	uvalue;
} uint16_union, int16_union;


#define RX_MAX_WAIT				100							// number of mseconds to wait for RX packet to complete
#define TX_MAX_WAIT				3000						// number of mseconds to wait for multi-packet TX  to complete
#define TX_DELAY				1							// number of seconds to wait between transmission
#define DISP_RX_BUF_LEN			30							// Creates a RX buffer of given length
#define DISP_TX_BUF_LEN			10							// Creates a TX buffer of give length
LDD_TDeviceData *displayUART;
byte 	dispRXBuffer[DISP_RX_BUF_LEN];
byte 	dispTXBuffer[DISP_TX_BUF_LEN];
uint16 	rx_timeout;
uint16 	tx_timeout;
uint8  	ti1_seconds;
uint16 	sec_1s_cnt;
//extern uint8 rx_cnt;										// increments when packet is complete

extern _Bool goToBootload;									// use for user application to go to bootload mode

extern uint16_union modbus_rw_reg_rcv[RW_REG_LEN];			// read/write registers, modbus 1-23
extern uint16_union modbus_rw_reg_snd[RW_REG_LEN];			// read/write registers, modbus 1-23
extern uint16_union modbus_ro_reg_rcv[RO_REG_LEN];			// read only registers, modbus 201-220 237-240
extern uint16_union modbus_ero_reg_rcv[ERO_REG_LEN+1];		// extended read only registers, modbus 501-509 599
extern uint16_union modbus_ero_reg_snd[ERO_REG_LEN+1];		// extended read only registers, modbus 501-509 599
extern uint16_union modbus_wo_reg_snd[WO_REG_LEN];			// write only display firmware and hardware revision registers, modbus 491-492
extern byte modbus_rw_coil_rcv[RW_COIL_LEN/8+1];			// read/write coils, modbus 1-9
extern byte modbus_rw_coil_snd[RW_COIL_LEN/8+1];			// read/write coils, modbus 1-9
extern byte modbus_erw_coil_rcv[ERW_COIL_LEN/8+1];			// extended read/write coils, modbus 201-209
extern byte modbus_erw_coil_snd[ERW_COIL_LEN/8+1];			// extended read/write coils, modbus 201-209
uint32 write_reg_flags;										// 32-bits of write register flags for update routine
uint32 write_ext_reg_flags;									// 32-bits of write register flags for update routine
uint32 write_rev_reg_flags;									// 32-bits of write register flags for update routine
uint32 write_ero_reg_flags;									// 32-bits of extended read only register flags for update routine
uint8 static write_coil_flags[RW_COIL_LEN/8+1];				// 8-bit array of write coil flags for update routine
uint8 static write_ext_coil_flags[ERW_COIL_LEN/8+1];		// 8-bit array of write extended coil flags for update routine

//int readModbusRegister(int registerNum);
//int readModbusCoil(int coilNum);

//*****************************************************************************
//
//	Function Name	: display_uart_init
//	Returned Value	: None
//
//			Initializes uart by getting it's device handle. The device handle
//			is used in all subsequent uart routines. The receive buffer is also
//			initialized to 0.
//
//*****************************************************************************
void display_uart_init();


//*****************************************************************************
//
//	Function Name	: rx_error_packet
//	Returned Value	: Unsigned 16-bit integer, 0=No error packet detected
//                    >0=error packet detected or invalid exit of routine.
//
//			This routine examines the incoming modbus rx data for error packets
//			while waiting to receive packets larger than 5 bytes. This will not
//			work on smaller rx packets.
//
//*****************************************************************************
uint16 rx_error_packet();


//*****************************************************************************
//
//	Function Name	: rx_invalid_packet_header
//	Returned Value	: Unsigned 16-bit integer, 0=No error detected in packet
//                    header, >0=header error detected or invalid exit of
//                    routine.
//
//			This routine examines the incoming modbus rx data for packet header
//			errors while waiting to receive packets larger than 2 bytes. This
//			will not work on smaller rx packets. This will help detect packets
//			with shifted data.
//
//*****************************************************************************
uint16 rx_invalid_packet_header();

//void addition_coil_read();

//*****************************************************************************
//
//	Function Name	: display_uart_run
//	Returned Value	: Boolean value indicating that all modbus registers
//                    and coils have been read. 1=Complete, 0=Not complete.
//
//			This routine is a state machine that handles all reading and
//			writing of all ModBUS coil and register addresses.
//
//*****************************************************************************
LDD_TError display_uart_run();


//*****************************************************************************
//
//	Function Name	: display_uart_update
//	Returned Value	: Unsigned 16-bit integer indicating that the appropriate
//                    update flags has been set and the value to be sent has
//                    been loaded. 0=success
//
//*****************************************************************************
uint16 display_uart_update(
		bool	type,					// update type, coil=0 or register=1
		uint16	address,				// ModBUS address to write
		uint8	coil_state,				// true false state of coil to update
		int16	reg_value,				// register value to write to register
		uint32	reg_flags,				// register flags to update
		uint8	coil_flags);			// coil flags to update


//*****************************************************************************
//
//	Function Name	: display_reg_read
//	Returned Value	: 16-bit Error code. 0=no error/success.
//
//			This function is designed to read ModBUS words starting at 'address'
//			and reading 'length' number or words.
//			This function will create the entire packet required to
//			complete and execute the read and receive the response back from
//			the main control board.  UART errors can be returned or 0 if
//			comm. is successful.
//
//*****************************************************************************
uint16 display_reg_read (
		uint16 address,					// ModBUS address to read
		uint16 length);					// Length of the data to read


//*****************************************************************************
//
//	Function Name	: display_reg_single_write
//	Returned Value	: 16-bit Error code, 0 = successful
//
//			This function is designed to write ModBUS address one at a time.
//			This function will create the entire packet required to complete
//			and execute the write and receive the response back from the main
//			control board.
//
//*****************************************************************************
uint16 display_reg_single_write (
		uint16 	address,				// ModBUS address to write
		uint16	value);					// Value to write to register


//*****************************************************************************
//
//	Function Name	: display_coil_read
//	Returned Value	: 16-bit Error code, 0 = successful
//
//			This function is designed to read ModBUS coils one at a time. This
//			function will create the entire packet required to complete and
//			execute the read and receive the response back from	the main
//			control board.
//
//*****************************************************************************
uint16 display_coil_read (
		uint16 address,					// ModBUS address to read
		uint16 length);					// Length of the data to read


//*****************************************************************************
//
//	Function Name	: display_single_coil_write
//	Returned Value	: 16-bit Error code, 0 = successful
//
//			This function is designed to write ModBUS coils one at a time. This
//			function will create the entire packet required to complete and
//			execute the write and receive the response back from the main
//			control board.
//
//*****************************************************************************
uint16 display_coil_single_write (
		uint16 	address,				// ModBUS coil address to write
		uint16	value);					// Value to write to coil


//*****************************************************************************
//
//	Function Name	: display_single_coil_write
//	Returned Value	: 16-bit Error code, 0 = successful
//
//			This routine loads the PE routines that load the uart structure and
//			allow interrupt driven uart operation. If the RX times out the uart
//			structure is reset so transmit can occur again.
//
//*****************************************************************************
uint16 display_uart_comm(bool read_only);


//*****************************************************************************
//
//	Function Name	: Cal_Checksum
//	Returned Value	: Unsigned 16-bit CRC value
//
//			Standard Modbus CRC calculation.
//
//*****************************************************************************
uint16_union cal_crc_value (char *data, uint16 count, uint16 start);


//*****************************************************************************
//
//	Function Name	: clear_rx_buffer
//	Returned Value	: none
//
//			Clear the RX buffer.
//
//*****************************************************************************
void clear_rx_buffer();



#endif /* SOURCES_DISPLAY_UART_H_ */
