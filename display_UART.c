/* Including needed modules to compile this module/procedure */
#include "Display.h"
#include "Events.h"
#include "display_UART.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"

/* User includes */


bool rx_receiving = FALSE;
uint16_union modbus_rw_reg_rcv[RW_REG_LEN];			// read/write registers, modbus 1-29
uint16_union modbus_rw_reg_snd[RW_REG_LEN];			// read/write registers, modbus 1-29
uint16_union modbus_ro_reg_rcv[RO_REG_LEN];			// read only registers, modbus 201-220 237-244
uint16_union modbus_ero_reg_rcv[ERO_REG_LEN+1];		// extended read only registers, modbus 501-509 599
uint16_union modbus_ero_reg_snd[ERO_REG_LEN+1];		// extended read only registers, modbus 501-509 599
uint16_union modbus_wo_reg_snd[WO_REG_LEN];			// write only display firmware and hardware revision registers, modbus 491-492
byte modbus_rw_coil_rcv[RW_COIL_LEN/8+1];			// read/write coils, modbus 1-9
byte modbus_rw_coil_snd[RW_COIL_LEN/8+1];			// read/write coils, modbus 1-9
byte modbus_erw_coil_rcv[ERW_COIL_LEN/8+1];			// extended read/write coils, modbus 201-209
byte modbus_erw_coil_snd[ERW_COIL_LEN/8+1];			// extended read/write coils, modbus 201-209

uint16_union modbus_bootload_reg_rcv[1];			// Bootload read/write command, modbus 605
uint16_union modbus_bootload_reg_snd[1];			// Bootload read/write command, modbus 605



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
void display_uart_init()
{
	uint8 i;
	LDD_TError error;

	displayUART = Display_Init(NULL);
	//error = Display_ReceiveBlock(displayUART, (LDD_TData*)&dispRXbyte, 1);		// read one byte at a time

	for(i=0;i<DISP_RX_BUF_LEN;i++)
	{
		dispRXBuffer[i] = 0;
	}

	for (i=0;i<RW_COIL_LEN/8+1;i++)
	{
		write_coil_flags[i] = 0;
	}

	for (i=0;i<ERW_COIL_LEN/8+1;i++)
	{
		write_ext_coil_flags[i] = 0;
	}

//		modbus_wo_reg_snd[0].uvalue = 1;							// firmware revision
//		modbus_wo_reg_snd[1].uvalue = 4;							// hardware revision

}


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
uint16 rx_error_packet()
{
	uint16_union crc16;

	if (dispRXBuffer[1]>0x7F && dispRXBuffer[1]<0x90)		// test for error code
	{
		if (dispRXBuffer[2]>0x00 && dispRXBuffer[2]<0x05)	// test for exception code
		{
			crc16 = cal_crc_value (dispRXBuffer, 3U, 0U);	// test for correct crc
			if (dispRXBuffer[3]==crc16.bytes.byte0 && dispRXBuffer[4]==crc16.bytes.byte1)
			{
				return (uint16)dispRXBuffer[2];				// error packet detected!!!!!!!
			}
			else
			{
				return 0;									// valid crc not received
			}
		}
		else
		{
			return 0;										// valid exception code not received
		}
	}
	else
	{
		return 0;											// valid error code not received
	}

	return 0xFFFA;											// invalid exit of routine
}


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
uint16 rx_invalid_packet_header()
{
	uint16_union crc16;

	if (dispRXBuffer[0]==0x03 && (dispRXBuffer[1]==0x01 || dispRXBuffer[1]==0x03 || dispRXBuffer[1]==0x05 || dispRXBuffer[1]==0x06 )	)
	{
		// test for valid packet header
		//command to go to bootLoad mode is 0306025D00001842
		if(dispRXBuffer[1]==0x06 && dispRXBuffer[2]==0x02 && dispRXBuffer[3]==0x5D  && dispRXBuffer[4]==0x00  && dispRXBuffer[5]==0x00)
			//&& dispRXBuffer[6]==0x18 && dispRXBuffer[7]==0x42)
		{
			goToBootload = TRUE;
			clear_rx_buffer();
		}
		return 0;	// no header error detected
	}
	/*else if ((dispRXBuffer[0]==0x30 && dispRXBuffer[1]==0x30 && dispRXBuffer[2] == 0x41 && dispRXBuffer[3] == 0x35 && dispRXBuffer[4] == 0x30 && dispRXBuffer[5] == 0x30  && dispRXBuffer[6] == 0x41) ||
			(dispRXBuffer[0]==0x35 && dispRXBuffer[1]==0x30 && dispRXBuffer[2] == 0x30 && dispRXBuffer[3] == 0x41 && dispRXBuffer[4] == 0x35 && dispRXBuffer[5] == 0x30  && dispRXBuffer[6] == 0x30))
	{
		goToBootload = TRUE;
		return 0;
	}*/
	else
	{
		return 0x4000;										// packet error detected!!!!!!!
	}

	return 0xFFF9;											// invalid exit of routine
}

