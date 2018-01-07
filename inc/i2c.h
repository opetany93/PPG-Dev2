/*
 * i2c.h
 *
 *  Created on: 07.01.2018
 *      Author: opetany
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef struct{

	uint8_t status;
	void (*write)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size);
	void (*read)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size);

}I2cDriver;

I2cDriver* i2cInit();

#endif /* I2C_H_ */
