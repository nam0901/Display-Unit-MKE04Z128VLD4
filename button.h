#ifndef __BUTTON_H
#define __BUTTON_H

#define BUTTON_HELD_REFRESH_TIME		400 // Every 50 ms
#define IDLE_TIMER_COUNTER_1_MIN		480000
#define IDLE_TIMER_COUNTER_30_SEC		240000
#define BUTTON_HELD_TIME				20000
#define BUTTON_DEBOUNCE_TIME			400

/* Global Variables */
extern _Bool pressedBack;
extern _Bool pressedOK;
extern _Bool pressedUp;
extern _Bool pressedDown;

extern _Bool releasedBack;
extern _Bool releasedOK;
extern _Bool releasedUp;
extern _Bool releasedDown;

extern _Bool heldUp;
extern _Bool heldDown;

/* Function Prototype */
/*
 ** ===================================================================
 **     Method      :  		scanButton
 **     Description :		Update button states
 ** ===================================================================
 */
void scanButton(void);

#endif
