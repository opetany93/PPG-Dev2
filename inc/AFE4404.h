/*
 * AFE4404.h
 *
 *  Created on: 05.02.2018
 *      Author: opetany
 */

#ifndef AFE4404_H_
#define AFE4404_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

#include "i2c.h"

typedef struct{

	uint32_t (*readReg)(uint8_t address);
	void (*writeReg)(uint8_t address, uint32_t value);
	uint32_t (*readLed2)(void);
	void (*rdyPinCallback)(void);

}Afe4404Driver;

Afe4404Driver* afe4404Init(I2cDriver* i2c, void (*setResetPin)(uint8_t state));

#endif /* AFE4404_H_ */
