/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <firPPG.h>
#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>
#include <string.h>

#include "boardConfig.h"
#include "uart.h"
#include "i2c.h"
#include "RTSP_communication.h"
#include "AFE4404.h"

#define RTSP_ENABLE		0

#if RTSP_ENABLE
	#include <math.h>
#endif

UartDriver* 	uartDriver;
Afe4404Driver* 	afe4404driver;

volatile uint32_t system_cnt = 0;

// ================================
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
#if RTSP_ENABLE
	int rtspBuffer[1];
#else
	char result[50];
#endif

	HAL_Init();
	clockInit();
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

	uartDriver = uartInit(1000000);
	ledInit();
	afeResetPinInit();
	measureTimePinInit();

	I2cDriver* i2c1Driver = i2c1Init();
	afe4404driver = afe4404Init(i2c1Driver, afeResetPinSet);

	afeRdyPinInit();

	while(1)
	{
#if RTSP_ENABLE
		rtspBuffer[0] = round(firFilterPPG(afe4404driver->readLed2()));
		rtspSendData(uartDriver->writeData, rtspBuffer, 1);
#else
		sprintf(result, "%f\n", firFilterPPG(afe4404driver->readLed2()));
		uartDriver->writeString(result);
#endif
	}
}
