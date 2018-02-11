/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#include "AFE4404.h"
#include "boardConfig.h"
#include "arm_math.h"
#include "firPPG.h"
#include <stdlib.h>
#include "uart.h"

extern Afe4404Driver* 	afe4404driver;
extern UartDriver* 		uartDriver;

volatile uint8_t sampleCnt;
volatile uint32_t system_cnt;

extern volatile uint8_t doFFTflag;
extern volatile float32_t pulse;
extern float32_t bufferInput[8192];
volatile float32_t sample;
char buf[50];
/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	system_cnt++;
}

void EXTI4_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

		sample = firFilterPPG(afe4404driver->readLed2InInterrupt());

		measureTimePinSet(1);

		sprintf(buf, "%f %f\n", sample, pulse);
		uartDriver->writeString(buf);

		for(int i = 0; i < (8192-1); i++)
		{
			bufferInput[i] = bufferInput[i+1];
		}

		bufferInput[8192-1] = sample;

		if(99 > sampleCnt)
		{
			sampleCnt++;
		}
		else
		{
			doFFTflag = 1;
			sampleCnt = 0;
		}

		measureTimePinSet(0);
	}
}
