/*
 * boardConfig.h
 *
 *  Created on: 06.02.2018
 *      Author: opetany
 */

#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#include <stdint.h>

void clockInit(void);
void ledInit(void);
void ledToggle(void);
void setLed(uint8_t state);
void afeResetPinInit(void);
void afeResetPinSet(uint8_t state);
void afeRdyPinInit(void);
uint8_t afeRdyPinRead(void);

#endif /* BOARDCONFIG_H_ */
