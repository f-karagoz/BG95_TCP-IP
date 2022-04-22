/*
 * App.h
 *
 *  Created on: Apr 21, 2022
 *      Author: fkaragoz
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32f746xx.h"

#define buffer_size 50

typedef struct {
	uint8_t size;
	uint8_t buffer[buffer_size];
	uint8_t head;
	uint8_t tail;
}rxData_TypeDef;

/**
 * @brief	Checks if the data in the head tail region is valid
 *
 * @param 	rxData		Pointer to a rxData_TypeDef struct
 * @param 	startChar	Start character that needs to be detected
 * @param 	endChar		End character that defines the end of the field
 * @return	0: Success
 */
uint8_t validateData(rxData_TypeDef * rxData, uint8_t startChar, uint8_t endChar);

/**
 * @brief Reads received data package considering the overlapping of the head
 * 			After the read operation moves the tail to head
 *
 * @param rxData	rxData_Typedef pointer to
 * @param buffer	Local buffer that we desire to move the received package
 */
uint8_t readData(rxData_TypeDef * rxData, uint8_t * buffer);

#endif /* INC_APP_H_ */
