/* ###################################################################
 **     Filename    : Events.h
 **     Project     : Display
 **     Processor   : MKE04Z128VLD4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2016-11-01, 12:57, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.h
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
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

/* Global Variables */

// I2C
extern _Bool I2CIsBusy;

// Update Main Screen Probe (#1-#3) Reading
extern _Bool TI1_mainScreenUpdateTimer_Flag;
extern int TI1_mainScreenUpdateTimer;

// pentair icon timer
extern _Bool TI1_showComapnyLogoTimer_Flag;
extern int TI1_showComapnyLogoTimer;

// icon all on timer
extern _Bool TI1_iconAllOnTimer_Flag;
extern int TI1_iconAllOnTimer;

/* Update Screen Timer */
extern _Bool TI1_updateScreenTimerIsOn;
extern int TI1_updateScreenTimer;
extern _Bool updateScreenTimerDone;
extern int updateScreenRate;

// Up Button timer
extern _Bool TI1_upButtonTimerIsOn;
extern int TI1_upButtonTimer;

// Down Button timer
extern _Bool TI1_downButtonTimerIsOn;
extern int TI1_downButtonTimer;

// Back button timer
extern _Bool TI1_backButtonTimerIsOn;
extern int TI1_backButtonTimer;

// OK Button timer
extern _Bool TI1_OKButtonTimerIsOn;
extern int TI1_OKButtonTimer;

// brightness Timer
extern _Bool TI1_checkPixelsTimer_Flag;
extern int TI1_checkPixelsTimer;

// idle Timer
extern _Bool TI1_idleTimer_Flag;
extern int TI1_idleTimer;
extern int TI1_idleTimer_cnt;

// Validation Screen Timer
extern _Bool TI1_validationScreenTimer_Flag;
extern int TI1_validationScreenTimer;

// Alarm Timer
extern _Bool TI1_alarmTimer_Flag;
extern int TI1_alarmTimer;

// Count Down Timer
extern _Bool TI1_countDownTimer_Flag;
extern int TI1_countDownTimer;

// Just for demo
extern int countDownTimeRemain;

// UART Write Wait Timer
extern _Bool TI1_UARTWriteTimer_Flag;
extern int TI1_UARTWriteTimer;
extern _Bool showValidationScreen;

_Bool DataReceivedFlg;
_Bool DataSentFlg;

#ifdef __cplusplus
extern "C" {
#endif

/*
 ** ===================================================================
 **     Event       :  CI2C1_OnReceiveData (module Events)
 **
 **     Component   :  CI2C1 [InternalI2C]
 **     Description :
 **         This event is invoked when I2C finishes the reception of the
 **         data successfully. This event is not available for the SLAVE
 **         mode and if both RecvChar and RecvBlock are disabled. This
 **         event is enabled only if interrupts/events are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void CI2C1_OnReceiveData(void);

/*
 ** ===================================================================
 **     Event       :  CI2C1_OnTransmitData (module Events)
 **
 **     Component   :  CI2C1 [InternalI2C]
 **     Description :
 **         This event is invoked when I2C finishes the transmission of
 **         the data successfully. This event is not available for the
 **         SLAVE mode and if both SendChar and SendBlock are disabled.
 **         This event is enabled only if interrupts/events are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void CI2C1_OnTransmitData(void);

/*
 ** ===================================================================
 **     Event       :  TI1_OnInterrupt (module Events)
 **
 **     Component   :  TI1_125us [TimerInt]
 **     Description :
 **         When a timer interrupt occurs this event is called (only
 **         when the component is enabled - <Enable> and the events are
 **         enabled - <EnableEvent>). This event is enabled only if a
 **         <interrupt service/event> is enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void TI1_OnInterrupt(void);

/*
 ** ===================================================================
 **     Event       :  Display_OnBlockReceived (module Events)
 **
 **     Component   :  Display [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called when the requested number of data is
 **         moved to the input buffer.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void Display_OnBlockReceived(LDD_TUserData *UserDataPtr);

/*
 ** ===================================================================
 **     Event       :  Display_OnBlockSent (module Events)
 **
 **     Component   :  Display [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called after the last character from the
 **         output buffer is moved to the transmitter.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void Display_OnBlockSent(LDD_TUserData *UserDataPtr);

/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMI (module Events)
 **
 **     Component   :  Cpu [MKE04Z128LD4]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMI(void);

/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMIINT (module Events)
 **
 **     Component   :  Cpu [MKE02Z64LC2]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMIINT(void);

/*
** ===================================================================
**     Event       :  IFsh1_OnWriteEnd (module Events)
**
**     Component   :  IFsh1 [IntFLASH]
*/
/*!
**     @brief
**         Event is called after a write operation to FLASH memory is
**         finished (except [SetPage]). This event is available only if
**         an [Interrupt service/event] is selected.
*/
/* ===================================================================*/
void IFsh1_OnWriteEnd(void);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif
/* ifndef __Events_H*/
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
