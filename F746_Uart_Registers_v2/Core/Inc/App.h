/*
 * App.h
 *
 *  Created on: Apr 21, 2022
 *      Author: fkaragoz
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32f746xx.h"
#include <string.h>		// strlen();

#define buffer_size 50

typedef struct
{
	uint8_t size;
	uint8_t buffer[buffer_size];
	uint8_t head;
	uint8_t tail;
}rxData_TypeDef;

/**
 * @brief	Checks if the data in the head tail region is valid
 *
 * @param 	rxData		Pointer to a rxData_TypeDef struct
 * @param 	startString	Start string that needs to be detected
 * @param 	endString	End string that defines the end of the field
 * @return	10: Standard Success			20: Wrapping success			39: Head and tail point to same position.
 * 			17: Received data too short		27: Received data too short
 * 			18: Start check fail			28: Start check fail
 * 			19: End check fail				29: End check fail
 */
uint8_t validateData ( rxData_TypeDef * rxData, uint8_t * startString, uint8_t * endString );

/**
 * @brief 	Reads received data package considering the overlapping of the head
 * 			After the read operation moves the tail to head
 *
 * @param 	rxData	rxData_Typedef pointer to
 * @param 	buffer	Local buffer that we desire to move the received package
 * @return	1: Standard read success	2: Looping read success		3: Error
 *
 * @test	while (( validateData(...) % 10 ) != 0);
 * 			readData(&rxData, myBuffer);
 */
uint8_t readData ( rxData_TypeDef * rxData, uint8_t * buffer );

#endif /* INC_APP_H_ */
