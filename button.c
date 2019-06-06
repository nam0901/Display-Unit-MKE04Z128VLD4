#include "button.h"
#include "Back_Button.h"
#include "BitIoLdd6.h"
#include "OK_Button.h"
#include "BitIoLdd7.h"
#include "Up_Button.h"
#include "BitIoLdd8.h"
#include "Down_Button.h"
#include "BitIoLdd9.h"
#include "displayPosition.h"
#include "stdbool.h"
#include "Events.h"

/*
** ===================================================================
**     Method      :  		scanButton
**     Description :		Update button states
** ===================================================================
*/
void scanButton(void)
{
	// If the display is showing validation screen, ignore any button press
	if (!showValidationScreen)
	{
		// Back button
		if (!Back_Button_GetVal())
		{
			releasedBack = false;

			if (!TI1_backButtonTimerIsOn)
			{
				TI1_backButtonTimerIsOn = true;
				TI1_backButtonTimer = 0;
			}
			else
			{
				if (TI1_backButtonTimer > BUTTON_DEBOUNCE_TIME)
				{
					pressedBack = true;
				}
			}
		}
		else
		{
			TI1_backButtonTimerIsOn = false;
			TI1_backButtonTimer = 0;

			if (pressedBack)
			{
				pressedBack = false;
				releasedBack = true;
			}
		}

		// OK Button
		if (!OK_Button_GetVal())
		{
			releasedOK = false;
			justClickOK = TRUE;

			if (!TI1_OKButtonTimerIsOn)
			{
				TI1_OKButtonTimerIsOn = true;
				TI1_OKButtonTimer = 0;
			}
			else
			{
				if (TI1_OKButtonTimer > BUTTON_DEBOUNCE_TIME)
				{
					pressedOK = true;
				}
			}
		}
		else
		{
			TI1_OKButtonTimerIsOn = false;
			TI1_OKButtonTimer = 0;

			if (pressedOK)
			{
				pressedOK = false;
				releasedOK = true;
			}
		}

		// Up button
		if (!Up_Button_GetVal())
		{
			releasedUp = false;

			if (!TI1_upButtonTimerIsOn)
			{
				TI1_upButtonTimerIsOn = true;
				TI1_upButtonTimer = 0;
			}
			else
			{
				if (TI1_upButtonTimer < BUTTON_HELD_TIME && TI1_upButtonTimer > BUTTON_DEBOUNCE_TIME)
				{
					pressedUp = true;
				}
				if (TI1_upButtonTimer >= BUTTON_HELD_TIME)
				{
					heldUp = true;
				}
			}
		}
		else
		{
			TI1_upButtonTimerIsOn = false;
			TI1_upButtonTimer = 0;
			if (pressedUp || heldUp)
			{
				pressedUp = false;
				heldUp = false;
				releasedUp = true;
			}
		}

		// Down button
		if (!Down_Button_GetVal())
		{
			releasedDown = false;
			if (!TI1_downButtonTimerIsOn)
			{
				TI1_downButtonTimerIsOn = true;
				TI1_downButtonTimer = 0;
			}
			else
			{
				if ((TI1_downButtonTimer < BUTTON_HELD_TIME) && (TI1_downButtonTimer > BUTTON_DEBOUNCE_TIME))
				{
					pressedDown = true;
				}
				if (TI1_downButtonTimer >= BUTTON_HELD_TIME)
				{
					heldDown = true;
				}
			}
		}
		else
		{
			TI1_downButtonTimerIsOn = false;
			TI1_downButtonTimer = 0;

			if (pressedDown || heldDown)
			{
				pressedDown = false;
				heldDown = false;
				releasedDown = true;
			}
		}

		// Press Four Button: Turn on entire screen to check all pixels.
		if (pressedDown && pressedUp && pressedBack && pressedOK)
		{
			currentPosition.displayLevel = CHECK_ALL_PIXELS;
			updateDisplayPosition = true;
		}

		// If user press or hold any button, then set updateDisplayPostion to be true to update currentPosition.
		if (releasedBack || releasedOK || releasedUp || releasedDown || heldUp || heldDown)

		{
			updateDisplayPosition = true;
		}
	}

	/*
	 * If the display is in the menu (MAIN_MENU_POSITION <= currentPosition.displayPosition <= THREE_PHASE_CURRENT_MENU_POSITION)
	 * and no button is being pressed for more than a minute, or if the display is showing either the enclosure outlet temperature
	 * or condenser coil temperature and no button is being pressed for more than 30 seconds, then display will enter the idle mode.
	 * Display in the idle mode will show the main screen or alarms if they are active.
	 */
	if (!pressedDown && !pressedUp && !pressedBack && !pressedOK)
	{
		if ((currentPosition.displayLevel >= HIGH_TEMP_ALARM_POSITION) && (currentPosition.displayLevel <= GROUP_CONTROL_ALARM_POSITION))// alarms
		{
			TI1_idleTimer_Flag = false;
			TI1_idleTimer = 0;
		}
		else
		{
			// set the idle timer counter
			if ((currentPosition.displayLevel == MAIN_SCREEN_POSITION) && (currentPosition.lineNumber != INLET_LINENUM))
			{
				TI1_idleTimer_cnt = IDLE_TIMER_COUNTER_30_SEC + 120000; // it seems IDLE_TIMER_COUNTER_30_SEC is not enough to give a 30 seconds. It only gives 20 secs,
				//so IDLE_TIMER_COUNTER_30_SEC/20sec makes 12000 a second. To add another 10 sec to the 20 sec to give 30 sec, we need 10(12000)=120000
			}
			else if (currentPosition.displayLevel != MAIN_SCREEN_POSITION)
			{
				TI1_idleTimer_cnt = IDLE_TIMER_COUNTER_1_MIN + 240000;//IDLE_TIMER_COUNTER_1_MIN only gives 40 seconds. Need another 20 seconds to make 60 secs.
				//to do that, we need 20(12000)=240000
			}
			TI1_idleTimer_Flag = true;
		}
	}
	else
	{
		TI1_idleTimer_Flag = false;
		TI1_idleTimer = 0;
	}
}
