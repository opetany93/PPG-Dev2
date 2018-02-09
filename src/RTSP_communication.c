/*
 * RTSP_communication.c
 *
 *  Created on: 20.12.2017
 *      Author: Arkadiusz Bochyñski
 */

#include "stdlib.h"
#include "string.h"

#include "RTSP_communication.h"
#include "crc16.h"

#define START_BYTE				(uint8_t)0xB4		// dop = 0x4B
#define STOP_BYTE				(uint8_t)0xD2		// dop = 0x2D
#define CHG_BYTE				(uint8_t)0xA5		// dop = 0x5A

static void (*writeDataFunc)(uint8_t* data, uint8_t size) = NULL;

/* maksymalna iloœc bajtów jaka mo¿e wyst¹pic:
   8 kana³ów * 4 bajty +
   2 bajty sumy CRC +
   1 bajt nbrOfChannels +
   2 bajty: startu + stopu +
   wszystkie mo¿liwe bajty change */
#define MAX_SIZE			69

static uint16_t sendFrame(uint8_t* buffer, uint16_t size)
{
	uint8_t frameSize = size + 2;
	buffer[0] = START_BYTE;
	buffer[frameSize-1] = STOP_BYTE;

	if( NULL != (*writeDataFunc) )
	{
		(*writeDataFunc)(buffer, frameSize);

		return size;
	}
	else
	{
		return 0;
	}
}

static uint8_t preparePayloadAndSend(uint8_t* buffer, uint16_t size)
{
	uint8_t* data = malloc(size);
	memcpy(data, buffer, size);
	buffer = malloc(MAX_SIZE);

	for(uint8_t k = 0 ; k < size ; k++)
	{
		if( (START_BYTE == data[k]) || (STOP_BYTE == data[k]) || (CHG_BYTE == data[k]) )
		{
			size++;											// zwiêkszam rozmiar danych o ilosc bajtów CHANGE, ktore zostan¹ dodane
		}
	}

	for (uint8_t k = 1, j = 0 ; k < (size+1); k++, j++)		// k = 1, poniewa¿ buffer[0] bêdzie bajtem startu
	{
		if( (START_BYTE == data[j]) || (STOP_BYTE == data[j]) || (CHG_BYTE == data[j]) )
		{
			buffer[k] = CHG_BYTE;
			buffer[k+1] = ~data[j];
			k++;
		}
		else
		{
			buffer[k] = data[j];
		}
	}

	free(data);
	uint16_t sentAllBytes = sendFrame(buffer, size);
	free(buffer);

	return sentAllBytes;
}

uint16_t rtspSendData(void (*writeData)(uint8_t* data, uint8_t size), int* data, uint8_t nbrOfChannels)
{
	writeDataFunc = writeData;

	if( nbrOfChannels > 8)
	{
		nbrOfChannels = 8;						// max 8 channels, if would be more , then send only first 8
	}

	uint8_t size = nbrOfChannels * 4 + 3; 		// number of channels * 4 bytes  +  one byte of size + two bytes of checksum
	uint8_t* buffer = (uint8_t* )malloc(size);
	*buffer = nbrOfChannels;					// first byte of buffer is number of channels

	for(int i = 0, k = 0; i < nbrOfChannels; i++)
	{
		buffer[++k] = (uint8_t)(data[i] >> 24);
		buffer[++k] = (uint8_t)(data[i] >> 16);
		buffer[++k] = (uint8_t)(data[i] >> 8);
		buffer[++k] = (uint8_t) data[i];
	}

	uint16_t crc = crc16_ccitt((const char* )buffer, size - 2);
	buffer[size - 2] = crc >> 8;
	buffer[size - 1] = crc;

	uint16_t sentAllBytes = preparePayloadAndSend(buffer, size);
	free(buffer);

	return sentAllBytes;
}
