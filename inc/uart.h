/*
 * uart.h
 *
 *  Created on: 07.01.2018
 *      Author: opetany
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32l4xx.h"

typedef struct{

	uint8_t status;
	void (*writeString)(char* s);
	void (*writeData)(uint8_t* data, uint8_t size);

}UartDriver;

UartDriver* uartInit(uint32_t baudrate);

#endif /* UART_H_ */
