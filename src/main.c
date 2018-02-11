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

/* -------------------------------------------------------------------
* RTSP
* ------------------------------------------------------------------- */
#define RTSP_ENABLE		0

#if RTSP_ENABLE
	#include <math.h>
	int rtspBuffer[2];
#else
	char uartBuffer[50];
#endif

/* -------------------------------------------------------------------
* System definitions
* ------------------------------------------------------------------- */
UartDriver* 	uartDriver;
Afe4404Driver* 	afe4404driver;

volatile uint32_t system_cnt = 0;		/* incremented in the SysTick interrupt every 1 ms */
void _Error_Handler(char * file, int line);

/* -------------------------------------------------------------------
* FFT declarations
* ------------------------------------------------------------------- */
#define SAMPLE_RATE_HZ 100

#include "arm_math.h"
#include "arm_const_structs.h"

/* -------------------------------------------------------------------
* External Input and Output buffer Declarations for FFT
* ------------------------------------------------------------------- */
#define N 4096
uint32_t  noFilteredBufferInput[N];
float32_t bufferInput[N];
float32_t bufferOutput[N];
float32_t magnitudesOutput[N];
float32_t bufferForFFT[N];
float32_t bufferInForMatlab[N];
uint32_t  noFilteredBufferInForMatlab[N];
/* ------------------------------------------------------------------
* Global variables for FFT
* ------------------------------------------------------------------- */
uint32_t fftSize = 4096;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
uint32_t maxValueIndex = 0;											/* Reference index at which max energy of bin ocuurs */

/* ------------------------------------------------------------------
* Global variables (flags)
* ------------------------------------------------------------------- */
volatile uint8_t doFFTflag;

/* ------------------------------------------------------------------
* Global variables for storing calculated values
* ------------------------------------------------------------------- */
volatile float32_t pulse;
/* ------------------------------------------------------------------ */

void afeReadyInterruptHandler(void)
{
	static volatile uint8_t sampleCnt = 0;
	static float32_t sample;
	static uint32_t noFilteredSample;

	noFilteredSample = afe4404driver->readLed2NoBlocking();
	sample = firFilterPPG(noFilteredSample);		/* Read and filter the sample of PPG signal */

	// ===================== measured execution time = 1.9 ms (using sprintf) and 1.6 (using RTSP) ============
	measureTimePinSet(1);
#if RTSP_ENABLE
	rtspBuffer[0] = round(sample);
	rtspBuffer[1] = round(pulse);
	rtspSendData(uartDriver->writeData, rtspBuffer, 2);
#else
	//sprintf(uartBuffer, "%f %f\n", sample, pulse);
	//uartDriver->writeString(uartBuffer);
#endif

	for(int i = 0; i < (N-1); i++)		// bytes shifting by one in buffer
	{
		noFilteredBufferInput[i] = noFilteredBufferInput[i+1];
	}

	noFilteredBufferInput[N-1] = noFilteredSample;			// store sample at last element in buffer

	for(int i = 0; i < (N-1); i++)		// bytes shifting by one in buffer
	{
		bufferInput[i] = bufferInput[i+1];
	}

	bufferInput[N-1] = sample;			// store sample at last element in buffer

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
	// ========================================================================================================
}

/* ------------------------------------------------------------------ */
int main(void)
{
	float32_t maxValue;
	float32_t freq[N] = {0};

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

	/* generate axis frequency for FFT */
	for(int i = 1; i < N; i++)
	{
		freq[i] = freq[i-1] + ((float)SAMPLE_RATE_HZ)/((float)N);
	}

	arm_rfft_fast_instance_f32 S;
	int arm_status = arm_rfft_fast_init_f32(&S, fftSize);		/* FFT module initialization */

	if(ARM_MATH_SUCCESS != arm_status)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	while(1)
	{
		if(doFFTflag)
		{
			doFFTflag = 0;

			memcpy(bufferForFFT, bufferInput, 4*N);								/* Measured execution time = 115 us (LENGHT_SAMPLES = 4096) */

			memcpy(bufferInForMatlab, bufferInput, 4*N);
			memcpy(noFilteredBufferInForMatlab, noFilteredBufferInput, 4*N);

			// ===== Measured execution time = 37 ms (fftSize = 4096) ========
			arm_rfft_fast_f32(&S, bufferForFFT, bufferOutput, ifftFlag);		/* Perform a FFT transformation of PPG signal */
			arm_cmplx_mag_f32(bufferOutput, magnitudesOutput, fftSize);			/* Process the data through the Complex Magnitude Module for calculating the magnitude at each bin */
			arm_max_f32(magnitudesOutput, fftSize, &maxValue, &maxValueIndex); 	/* Calculates maxValue and returns corresponding BIN value */
			// ===============================================================

			pulse = 60.0 * freq[maxValueIndex];									/* Calculate pulse */

			sprintf(uartBuffer, " --- signal PPG and FFT magnitudes buffer: ---\n\r");
			uartDriver->writeString(uartBuffer);

			for (int i = 0; i < N; i++)
			{
				sprintf(uartBuffer, "%lu %f %f\n", noFilteredBufferInForMatlab[i], bufferInForMatlab[i], magnitudesOutput[i]);
				uartDriver->writeString(uartBuffer);
			}

			sprintf(uartBuffer, "\n\r---------- Calculated pulse: %f\n\r", pulse);
			uartDriver->writeString(uartBuffer);
		}
	}
}

/* ------------------------------------------------------------------ */
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
