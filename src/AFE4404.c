/*
 * AFE4404.c
 *
 *  Created on: 05.02.2018
 *      Author: opetany
 */

#include "AFE4404.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "stm32l4xx_hal.h"

#include "uart.h"
#include "boardConfig.h"

#define AFE4404_I2C_ADDRESS				0x58

#define LED2VAL_REG						((uint8_t) 0x2A)
#define ALED2VAL_REG					((uint8_t) 0x2B)
#define LED1VAL_REG						((uint8_t) 0x2C)
#define ALED1VAL_REG					((uint8_t) 0x2D)
#define LED2_ALED2VAL_REG				((uint8_t) 0x2E)
#define LED1_ALED1VAL_REG				((uint8_t) 0x2F)

static I2cDriver* i2cDriver;
extern void _Error_Handler(char * file, int line);

static volatile uint8_t dataRdyToRead = 0;

typedef enum
{
	TRUE 	= 0x00,
	FALSE   = 0x01
} bool;

// values of registers generated from AFE4404 EVM GUI
uint32_t defaultConfig[][2] = {

		{0x00, 0x000000}, /*CONTROL0*/
		{0x01, 0x00000D}, /*LED2STC*/
		{0x02, 0x000031}, /*LED2ENDC*/
		{0x03, 0x000066}, /*LED1LEDSTC*/
		{0x04, 0x000097}, /*LED1LEDENDC*/
		{0x05, 0x000040}, /*ALED2STC*/
		{0x06, 0x000064}, /*ALED2ENDC*/
		{0x07, 0x000073}, /*LED1STC*/
		{0x08, 0x000097}, /*LED1ENDC*/
		{0x09, 0x000000}, /*LED2LEDSTC*/
		{0x0A, 0x000031}, /*LED2LEDENDC*/
		{0x0B, 0x0000A6}, /*ALED1STC*/
		{0x0C, 0x0000CA}, /*ALED1ENDC*/
		{0x0D, 0x000036}, /*LED2CONVST*/
		{0x0E, 0x000066}, /*LED2CONVEND*/
		{0x0F, 0x00006B}, /*ALED2CONVST*/
		{0x10, 0x00009B}, /*ALED2CONVEND*/
		{0x11, 0x0000A0}, /*LED1CONVST*/
		{0x12, 0x0000D0}, /*LED1CONVEND*/
		{0x13, 0x0000D5}, /*ALED1CONVST*/
		{0x14, 0x000105}, /*ALED1CONVEND*/
		{0x15, 0x000033}, /*ADCRSTSTCT0*/
		{0x16, 0x000034}, /*ADCRSTENDCT0*/
		{0x17, 0x000068}, /*ADCRSTSTCT1*/
		{0x18, 0x000069}, /*ADCRSTENDCT1*/
		{0x19, 0x00009D}, /*ADCRSTSTCT2*/
		{0x1A, 0x00009E}, /*ADCRSTENDCT2*/
		{0x1B, 0x0000D2}, /*ADCRSTSTCT3*/
		{0x1C, 0x0000D3}, /*ADCRSTENDCT3*/
		{0x1D, 0x001387}, /*PRPCOUNT*/
		{0x1E, 0x000103}, /*CONTROL1*/
		{0x20, 0x008005}, /*TIAGAIN*/
		{0x21, 0x000005}, /*TIA_AMB_GAIN*/
		{0x22, 0x006186}, /*LEDCNTRL*/
		{0x23, 0x104218}, /*CONTROL2*/
		{0x29, 0x000000}, /*CLKDIV1*/
		{0x2A, 0x000000}, /*LED2VAL*/
		{0x2B, 0x000000}, /*ALED2VAL*/
		{0x2C, 0x000000}, /*LED1VAL*/
		{0x2D, 0x000000}, /*ALED1VAL*/
		{0x2E, 0x000000}, /*LED2-ALED2VAL*/
		{0x2F, 0x000000}, /*LED1-ALED1VAL*/
		{0x31, 0x000020}, /*CONTROL3*/
		{0x32, 0x000169}, /*PDNCYCLESTC*/
		{0x33, 0x001323}, /*PDNCYCLEENDC*/
		{0x34, 0x000000}, /*PROG_TG_STC*/
		{0x35, 0x000000}, /*PROG_TG_ENDC*/
		{0x36, 0x000033}, /*LED3LEDSTC*/
		{0x37, 0x000064}, /*LED3LEDENDC*/
		{0x39, 0x000006}, /*CLKDIV2*/
		{0x3A, 0x000000}, /*OFFDAC*/
};

static void readRegEnable()
{
	uint8_t buffer[] = {0, 0, 1};

	i2cDriver->write(i2cDriver, AFE4404_I2C_ADDRESS, 0x00, buffer, 3);

	if(i2cDriver->status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}

static void readRegDisable()
{
	uint8_t buffer[3] = {0};

	i2cDriver->write(i2cDriver, AFE4404_I2C_ADDRESS, 0x00, buffer, 3);

	if(i2cDriver->status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}

static uint32_t readReg(uint8_t registerAddress)
{
	uint8_t buffer[3] = {0};
	readRegEnable();

	i2cDriver->read(i2cDriver, AFE4404_I2C_ADDRESS, registerAddress, buffer, 3);

	if(i2cDriver->status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	readRegDisable();

	return (uint32_t)((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);
}

static uint32_t readOutputReg(uint8_t registerAddress)
{
	uint8_t buffer[3] = {0};

	i2cDriver->read(i2cDriver, AFE4404_I2C_ADDRESS, registerAddress, buffer, 3);

	if(i2cDriver->status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	return (uint32_t)((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);
}

static void writeReg(uint8_t address, uint32_t value)
{
	uint8_t buffer[3] = { value >> 16, value >> 8, value };

	i2cDriver->write(i2cDriver, AFE4404_I2C_ADDRESS, address, buffer, 3);

	if(i2cDriver->status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}

static void setDefaultConfig(void)
{
	for(uint8_t i = 0; i < 51; i++)
	{
		writeReg(defaultConfig[i][0], defaultConfig[i][1]);
	}
}

static bool checkDefaultConfig(void)
{
	for(uint8_t i = 1; i < 51; i++)
	{
		if( !((defaultConfig[i][0] >= LED2VAL_REG) && (defaultConfig[i][0] <= LED1_ALED1VAL_REG)) )
		{
			if ( readReg(defaultConfig[i][0]) != defaultConfig[i][1])
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

static void dataRdyCallback(void)
{
	dataRdyToRead = 1;
}

static uint32_t readLed2Blocking(void) /* To use this function we need to place dataRdyCallback in interrupt function from Afe Data Ready */
{
	while( 0 == dataRdyToRead );
	dataRdyToRead = 0;

	return readOutputReg(LED2VAL_REG);
}

static uint32_t readLed2NoBlocking(void)
{
	return readOutputReg(LED2VAL_REG);
}

Afe4404Driver* afe4404Init(I2cDriver* i2c, void (*setResetPin)(uint8_t state))
{
	i2cDriver = i2c;

	Afe4404Driver *driver = malloc(sizeof(Afe4404Driver));
	driver->readReg = readReg;
	driver->writeReg = writeReg;
	driver->readLed2Blocking = readLed2Blocking;
	driver->readLed2NoBlocking = readLed2NoBlocking;
	driver->rdyPinCallback = dataRdyCallback;

	(*setResetPin)(1);

	if(i2cDriver->isDeviceReady(i2cDriver, AFE4404_I2C_ADDRESS))
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	setDefaultConfig();

	if(checkDefaultConfig())
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	return driver;
}