/*void addition_coil_read()
{
	LDD_TError error;

	error = display_coil_read(RW_COIL_START, RW_COIL_LEN); //READ_WRITE_COIL_LEN
	if (!error)
	{
		for (int i=0;i<RW_COIL_LEN/8+1;i++)
		{
			modbus_rw_coil_rcv[i] = dispRXBuffer[i+3];
		}
		clear_rx_buffer();
		//send_cnt = 0;
		//				comm_rx_complete = TRUE;
	}


	error = display_reg_read(ERO_REG_START+8, 1);
	if (!error)
	{
		for (int i=0;i<1;i++)
		{
			modbus_ero_reg_rcv[i+8].bytes.byte1 = dispRXBuffer[2*i+3];
			modbus_ero_reg_rcv[i+8].bytes.byte0 = dispRXBuffer[2*i+4];
		}
		clear_rx_buffer();
	}
}*/

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
LDD_TError display_uart_run()
{
	uint32 update_flags;				// temporary holding place for coil or register update flags
	uint16 coil_value;					// ModBUS coil value TRUE=0xFF00 and FALSE=0x0000
	uint8 i;							// generic index value
	LDD_TError error;					// error value returned from uart routines
	static uint32 send_cnt;				// bit position of the update flag being analyzed
	static uint8 state;					// current step in the state machine
	//	bool comm_rx_complete = FALSE;		// return value that indicates uart comm library is complete
	uint8 shift;

	uint16 UART_Return;

	// Communication state machine

	switch(state)
	{
		case 0: //read command for the bootload
			error = display_reg_read(RW_REG_BOOTLOAD, 1);
			if (!error)
			{
				if( dispRXBuffer[3] == 0x00 &&
					dispRXBuffer[4] == 0x00 &&
					dispRXBuffer[5] == 0xC1 &&
					dispRXBuffer[6] == 0x84) //Bootload Mode
					goToBootload = TRUE;
				clear_rx_buffer();
				state = 1;
			}
			break;

		case 1:	//Read first piece of r/w registers
			error = display_reg_read(RW_REG_START, 12);
			if (!error)
			{
				for (i=0;i<12;i++)
				{
					modbus_rw_reg_rcv[i].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_rw_reg_rcv[i].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 2;

//				//Test EEPROM Coil Values, 1-9
//				UART_Return = display_uart_update(COIL, RW_COIL_START+UNIT_OF_MEASURE, FALSE, 0, 0, UNIT_OF_MEASURE_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+COOL_HYSTERESIS, FALSE, 0, 0, COOL_HYSTERESIS_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+DOOR_SWITCH, FALSE, 0, 0, DOOR_SWITCH_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+DISABLE_SWITCH, FALSE, 0, 0, DISABLE_SWITCH_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+ALARM_OUTPUT, FALSE, 0, 0, ALARM_OUTPUT_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+PROBE2_PRESENT, FALSE, 0, 0, PROBE2_PRESENT_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+PASSWORD_ENABLED, FALSE, 0, 0, PASSWORD_ENABLED_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+HEATER_PRESENT, TRUE, 0, 0, HEATER_PRESENT_F);
//				UART_Return = display_uart_update(COIL, RW_COIL_START+COND_FAN2_PRESENT, TRUE, 0, 0, COND_FAN2_PRESENT_F);
//
//				//Test EEPROM Coil Values, 201-209
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+DIAGNOSTICS_ACTIVE, FALSE, 0, 0, DIAGNOSTICS_ACTIVE_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+COMPRESSOR_ACTIVE, FALSE, 0, 0, COMPRESSOR_ACTIVE_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+COND_FAN_MI_ACTIVE, FALSE, 0, 0, COND_FAN_MI_ACTIVE_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+EVAP_FAN_MI_ACTIVE, FALSE, 0, 0, EVAP_FAN_MI_ACTIVE_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+RELAYS_OFF, FALSE, 0, 0, RELAYS_OFF_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+FORWARD_RELAY_ACT, FALSE, 0, 0, FORWARD_RELAY_ACT_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+REVERSE_RELAY_ACT, FALSE, 0, 0, REVERSE_RELAY_ACT_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+COND_FAN2_MI_ACTIVE, FALSE, 0, 0, COND_FAN2_MI_ACTIVE_F);
//				UART_Return = display_uart_update(COIL, ERW_COIL_START+HEATER_ACTIVE, TRUE, 0, 0, HEATER_ACTIVE_F);
//
//				//Test EEPROM Register Values, 1-15
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+COOLING_SP_MIN, FALSE, 1,	COOLING_SP_MIN_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+COOLING_SP_MAX, FALSE, 2, COOLING_SP_MAX_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+COOLING_SP, FALSE, 3, COOLING_SP_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+COOLING_DIF_1, FALSE, 4, COOLING_DIF_1_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HIGH_TEMP_ALARM_SP, FALSE, 5, HIGH_TEMP_ALARM_SP_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HIGH_TEMP_ALARM_DIF, FALSE, 6, HIGH_TEMP_ALARM_DIF_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+LOW_TEMP_ALARM_SP, FALSE, 7, LOW_TEMP_ALARM_SP_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+LOW_TEMP_ALARM_DIF, FALSE, 8, LOW_TEMP_ALARM_DIF_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+FROST_ALARM_SP, FALSE, 9, FROST_ALARM_SP_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+FROST_ALARM_DIF, FALSE, 10, FROST_ALARM_DIF_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HEATING_SP_MIN, FALSE, 11, HEATING_SP_MIN_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HEATING_SP_MAX, FALSE, 12, HEATING_SP_MAX_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HEATING_SP, FALSE, 13, HEATING_SP_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+HEATING_DIF, FALSE, 14, HEATING_DIF_F, 0);
//				UART_Return = display_uart_update(REGISTER, RW_REG_START+COMP_MIN_STOP_TIME, FALSE, 15, COMP_MIN_STOP_TIME_F, 0);
//
//				//Test EEPROM Register Values, 491-492
//				UART_Return = display_uart_update(REGISTER, WO_REG_START+DISPLAY_FIRMWARE_REV, FALSE, 421, DISPLAY_FIRMWARE_REV_F, 0);
//				UART_Return = display_uart_update(REGISTER, WO_REG_START+DISPLAY_HARDWARE_REV, FALSE, 422, DISPLAY_HARDWARE_REV_F, 0);
			}
			break;

		case 2:	//Read second piece of r/w registers
			error = display_reg_read(RW_REG_START+HEATING_SP, 11);
			if (!error)
			{
				for (i=0;i<11;i++)
				{
					modbus_rw_reg_rcv[i+12].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_rw_reg_rcv[i+12].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 3;
			}
			break;

		case 3: //Read added registers (Heating Diff Max and GCM)
			error = display_reg_read(RW_REG_START+HEATING_DIF_MIN, 6);
			if(error == 0xFFFF)
			{
				clear_rx_buffer();
				state = 4;
				break;
			}
			if (!error)
			{
				for (i=0;i<6;i++)
				{
					modbus_rw_reg_rcv[i+23].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_rw_reg_rcv[i+23].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 4;
			}
			break;

		case 4:	//Read first piece of read only registers
			error = display_reg_read(RO_REG_START, 10);
			if (!error)
			{
				for (i=0;i<10;i++)
				{
					modbus_ro_reg_rcv[i].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_ro_reg_rcv[i].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 5;
			}
			break;

		case 5:	//Read second piece of read only registers
			error = display_reg_read(RO_REG_START+DIGITAL_IO, 10);
			if (!error)
			{
				for (i=0;i<10;i++)
				{
					modbus_ro_reg_rcv[i+10].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_ro_reg_rcv[i+10].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 6;
			}
			break;

		case 6: //Read added registers: Cool Set Point Min Max, Cool Diff Min Max, Group Control Size Min Max, GCM Status, Cool On Point
			error = display_reg_read(RO_COOL_SP_MIN, 8);
			if(!error)
			{
				for(i=0; i<8; i++)
				{
					modbus_ro_reg_rcv[i+20].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_ro_reg_rcv[i+20].bytes.byte0 = dispRXBuffer[2*i+4];
				}
			}
			clear_rx_buffer();
			state = 7;
			break;

		case 7: //Read GC Status Register
			error = display_reg_read(RO_GROUP_CONT_STATUS, 1);
			if(!error)
			{
				modbus_ro_reg_rcv[GROUP_CONT_STATUS].bytes.byte1 = dispRXBuffer[3];
				modbus_ro_reg_rcv[GROUP_CONT_STATUS].bytes.byte0 = dispRXBuffer[4];
			}
			clear_rx_buffer();
			state = 8;
			break;

		case 8: //Read First Work Status Register
			error = display_reg_read(RW1_ID1_WORK_STATUS, 1);
			if(!error)
			{
				modbus_ro_reg_rcv[ID1_WORK_STATUS].bytes.byte1 = dispRXBuffer[3];
				modbus_ro_reg_rcv[ID1_WORK_STATUS].bytes.byte0 = dispRXBuffer[4];
			}
			clear_rx_buffer();
			state = 9;
			break;

		case 9: //Read Second Work Status Register
			error = display_reg_read(RW1_ID2_WORK_STATUS, 1);
			if(!error)
			{
				modbus_ro_reg_rcv[ID2_WORK_STATUS].bytes.byte1 = dispRXBuffer[3];
				modbus_ro_reg_rcv[ID2_WORK_STATUS].bytes.byte0 = dispRXBuffer[4];
			}
			clear_rx_buffer();
			state = 10;
			break;

		case 10: //Read all coils
			error = display_coil_read(RW_COIL_START, RW_COIL_LEN); //READ_WRITE_COIL_LEN
			if (!error)
			{
				for (i=0;i<RW_COIL_LEN/8+1;i++)
				{
					modbus_rw_coil_rcv[i] = dispRXBuffer[i+3];
				}
				clear_rx_buffer();
				send_cnt = 0;
				//				comm_rx_complete = TRUE;
				state = 11;
			}
			//updateWrtRd = 0;
			break;

		case 11: // Write updated registers
			tx_timeout = 0;										// restart multi-packet TX timeout timer
			while (send_cnt<RW_REG_LEN)
			{
				update_flags = write_reg_flags;					// store update registers flags in temporary location
				update_flags >>= send_cnt;						// shift temporary flags
				if( 0x0001 & update_flags )
				{
					error = display_reg_single_write(RW_REG_START+send_cnt, modbus_rw_reg_snd[send_cnt].uvalue );
					if (!error)
					{
						send_cnt++;								// increment if tx is successful
					}
				}
				else
				{
					send_cnt++;									// increment if flag is 0
				}
				if (tx_timeout>=TX_MAX_WAIT)					// time out on transmit if not complete in x seconds.
				{
					send_cnt = RW_REG_LEN;
				}
			}
			if (send_cnt>=RW_COIL_LEN && tx_timeout<TX_MAX_WAIT)
			{
				clear_rx_buffer();
				write_reg_flags = 0;							// clear all flags if multi-packet TX is successful
				send_cnt = 0;
				state = 12;
			}
			else
			{
				clear_rx_buffer();
				send_cnt = 0;
			}
			break;

		case 12: // Write updated coils
			tx_timeout = 0;										// restart multi-packet TX timeout timer
			while (send_cnt<RW_COIL_LEN)
			{
				update_flags = write_coil_flags[send_cnt/8];	// store update coil flags in temporary location
				shift = send_cnt/8;
				shift = send_cnt - (shift * 8);
				update_flags >>= shift;							// shift temporary flags
				if( 0x0001&update_flags )
				{
					update_flags = (uint32)modbus_rw_coil_snd[send_cnt/8];
					update_flags = update_flags & 0x0001<<shift;
					if(update_flags)
					{
						coil_value = COIL_TRUE;
					}
					else
					{
						coil_value = COIL_FALSE;
					}
					error = display_coil_single_write(RW_COIL_START+send_cnt, coil_value );
					if (!error)
					{
						send_cnt++;								// increment if tx is successful
					}
				}
				else
				{
					send_cnt++;									// increment if flag is 0
				}
				if (tx_timeout>=TX_MAX_WAIT)					// time out on transmit if on complete in x seconds.
				{
					send_cnt = RW_COIL_LEN;
				}
			}
			if (send_cnt>=RW_COIL_LEN && tx_timeout<TX_MAX_WAIT)
			{
				clear_rx_buffer();
				for (i=0;i<(RW_COIL_LEN/8)+1;i++)
				{
					write_coil_flags[i] = 0;						// clear all flags if multi-packet TX is successful
				}
				send_cnt = 0;
				state = 13;
			}
			else
			{
				clear_rx_buffer();
				send_cnt = 0;
			}
			break;

		case 13: // Read production configuration registers (should read only)
			error = display_reg_read(ERO_REG_START, ERO_REG_LEN);
			if (!error)
			{
				for (i=0;i<9;i++)
				{
					modbus_ero_reg_rcv[i].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_ero_reg_rcv[i].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 14;
			}
			//updateWrtRd = 0;
			break;

		case 14: // Read production configuration registers (should read only)
			error = display_reg_read(ERO_REG_START+CURRENT_SETPOINT, 1);
			if (!error)
			{
				for (i=0;i<1;i++)
				{
					modbus_ero_reg_rcv[i+9].bytes.byte1 = dispRXBuffer[2*i+3];
					modbus_ero_reg_rcv[i+9].bytes.byte0 = dispRXBuffer[2*i+4];
				}
				clear_rx_buffer();
				state = 15;
			}
			//updateWrtRd = 0;
			break;

		case 15: // Write display firmware and hardware revision to be read by comm board
			tx_timeout = 0;										// restart multi-packet TX timeout timer
			while (send_cnt<WO_REG_LEN)
			{
				update_flags = write_rev_reg_flags;				// store update registers flags in temporary location
				update_flags >>= send_cnt;						// shift temporary flags
				if( 0x0001 & update_flags )
				{
					error = display_reg_single_write(WO_REG_START+send_cnt, modbus_wo_reg_snd[send_cnt].uvalue );
					if (!error)
					{
						send_cnt++;								// increment if tx is successful
					}
				}
				else
				{
					send_cnt++;									// increment if flag is 0
				}
				if (tx_timeout>=TX_MAX_WAIT)					// time out on transmit if not complete in x seconds.
				{
					send_cnt = WO_REG_LEN;
				}
			}
			if (send_cnt>=WO_REG_LEN && tx_timeout<TX_MAX_WAIT)
			{
				clear_rx_buffer();
				write_rev_reg_flags = 0;						// clear all flags if multi-packet TX is successful
				send_cnt = 0;
				state = 16;
			}
			else
			{
				clear_rx_buffer();
				send_cnt = 0;
			}
			break;

		case 16: // Write display firmware and hardware revision to be read by comm board
			tx_timeout = 0;										// restart multi-packet TX timeout timer
			while (send_cnt<ERO_REG_LEN)
			{
				update_flags = write_ero_reg_flags;				// store update registers flags in temporary location
				update_flags >>= send_cnt;						// shift temporary flags
				if( 0x0001 & update_flags )
				{
					error = display_reg_single_write(ERO_REG_START+send_cnt, modbus_ero_reg_snd[send_cnt].uvalue );
					if (!error)
					{
						send_cnt++;								// increment if tx is successful
					}
				}
				else
				{
					send_cnt++;									// increment if flag is 0
				}
				if (tx_timeout>=TX_MAX_WAIT)					// time out on transmit if not complete in x seconds.
				{
					send_cnt = ERO_REG_LEN;
				}
			}
			if (send_cnt>=ERO_REG_LEN && tx_timeout<TX_MAX_WAIT)
			{
				clear_rx_buffer();
				write_ero_reg_flags = 0;						// clear all flags if multi-packet TX is successful
				send_cnt = 0;
				state = 17;
			}
			else
			{
				clear_rx_buffer();
				send_cnt = 0;
			}
			break;

		case 17: // Read all coils
			error = display_coil_read(ERW_COIL_START, ERW_COIL_LEN);
			if (!error)
			{
				for (i=0;i<ERW_COIL_LEN/8+1;i++)
				{
					modbus_erw_coil_rcv[i] = dispRXBuffer[i+3];
				}
				clear_rx_buffer();
				send_cnt = 0;
				//				comm_rx_complete = TRUE;
				state = 18;
			}
			break;

		case 18: // Write updated coils
			tx_timeout = 0;										// restart multi-packet TX timeout timer
			while (send_cnt<ERW_COIL_LEN)
			{
				update_flags = write_ext_coil_flags[send_cnt/8];// store update coil flags in temporary location
				shift = send_cnt/8;
				shift = send_cnt - (shift * 8);
				update_flags >>= shift;							// shift temporary flags
				if( 0x0001&update_flags )
				{
					update_flags = (uint32)modbus_erw_coil_snd[send_cnt/8];
					update_flags = update_flags & 0x0001<<shift;
					if(update_flags)
					{
						coil_value = COIL_TRUE;
					}
					else
					{
						coil_value = COIL_FALSE;
					}
					error = display_coil_single_write(ERW_COIL_START+send_cnt, coil_value );
					if (!error)
					{
						send_cnt++;								// increment if tx is successful
					}
				}
				else
				{
					send_cnt++;									// increment if flag is 0
				}
				if (tx_timeout>=TX_MAX_WAIT)					// time out on transmit if on complete in x seconds.
				{
					send_cnt = ERW_COIL_LEN;
				}
			}
			if (send_cnt>=ERW_COIL_LEN && tx_timeout<TX_MAX_WAIT)
			{
				clear_rx_buffer();
				for ( i=0; i<(ERW_COIL_LEN/8)+1; i++ )
				{
					write_ext_coil_flags[i] = 0;				// clear all flags if multi-packet TX is successful
				}
				send_cnt = 0;
				state = 19;
			}
			else
			{
				clear_rx_buffer();
				send_cnt = 0;
			}
			break;

		default:
			state = 0;
			break;
	}

	/*if(updateWrtRd) //doing additional reading to re-update any ready that does not contain the most recent data
	{
		addition_coil_read(updateWrtRd);
		updateWrtRd = 0;
	}*/
	return error;
}


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
		uint8	coil_flags)				// coil flags to update
{
	if (type == COIL)
	{
		if ((address >= RW_COIL_START  && address <= RW_COIL_END) ||
			(address >= ERW_COIL_START && address <= ERW_COIL_END))
		{
			if (address >= RW_COIL_START  && address <= RW_COIL_END)
			{
				address -= RW_COIL_START;
				if (coil_state) {	modbus_rw_coil_snd[address/8] |= coil_flags;	}
				else			{	modbus_rw_coil_snd[address/8] &= ~coil_flags;	}
				write_coil_flags[address/8] |= coil_flags;
			}
			else if (address >= ERW_COIL_START && address <= ERW_COIL_END)
			{
				address -= ERW_COIL_START;
				if (coil_state) {	modbus_erw_coil_snd[address/8] |= coil_flags;	}
				else			{	modbus_erw_coil_snd[address/8] &= ~coil_flags;	}
				write_ext_coil_flags[address/8] |= coil_flags;
			}
			return 0;
		}
		else
		{
			return 0x1000U;										// invalid modbus coil address
		}
	}
	else if (type == REGISTER)
	{
		if ( (address >= RW_REG_START  && address <= RW_REG_END)  ||
			 (address >= WO_REG_START  && address <= WO_REG_END)  ||
			 (address >= ERO_REG_START && address <= ERO_REG_END) ||
			 (address == (ERO_REG_START+CURRENT_SETPOINT))	)
		{
			if (address >= RW_REG_START && address <= RW_REG_END)
			{
				address--;
				modbus_rw_reg_snd[address].ivalue = reg_value;
				write_reg_flags |= reg_flags;
			}
			else if (address >= WO_REG_START && address <= WO_REG_END)
			{
				address -= WO_REG_START;
				modbus_wo_reg_snd[address].ivalue = reg_value;
				write_rev_reg_flags |= reg_flags;
			}
			else if (address >= ERO_REG_START && address <= ERO_REG_END)
			{
				address -= ERO_REG_START;
				modbus_ero_reg_snd[address].ivalue = reg_value;
				write_ero_reg_flags |= reg_flags;
			}
			else if (address == (ERO_REG_START+CURRENT_SETPOINT))
			{
				address = 4;
				modbus_ero_reg_snd[address].ivalue = reg_value;
				write_ext_reg_flags |= reg_flags;
			}
			return 0;
		}
		else
		{
			return 0x1000U;										// invalid modbus coil address
		}
	}

	return 0xFFFB;
}


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
		uint16 length)					// Length of the data to read
{
	uint16_union crc16;											// standard crc16 calculation
	uint16_union adr;											// ModBUS address of data to read
	uint16_union len;
	LDD_TError error;

	// Device Address and Read Register Command
	dispTXBuffer[0] = 0x03U;									// ModBUS device address
	dispTXBuffer[1] = 0x03U;									// (0x03) Read Holding Registers

	// Load address into TX buffer
	if ( (address == RW_REG_BOOTLOAD) 									||
		 (address >= RW_REG_START && address <= RW_REG_END)				||
		 (address >= RO_REG_START && address <= RO_REG_END)				||
		 (address >= RO_COOL_SP_MIN && address <= RO_COOL_ON_POINT_2) 	||
		 (address == RO_GROUP_CONT_STATUS) 								||
		 (address == RW1_ID1_WORK_STATUS) 								||
		 (address == RW1_ID2_WORK_STATUS)								||
		 (address >= ERO_REG_START && address <= (ERO_REG_END)) 		|| //(ERO_REG_START+MALF_DEACTIVATE))
		 (address == (ERO_REG_START+CURRENT_SETPOINT))	)
	{
		adr.uvalue = address;
		dispTXBuffer[2] = adr.bytes.byte1;						// ModBUS address high
		dispTXBuffer[3] = adr.bytes.byte0;						// ModBUS address low
	}
	else
	{
		return 0x1000U;											// invalid modbus address
	}

	// Load length into TX buffer
	if ( length<=(DISP_RX_BUF_LEN-5)/2 )
	{
		len.uvalue = length;
		dispTXBuffer[4] = len.bytes.byte1;						// Data length high
		dispTXBuffer[5] = len.bytes.byte0;						// Data length low
	}
	else
	{
		return 0x1500;											// invalid length
	}

	// CRC16 Calculation for TX buffer
	crc16 = cal_crc_value (dispTXBuffer, 6U, 0U);
	dispTXBuffer[6] = crc16.bytes.byte0;						// CRC16 low
	dispTXBuffer[7] = crc16.bytes.byte1;						// CRC16 high

	error = display_uart_comm(TRUE);

	// CRC16 Calculation for the RX buffer
	if (!error)
	{
		crc16 = cal_crc_value (dispRXBuffer, 2*length+3U, 0U);
		if (dispRXBuffer[2*length+3] != crc16.bytes.byte0 || dispRXBuffer[2*length+4] != crc16.bytes.byte1)
		{
			return 0x2000;
		}
		else
		{
			return 0;
		}

	}
	else
	{
		return (uint16)error;
	}

	return 0xFFFE;
}

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
		uint16	value)					// Value to write to register
{
	uint16_union crc16;											// standard crc16 calculation
	uint16_union adr;											// ModBUS address of data to read
	uint16_union val;
	LDD_TError error;

	// Device Address and Read Register Command
	dispTXBuffer[0] = 0x03U;									// ModBUS device address
	dispTXBuffer[1] = 0x06U;									// (0x06) Write Single Registers

	// Load address into TX buffer
	if ( (address>=RW_REG_START && address<=RW_REG_END) || (address>=WO_REG_START && address<=WO_REG_END) || (address>=ERO_REG_START && address<=ERO_REG_END))
	{
		adr.uvalue = address;
		dispTXBuffer[2] = adr.bytes.byte1;						// ModBUS address high
		dispTXBuffer[3] = adr.bytes.byte0;						// ModBUS address low
	}
	else
	{
		return 0x1000U;											// invalid modbus address
	}

	// Load value into TX buffer
	val.uvalue = value;
	dispTXBuffer[4] = val.bytes.byte1;							// Data value high
	dispTXBuffer[5] = val.bytes.byte0;							// Data value low

	// CRC16 Calculation for TX buffer
	crc16 = cal_crc_value (dispTXBuffer, 6U, 0U);
	dispTXBuffer[6] = crc16.bytes.byte0;						// CRC16 low
	dispTXBuffer[7] = crc16.bytes.byte1;						// CRC16 high

	error = display_uart_comm(FALSE);

	// CRC16 Calculation for the RX buffer
	if (!error)
	{
		crc16 = cal_crc_value (dispRXBuffer, 6U, 0U);
		if (dispRXBuffer[6]!=crc16.bytes.byte0 || dispRXBuffer[7]!=crc16.bytes.byte1)
		{
			return 0x2000;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return (uint16)error;
	}

	return	0xFFFC;
}


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
		uint16 length)					// Length of the data to read
{
	uint16_union crc16;											// standard crc16 calculation
	uint16_union adr;											// ModBUS address of data to read
	uint16_union len;
	LDD_TError error;
	uint16 temp1;

	// Device Address and Read Register Command
	dispTXBuffer[0] = 0x03U;									// ModBUS device address
	dispTXBuffer[1] = 0x01U;									// (0x01) Read coils

	// Load address into TX buffer
	if ( (address>=RW_COIL_START  && address<=RW_COIL_END) ||
			(address>=ERW_COIL_START && address<=ERW_COIL_END) )
	{
		adr.uvalue = address;
		dispTXBuffer[2] = adr.bytes.byte1;						// ModBUS address high
		dispTXBuffer[3] = adr.bytes.byte0;						// ModBUS address low
	}
	else
	{
		return 0x1000U;											// invalid modbus address
	}

	// Load length into TX buffer
	if ( length<=(DISP_RX_BUF_LEN-5)*8 )
	{
		len.uvalue = length;
		dispTXBuffer[4] = len.bytes.byte1;						// Data length high
		dispTXBuffer[5] = len.bytes.byte0;						// Data length low
	}
	else
	{
		return 0x1500;											// invalid length
	}

	// CRC16 Calculation for TX buffer
	crc16 = cal_crc_value (dispTXBuffer, 6U, 0U);
	dispTXBuffer[6] = crc16.bytes.byte0;						// CRC16 low
	dispTXBuffer[7] = crc16.bytes.byte1;						// CRC16 high

	error = display_uart_comm(TRUE);

	length /= 8;

	// CRC16 Calculation for the RX buffer
	if (!error)
	{
		crc16 = cal_crc_value (dispRXBuffer, length+4U, 0U);
		if (dispRXBuffer[length+4]!=crc16.bytes.byte0 || dispRXBuffer[length+5]!=crc16.bytes.byte1)
		{
			return 0x2000;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return (uint16)error; //return this
	}

	return 0xFFFD;
}


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
		uint16	value)					// Value to write to coil
{
	uint16_union crc16;											// standard crc16 calculation
	uint16_union adr;											// ModBUS address of data to read
	uint16_union val;
	LDD_TError error;

	// Device Address and Read Register Command
	dispTXBuffer[0] = 0x03U;									// ModBUS device address
	dispTXBuffer[1] = 0x05U;									// (0x05) Write Single Coils

	// Load address into TX buffer
	if ( (address>=RW_COIL_START  && address<=RW_COIL_END) ||
			(address>=ERW_COIL_START && address<=ERW_COIL_END) )
	{
		adr.uvalue = address;
		dispTXBuffer[2] = adr.bytes.byte1;						// ModBUS address high
		dispTXBuffer[3] = adr.bytes.byte0;						// ModBUS address low
	}
	else
	{
		return 0x1000U;											// Parameter not found
	}

	// Load value into TX buffer
	if ( value==COIL_TRUE || value==COIL_FALSE )
	{
		val.uvalue = value;
		dispTXBuffer[4] = val.bytes.byte1;						// Data value high
		dispTXBuffer[5] = val.bytes.byte0;						// Data value low
	}
	else
	{
		return 0x1600;											// invalid coil value
	}

	// CRC16 Calculation for TX buffer
	crc16 = cal_crc_value (dispTXBuffer, 6U, 0U);
	dispTXBuffer[6] = crc16.bytes.byte0;						// CRC16 low
	dispTXBuffer[7] = crc16.bytes.byte1;						// CRC16 high

	error = display_uart_comm(FALSE);

	// CRC16 Calculation for the RX buffer
	if (!error)
	{
		crc16 = cal_crc_value (dispRXBuffer, 6U, 0U);
		if (dispRXBuffer[6]!=crc16.bytes.byte0 || dispRXBuffer[7]!=crc16.bytes.byte1)
		{
			return 0x2000;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return (uint16)error;
	}

	return 0xFFFB;
}


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
uint16 display_uart_comm(bool read_only)
{
	uint16_union rx_len;
	LDD_TError	tx_error = 0xAA;			// set to phony error state
	LDD_TError	rx_error;
	LDD_TError	cancel_rx_error;
	uint16		packet_error;
	uint16		header_error;
	uint8 		i;
	uint16		temp;

	if (read_only) //read
	{
		if ( dispTXBuffer[1]==0x03 )
		{
			rx_len.bytes.byte1 = dispTXBuffer[4];				// Data length high
			rx_len.bytes.byte0 = dispTXBuffer[5];				// Data length low
			rx_len.uvalue = 2 * rx_len.uvalue + 5U;				// adjust length for DEV ID, FC, Length, CRC and word length data
		}
		else if ( dispTXBuffer[1]==0x01 )
		{
			rx_len.bytes.byte1 = dispTXBuffer[4];				// Data length high
			rx_len.bytes.byte0 = dispTXBuffer[5];				// Data length low
			temp = rx_len.uvalue / 8;
			if  ( rx_len.uvalue % 8 )
			{
				temp++;
			}
			rx_len.uvalue = temp;
			//			if (rx_len.uvalue==0)
			//			{
			//				rx_len.uvalue = 1;
			//			}
			rx_len.uvalue += 5U;								// adjust length for DEV ID, FC, Length, CRC and bytes of packed coils (bits)
		}
	}
	else //write
	{													// write single register or single coil data
		rx_len.uvalue = 8;										// response is always 8 bytes
	}

	// TXing every second while not listening for RX
	if (!rx_receiving  && ti1_seconds>0)
	{
		tx_error = Display_SendBlock(displayUART, dispTXBuffer, 8U);	// Send block of characters
		ti1_seconds = 0;
		sec_1s_cnt = 0; /* clear delay counter, transmit request every 1 second */
		if (tx_error==ERR_OK)
		{
			rx_timeout = 0; /*start to receive response*/
		}
		else
		{
			return (uint16)tx_error;							// return RX error code
		}
	}

	// RXing every time a TX is successfully sent
	if (tx_error==ERR_OK || rx_receiving)
	{
		rx_receiving = TRUE;									// latch receiving mode ON
		rx_error = Display_ReceiveBlock(displayUART, dispRXBuffer, rx_len.uvalue);	// Receive block of characters

		while (!DataReceivedFlg && rx_timeout<RX_MAX_WAIT) 		// Wait until read command is sent, I think this means wait for the first RX interrupt
		{
			packet_error = rx_error_packet();
			header_error = rx_invalid_packet_header();
		}

		if ( (DataReceivedFlg || rx_timeout>=RX_MAX_WAIT) && packet_error>0 )
		{
			rx_receiving = FALSE;								// unlatch receive mode
			//			rx_cnt = 0;											// clear receive data count
			DataReceivedFlg = FALSE;
			cancel_rx_error = Display_CancelBlockReception(displayUART);	// cancel reception
			return packet_error;
		}

		if( (DataReceivedFlg || rx_timeout>=RX_MAX_WAIT) && header_error>0 )
		{
			rx_receiving = FALSE;								// unlatch receive mode
			//			rx_cnt = 0;											// clear receive data count
			DataReceivedFlg = FALSE;
			cancel_rx_error = Display_CancelBlockReception(displayUART);	// cancel reception
			return header_error;
		}

		if (rx_timeout>=RX_MAX_WAIT)							// if 100ms RX timeout occurs cancel the reception
		{
			rx_receiving = FALSE;								// unlatch receive mode
			//			rx_cnt = 0;											// clear receive data count
			DataReceivedFlg = FALSE;
			cancel_rx_error = Display_CancelBlockReception(displayUART);	// cancel reception
			return 0x1000U;										// return RX timeout error code
		}

		if (rx_error == ERR_OK)
		{
			rx_receiving = FALSE;								// unlatch receive mode
			//			rx_cnt = 0;											// clear receive data count
			DataReceivedFlg = FALSE;
			return 0;
		}
		else
		{
			return (uint16)rx_error;							// return RX error code
		}
	}

	return 0xFFFF;												//ECU TX error code; DU did not recieve anything
}


//*****************************************************************************
//
//	Function Name	: Cal_Checksum
//	Returned Value	: Unsigned 16-bit CRC value
//
//			Standard Modbus CRC calculation.
//
//*****************************************************************************
uint16_union cal_crc_value (char *data, uint16 count, uint16 start)
{
	uint16_union crc_value;
	uint16 i,j;
	uint16_t carry_flag;

	count += start;
	crc_value.uvalue = 0xFFFF;
	for (i=start; i<count; i++)
	{
		//carry_flag = (uint_8)data[Get_Index(i)];
		crc_value.uvalue ^= (uint8)data[i];
		for (j=0; j<8; j++)
		{
			carry_flag = crc_value.uvalue & 0x0001;
			crc_value.uvalue >>= 1;
			if (carry_flag)
			{
				crc_value.uvalue ^= 0xA001;
			}
		}
	}

	return crc_value;
}


//*****************************************************************************
//
//	Function Name	: clear_rx_buffer
//	Returned Value	: none
//
//			Clear the RX buffer.
//
//*****************************************************************************
void clear_rx_buffer()
{
	uint8 i;

	for(i=0;i<DISP_RX_BUF_LEN;i++)
	{
		dispRXBuffer[i] = 0;
	}
}
