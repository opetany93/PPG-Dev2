/*
 * RTSP_communication.h
 *
 *  Created on: 20.12.2017
 *      Author: opetany
 */

#ifndef INCLUDE_RTSP_COMMUNICATION_H_
#define INCLUDE_RTSP_COMMUNICATION_H_

#include "stdint.h"

uint16_t rtspSendData(void (*writeData)(uint8_t* data, uint8_t size), int* data, uint8_t nbrOfChannels);

#endif /* INCLUDE_RTSP_COMMUNICATION_H_ */
