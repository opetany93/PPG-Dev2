/*
 * i2c.h
 *
 *  Created on: 07.01.2018
 *      Author: opetany
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

typedef struct{

	I2C_HandleTypeDef I2cHandle;
	uint8_t status;
	void (*write)(void* driver, uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size);
	void (*read)(void* driver, uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size);
	uint8_t (*isDeviceReady)(void* driver, uint8_t deviceAddress);

}I2cDriver;

I2cDriver* i2c1Init();

#endif /* I2C_H_ */
