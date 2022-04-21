/*
 * UartConfig.h
 *
 *  Created on: Apr 15, 2022
 *      Author: fkaragoz
 */

#ifndef INC_UARTCONFIG_H_
#define INC_UARTCONFIG_H_

#include "stm32f746xx.h"

/**
 * @brief Configures UART.
 *
 */
void UartConfig (void);

/**
 * @brief Sends one char through USART3;
 *
 * @param c Actual char.
 */
void UART3_SendChar (uint8_t c);

/**
 * @brief Sends char array.
 *
 * @param data Pointer to char array.
 */
void UART3_SendData (uint8_t * data);

/**
 * @brief Gets the char from UART.
 *
 * @return Read char data.
 */
uint8_t UART3_GetChar (void);

#endif /* INC_UARTCONFIG_H_ */
