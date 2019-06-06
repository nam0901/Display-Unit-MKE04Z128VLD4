/*
 * bootstrap.h
 *
 *  Created on: Jul 17, 2017
 *      Author: tgack
 */

#ifndef SOURCES_BOOTSTRAP_H_
#define SOURCES_BOOTSTRAP_H_


#define LAUNCH_CODE_SEGMENT		(0x3E00)
#define LAUNCH_CODE				(0xA5)
#define LAUNCH_CODE_INDEX		(0)

void setupBootLoad(void);
uint8_t getBootstrapConfig(void);

extern const uint8_t *segBootStrapConfig;

#endif /* SOURCES_BOOTSTRAP_H_ */
