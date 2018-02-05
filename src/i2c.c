/*
 * i2c.c
 *
 *  Created on: 07.01.2018
 *      Author: opetany
 */

#include "i2c.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>

#define I2C_TIMING			0x10E0A1E1

I2C_HandleTypeDef I2cHandle;

void write(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size)
{
	HAL_I2C_Mem_Write(&I2cHandle, (deviceAddress << 1), address, I2C_MEMADD_SIZE_8BIT, data, size, 1000);
}

void read(uint8_t deviceAddress, uint8_t address, uint8_t* buffer, uint8_t size)
{
	HAL_I2C_Mem_Read(&I2cHandle, (deviceAddress << 1), address, I2C_MEMADD_SIZE_8BIT, buffer, size, 1000);
}

I2cDriver* i2cInit()
{
	I2cDriver *driver = malloc(sizeof(I2cDriver));
	driver->write = write;
	driver->read = read;

	GPIO_InitTypeDef  			GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef  	RCC_PeriphCLKInitStruct;

	/*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

	 __HAL_RCC_GPIOA_CLK_ENABLE();			/* Enable GPIO TX/RX clock */
	__HAL_RCC_I2C1_CLK_ENABLE();			/* Enable I2C1 clock */

	/* I2C GPIO pins configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	I2cHandle.Instance             = I2C1;
	I2cHandle.Init.Timing          = I2C_TIMING;
	I2cHandle.Init.OwnAddress1     = (0x58 << 1);
	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.OwnAddress2     = 0;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	driver->status = HAL_I2C_Init(&I2cHandle);

	HAL_I2CEx_ConfigAnalogFilter(&I2cHandle,I2C_ANALOGFILTER_ENABLE);		/* Enable the Analog I2C Filter */

	return driver;
}
