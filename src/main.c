/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>
#include <string.h>

#include "boardConfig.h"
#include "uart.h"
#include "i2c.h"

#include "AFE4404.h"

UartDriver* 	uartDriver;

volatile uint32_t system_cnt = 0;

void _Error_Handler(char * file, int line)
{
	char buf_out[50];
	sprintf(buf_out , "\r\n\r\n### ERROR in file:%s, line: %d, system_cnt: %lu [ms]" , file , line, system_cnt);
	uartDriver->writeString(buf_out);

	while(1)
	{
		ledToggle();
		HAL_Delay(100);
	}
}

int main(void)
{
	char result[50];

	HAL_Init();

	clockInit();
	uartDriver = uartInit(1000000);
	ledInit();
	afeResetPinInit();

	I2cDriver* 		i2c1Driver = i2c1Init();
	Afe4404Driver* 	afe4404driver = afe4404Init(i2c1Driver, afeResetPinSet, afeRdyPinRead);

	while(1)
	{
		sprintf(result, "%lu\n\r", afe4404driver->readLed2());
		uartDriver->writeString(result);
	}
}
