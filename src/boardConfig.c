/*
 * boardConfig.c
 *
 *  Created on: 06.02.2018
 *      Author: opetany
 */
#include "boardConfig.h"
#include "stm32l4xx_hal.h"

#define AFE_RESET_PIN_PORT		GPIOB
#define AFE_RESET_PIN			GPIO_PIN_9
#define AFE_RDY_PIN_PORT		GPIOB
#define AFE_RDY_PIN				GPIO_PIN_4

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void clockInit(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		while(1);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		while(1);
	}

	/**Configure the main internal regulator output voltage
	*/
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		while(1);
	}

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void ledInit(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpioHandle;
	gpioHandle.Pin = GPIO_PIN_13;
	gpioHandle.Mode = GPIO_MODE_OUTPUT_PP;
	gpioHandle.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &gpioHandle);
}

void ledToggle(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
}

void setLed(uint8_t state)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, state);
}

void afeResetPinInit(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpioHandle;
	gpioHandle.Pin = AFE_RESET_PIN;
	gpioHandle.Mode = GPIO_MODE_OUTPUT_PP;
	gpioHandle.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(AFE_RESET_PIN_PORT, &gpioHandle);
}

void afeResetPinSet(uint8_t state)
{
	HAL_GPIO_WritePin(AFE_RESET_PIN_PORT, AFE_RESET_PIN, state);
}

void afeRdyPinInit(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpioHandle;
	gpioHandle.Pin = AFE_RDY_PIN;
	gpioHandle.Mode = GPIO_MODE_INPUT;
	gpioHandle.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(AFE_RDY_PIN_PORT, &gpioHandle);
}

uint8_t afeRdyPinRead(void)
{
	return HAL_GPIO_ReadPin(AFE_RDY_PIN_PORT, AFE_RDY_PIN);
}
