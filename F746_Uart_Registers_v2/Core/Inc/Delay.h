/*
 * Delay.h
 *
 *  Created on: Apr 13, 2022
 *      Author: fkaragoz
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "RccConfig.h"

/**
 * @brief Configures TIM6.
 *
 */
void TIM6Config (void);

/**
 * @brief Delay for uS.
 *
 * @param us Microsecond
 */
void delay_us (uint16_t us);

/**
 * @brief Delay for mS.
 *
 * @param ms Millisecond
 */
void delay_ms (uint16_t ms);

#endif /* INC_DELAY_H_ */
