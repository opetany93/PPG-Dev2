/*
 * AFE4404.h
 *
 *  Created on: 05.02.2018
 *      Author: opetany
 */

#ifndef AFE4404_H_
#define AFE4404_H_

#include <stdint.h>

typedef struct{

	uint32_t (*readReg)();
	void (*writeReg)(uint8_t address, uint32_t value);

}Afe4404Driver;

Afe4404Driver* afe4404Init(void (*write)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size), void (*read)(uint8_t deviceAddress, uint8_t address, uint8_t* data, uint8_t size));

#endif /* AFE4404_H_ */
