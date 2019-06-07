/*
 * bootstrap.c
 *
 *  Created on: Jul 17, 2017
 *      Author: tgack
 */



#include <stdint.h>
#include "bootstrap.h"
#include "IFsh1.h"


const uint8_t *segBootStrapConfig = ((uint8_t*)LAUNCH_CODE_SEGMENT);

/**
 * Configure the system to enter bootloader then force a software reset.
 */
void setupBootLoad(void)
{
	IFsh1_EraseSector(LAUNCH_CODE_SEGMENT);

	//Soft reset
	// Force the system to reset
	SCB_AIRCR = SCB_AIRCR_VECTKEY(0x05FA) | SCB_AIRCR_SYSRESETREQ_MASK;

	// Wait for the system to reset.
	for(;;)
	{

	}


}

/**
 * return the boot strap configuration value.
 */
uint8_t getBootstrapConfig(void)
{
	return segBootStrapConfig[LAUNCH_CODE_INDEX];
}
