/*
 * AFE4404.c
 *
 *  Created on: 05.02.2018
 *      Author: opetany
 */

#include "AFE4404.h"

#include <stddef.h>
#include <stdlib.h>

#define AFE4404_I2C_ADDRESS			0x58

/* AFE4404 registers */
#define AFE4404_TIA_GAIN_SEP		0x20
#define AFE4404_TIA_GAIN			0x21
#define AFE4404_PROG_TG_STC			0x34
#define AFE4404_PROG_TG_ENDC		0x35
#define AFE4404_LED3LEDSTC			0x36
#define AFE4404_LED3LEDENDC			0x37
#define AFE4404_CLKDIV_PRF			0x39
#define AFE4404_OFFDAC				0x3A
#define AFE4404_DEC					0x3D
#define AFE4404_AVG_LED2_ALED2VAL	0x3F
#define AFE4404_AVG_LED1_ALED1VAL	0x40

static void (*writeFunc)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size) = NULL;
static void (*readFunc)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size) = NULL;

static void readRegEnable()
{
	uint8_t buffer[] = {0, 0, 1};

	writeFunc(AFE4404_I2C_ADDRESS, 0x00, buffer, 3);
}

static void readRegDisable()
{
	uint8_t buffer[3] = {0};

	writeFunc(AFE4404_I2C_ADDRESS, 0x00, buffer, 3);
}

static uint32_t readReg()
{
	uint8_t buffer[3] = {0};
	readRegEnable();

	if(NULL != readFunc)
	{
		(*readFunc)(AFE4404_I2C_ADDRESS, 0x01, buffer, 3);
	}

	readRegDisable();

	return (uint32_t)((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);
}

static void writeReg(uint8_t address, uint32_t value)
{
	uint8_t buffer[3] = { value >> 16, value >> 8, value};

	if(NULL != writeFunc)
	{
		(*writeFunc)(AFE4404_I2C_ADDRESS, address, buffer, 3);
	}
}

Afe4404Driver* afe4404Init(void (*write)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size), void (*read)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size))
{
	writeFunc = write;
	readFunc = read;

	Afe4404Driver *driver = malloc(sizeof(Afe4404Driver));
	driver->readReg = readReg;
	driver->writeReg = writeReg;


	return driver;
}


