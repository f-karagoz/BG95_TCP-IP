#include "Delay.h"

void TIM6Config (void) {
	/************** STEPS TO FOLLOW *****************
	1. Enable Timer clock
	2. Set the prescalar and the ARR
	3. Enable the Timer, and wait for the update Flag to set
	************************************************/
	// 1. Enable Timer clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	// 2. Set the prescalar and the ARR
	// 72Mhz default value as now of the TIM6 we need to set a prescaler for 1Mhz
	TIM6->PSC = 72-1; 	// +1 is added by default
	TIM6->ARR = 0xFFFF;	// Max auto-reload value, so we can count higher

	// 3. Enable the Timer, and wait for the update Flag to set
	TIM6->CR1 |= TIM_CR1_CEN;	// Enable the timer
	while(!(TIM6->SR & TIM_SR_UIF));	// Wait for update flag
}

void delay_us (uint16_t us) {
	/************** STEPS TO FOLLOW *****************
	1. RESET the Counter
	2. Wait for the Counter to reach the entered value. As each count will take 1 us,
		 the total waiting time will be the required us delay
	************************************************/
	// 1. RESET the Counter
	TIM6->CNT = 0x00000000U; // Reset the counter

	// 2. Wait for the Counter to reach the entered value.
	while(TIM6->CNT < us);
}

void delay_ms (uint16_t ms) {
	for(uint16_t i=0; i<ms;i++ ) {
		delay_us(1000);
	}
}
