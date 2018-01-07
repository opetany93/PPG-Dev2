/*
 * uart.c
 *
 *  Created on: 07.01.2018
 *      Author: opetany
 */

/*
 * uart.c
 *
 *  Created on: 23.12.2017
 *      Author: opetany
 */

#include "uart.h"
#include <stdlib.h>

//#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_usart.h"

#define USART2_PORT		GPIOA
#define TX_PIN			GPIO_PIN_2;
#define RX_PIN			GPIO_PIN_3

#define TX_TIMEOUT		10000

USART_HandleTypeDef uartHandle;

static inline HAL_StatusTypeDef uartWrite(uint8_t data)
{
	return HAL_USART_Transmit(&uartHandle, &data, 1, TX_TIMEOUT);
}

void uartWriteString(char* s)
{
	while(*s){ uartWrite(*s++); }
}

HAL_StatusTypeDef uartWriteData(uint8_t* data, uint8_t size)
{
	return HAL_USART_Transmit(&uartHandle, data, size, TX_TIMEOUT);
}

UartDriver* uartInit(uint32_t baudrate)
{
	UartDriver *driver = malloc(sizeof(UartDriver));

	driver->writeString = uartWriteString;
	driver->writeData = uartWriteData;

	GPIO_InitTypeDef gpioInitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	// PA2 TX PIN
	gpioInitStruct.Pin = TX_PIN;
	gpioInitStruct.Mode = GPIO_MODE_AF_PP;
	gpioInitStruct.Pull = GPIO_PULLUP;
	gpioInitStruct.Alternate = GPIO_AF7_USART2;
	gpioInitStruct.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(USART2_PORT, &gpioInitStruct);

	// RX PIN PA3
	//LL_GPIO_SetPinMode(GPIOA, RX_PIN, LL_GPIO_MODE_ALTERNATE);
	//LL_GPIO_SetPinPull(GPIOA, RX_PIN, LL_GPIO_PULL_UP);
	//LL_GPIO_SetAFPin_0_7(GPIOA, RX_PIN, LL_GPIO_AF_7);

	uartHandle.Instance = USART2;
	uartHandle.Init.BaudRate = baudrate;
	uartHandle.Init.WordLength = USART_WORDLENGTH_8B;
	uartHandle.Init.StopBits = USART_STOPBITS_1;
	uartHandle.Init.Parity = USART_PARITY_NONE;
	uartHandle.Init.Mode = USART_MODE_TX;

	__HAL_RCC_USART2_CLK_ENABLE();

	if( !HAL_USART_Init(&uartHandle))
	{
		driver->status = ERROR;
		return driver;
	}

	__HAL_USART_ENABLE(&uartHandle);

	driver->status = SUCCESS;
	return driver;
}

//TODO: do deinit function


