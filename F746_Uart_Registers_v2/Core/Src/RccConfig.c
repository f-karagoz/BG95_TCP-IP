/*
 * RccConfig.c
 *
 *  Created on: Apr 12, 2022
 *      Author: fkaragoz
 */

#include "RccConfig.h"

void systemClockConfig(void) {
	/*************>>>>>>> STEPS FOLLOWED <<<<<<<<************
	1. ENABLE HSE and wait for the HSE to become Ready
	2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	3. Configure the FLASH PREFETCH and the LATENCY Related Settings
	4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	5. Configure the MAIN PLL
	6. Enable the PLL and wait for it to become ready
	7. Select the Clock Source and wait for it to be set
	********************************************************/
	// 1. ENABLE HSE and wait for the HSE to become Ready
	RCC->CR |= RCC_CR_HSEON; // RCC->CR |= (1<<16); // Set RCC's CR register's 16th bit to 1
	while (!(RCC->CR & RCC_CR_HSERDY));

	// 2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR1 |= PWR_CR1_VOS;

	// 3. Configure the FLASH PREFETCH and the LATENCY Related Settings
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS; // Our MCU does not have cache settings

	// 4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 	// AHB PS Div 1
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 	// APB1 PS Div 2
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; 	// APB2 PS Div 1

	// 5. Configure the MAIN PLL
	// PLLM=4	PLLN=72		PLLP=2(!)	PLLQ=3(did not use)
	RCC->PLLCFGR =0x00000000; // Reset the register. Default values prevent us to set correct values.
	RCC->PLLCFGR |= (4<<0) | (72<<6) | (0<<16);
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; // High speed external clock selected

	// 6. Enable the PLL and wait for it to become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)); 	// Wait for the PLL to get ready

	// 7. Select the Clock Source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);
}
