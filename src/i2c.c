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

extern void _Error_Handler(char * file, int line);

static void write(void* driver, uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size)
{
	((I2cDriver*)driver)->status = HAL_I2C_Mem_Write(&((I2cDriver*)driver)->I2cHandle, (deviceAddress << 1), address, I2C_MEMADD_SIZE_8BIT, data, size, 500);
}

static void read(void* driver, uint8_t deviceAddress, uint8_t address, uint8_t* buffer, uint8_t size)
{
	((I2cDriver*)driver)->status = HAL_I2C_Mem_Read(&((I2cDriver*)driver)->I2cHandle, (deviceAddress << 1), address, I2C_MEMADD_SIZE_8BIT, buffer, size, 500);
}

static uint8_t isDeviceReady(void* driver, uint8_t deviceAddress)
{
	return HAL_I2C_IsDeviceReady(&((I2cDriver*)driver)->I2cHandle, (deviceAddress << 1), 10, 100);
}

I2cDriver* i2c1Init()
{
	I2cDriver *driver = malloc(sizeof(I2cDriver));
	driver->write = write;
	driver->read = read;
	driver->isDeviceReady = isDeviceReady;

	GPIO_InitTypeDef  			GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef  	RCC_PeriphCLKInitStruct;

	/*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

	__HAL_RCC_GPIOA_CLK_ENABLE();			/* Enable GPIO TX/RX clock */
	__HAL_RCC_I2C1_CLK_ENABLE();			/* Enable I2C1 clock */

	/* I2C GPIO pins configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;	/* SCL = PA9, SDA = PA10 */
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	driver->I2cHandle.Instance              = I2C1;
	driver->I2cHandle.Init.Timing           = 0x80601419;
	driver->I2cHandle.Init.OwnAddress1      = 0;
	driver->I2cHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
	driver->I2cHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
	driver->I2cHandle.Init.OwnAddress2      = 0;
	driver->I2cHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
	driver->I2cHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
	driver->I2cHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;

	driver->status = HAL_I2C_Init(&driver->I2cHandle);

	if(driver->status != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_I2CEx_ConfigAnalogFilter(&driver->I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK) /* Enable the Analog I2C Filter */
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_I2CEx_ConfigDigitalFilter(&driver->I2cHandle, 0) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	return driver;
}
