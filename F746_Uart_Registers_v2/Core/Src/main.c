#include "main.h"

uint8_t Flag = 0;						// Flag for button press
uint8_t Flag2 = 0;						// Flag for toggling with uart
rxData_TypeDef RxData;
uint8_t TxData[100] = "DEFAULT_VAL";	// TxData buffer
/**
  * @brief  The application entry point.
  * @details	When communicating as M2M with At commands the response from the device may take up to 40sec.
  * 			The unpredictable response time causes ORE in the polling method UART.
  * 			In order to overcome that problem we will be reading data with interrupts to a large buffer.
  * 			An we will be following the received data'
  *
  * @todo		Handle unexpected start chars.
  * @todo		Handle buffer overflow.
  *
  * @retval int
  */
int main (void)
{
	systemClockConfig();
	UartConfig();
	TIM6Config();
	GPIO_Config();
	Interrupt_Config();

	RxData.size = buffer_size;
	memset ( RxData.buffer, 0, RxData.size );	// Empty the RxData buffer
	uint8_t myBuffer[100];
	//uint8_t startCheck[4] = { 'A','B','C','\0' };
	//uint8_t endCheck[3] = { 'D','E','\0' };
	uint8_t startCheck[2] = { '<','\0' };
	uint8_t endCheck[2] = { '>','\0' };

	while (1)
	{
		while ( ( validateData( &RxData, startCheck, endCheck ) % 10 ) != 0 );
		readData ( &RxData, myBuffer );
		Flag2 = !Flag2;

		if ( Flag2 == 0 )
		{
			GPIOB->BSRR |= (1<<0);			// Set LED
		}
		else
		{
			GPIOB->BSRR |= (1<<16);			// Reset LED
		}

		/*
		if (Flag) {
			sprintf((char*)TxData,"\r\nReceived data: %s\r\n> ",(char*)RxData.buffer); // Edit TxData with RxData
			memset(RxData.buffer,0,RxData.size);	// Clear RxData
			RxData.head = 0;				// Reset RxData index
			UART3_SendData(TxData);
			GPIOB->BSRR |= (1<<0);			// Set LED
			delay_ms(1000);
			Flag = 0;
		}
		else {
			GPIOB->BSRR |= (1<<16);			// Reset LED
		}
		*/
	}
}

void GPIO_Config (void) {
	/************ STEPS FOLLOWED ***********
	1. Enable the GPIO CLOCK
	2. Set the Pin as OUTPUT
	3. Configure the OUTPUT MODE
	Note: LED connected to B0
	***************************************/
	// 1. Enable the GPIO CLOCK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	// Enable GPIO B Clock

	// 2. Set the Pin as OUTPUT
	GPIOB->MODER |= (1<<0);	// Set B0 as General purpose output mode. Bits 1:0 set as (01)

	// 3. Configure the OUTPUT MODE
	GPIOB->OTYPER |= (0<<0);	// Push-pull type for pin 0.
	GPIOB->OSPEEDR |= (2<<0);	// High speed setup for pin 0
	GPIOB->PUPDR &= ~((1<<1) | (1<<0)); // 1:0 set as (00) for pin 0 to be no pull up or down
	/************ STEPS FOLLOWED ***********
	1. Enable the GPIO CLOCK
	2. Set the Pin as INPUT
	3. Configure the PULL UP or DOWN
	Note: Button connected to PC13 and hardware pull-down
	***************************************/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	// Enable GPIO C Clock
	GPIOC->MODER &= ~GPIO_MODER_MODER13;	// Set C13 as Input with bits (27:26) as 0:0
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;	// No pull up or pull down
	GPIOC->PUPDR |= (2 << GPIO_PUPDR_PUPDR13_Pos); // Pull down
}

void Interrupt_Config (void) {
	/*************>>>>>>> STEPS FOLLOWED <<<<<<<<************
	1. Enable the SYSCNFG bit in RCC register
	2. Configure the EXTI configuration Register in the SYSCNFG
	3. Disable the EXTI using Interrupt Mask Register (IMR)
	4. Configure the Rising Edge / Falling Edge Trigger
	5. Set the Interrupt Priority
	6. Enable the interrupt
	********************************************************/
	// 1. Enable the SYSCNFG bit in RCC register
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// 2. Configure the EXTI configuration Register in the SYSCNFG
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// 3. Disable the EXTI using Interrupt Mask Register (IMR)
	EXTI->IMR |= EXTI_IMR_IM13;

	// 4. Configure the Rising Edge / Falling Edge Trigger
	EXTI->RTSR |= EXTI_RTSR_TR13; // Rising edge trigger for line 13 is set
	EXTI->FTSR &= ~EXTI_FTSR_TR13;

	// 5. Set the Interrupt Priority
	NVIC_SetPriority(EXTI15_10_IRQn, 0);

	// 6. Enable the interrupt
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void) {
	/*************>>>>>>> STEPS FOLLOWED <<<<<<<<************
	1. Check the Pin, which triggered the Interrupt
	2. Clear the Interrupt Pending Bit
	********************************************************/

	if(EXTI->PR & EXTI_PR_PR13) {	// We are checking if this interrupt handler triggered by 13th interrupt line
		Flag = 1;
		EXTI->PR |= EXTI_PR_PR13;	// Bit is cleared by writing 1 to it as described in the RM.
	}
}

void USART3_IRQHandler (void) {
	/*********** STEPS FOLLOWED *************
	1. Check that interrupt is caused by RXNE
	2. If so read the data into the array member pointed by the pointer
	3. If interrupt is not caused by RXNE (which is not expected) run the ErrorHandler
	****************************************/

	if (USART3->ISR & USART_ISR_RXNE) {
		RxData.buffer[RxData.head] = USART3->RDR;
		RxData.head++;
		if (RxData.head == RxData.size) RxData.head = 0; // Circular buffer
	}
	else ErrorHandler();
}

void ErrorHandler (void) {
	while(1); //gotcha!
}

