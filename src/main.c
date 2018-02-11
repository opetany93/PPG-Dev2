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

// ================ RTSP ===============================
#define RTSP_ENABLE		0

#if RTSP_ENABLE
	#include <math.h>
#endif

// ============== system definitions =====================
UartDriver* 	uartDriver;
Afe4404Driver* 	afe4404driver;

volatile uint32_t system_cnt = 0;
void _Error_Handler(char * file, int line);

// =================== FFT =======================
// FFT and sampling configuration
#define SAMPLE_RATE_HZ 100

#include "arm_math.h"
#include "arm_const_structs.h"

#define LENGTH_SAMPLES 8192
/* -------------------------------------------------------------------
* External Input and Output buffer Declarations for FFT Bin Example
* ------------------------------------------------------------------- */
float32_t bufferInput[LENGTH_SAMPLES];
float32_t bufferOutput[LENGTH_SAMPLES];
float32_t testOutput[LENGTH_SAMPLES/2];

float32_t bufferForFFT[LENGTH_SAMPLES];
/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
uint32_t fftSize = 4096;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
/* Reference index at which max energy of bin ocuurs */
uint32_t testIndex = 0;

volatile uint8_t doFFTflag;
volatile float32_t pulse;

// ======================================================================================
int main(void)
{
#if RTSP_ENABLE
	int rtspBuffer[1];
#else
	char result[50];
#endif

	float32_t maxValue;
	float32_t freq[LENGTH_SAMPLES/2]={0};

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

	for(int i = 1; i < LENGTH_SAMPLES/2; i++)
	{
		freq[i] = freq[i-1] + ((float)SAMPLE_RATE_HZ)/4096.0;
	}

	arm_rfft_fast_instance_f32 S;
	int arm_status = arm_rfft_fast_init_f32(&S, fftSize);

	if(ARM_MATH_SUCCESS != arm_status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	while(1)
	{
#if RTSP_ENABLE
		rtspBuffer[0] = round(firFilterPPG(afe4404driver->readLed2UsingCallback()));
		rtspSendData(uartDriver->writeData, rtspBuffer, 1);
#else
		if(doFFTflag)
		{
			measureTimePinSet(1);

			doFFTflag = 0;

			memcpy(bufferForFFT, bufferInput, 4*LENGTH_SAMPLES);
			measureTimePinSet(0);
			arm_rfft_fast_f32(&S, bufferForFFT, bufferOutput, ifftFlag);
			arm_cmplx_mag_f32(bufferOutput, testOutput, fftSize);		/* Process the data through the Complex Magnitude Module for calculating the magnitude at each bin */
			arm_max_f32(testOutput, fftSize, &maxValue, &testIndex); 	/* Calculates maxValue and returns corresponding BIN value */

			pulse = 60.0 * freq[testIndex];


		}
#endif
	}
}

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
