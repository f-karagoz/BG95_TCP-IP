/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "bg95.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

BG95_TypeDef bg95;
uint8_t resultCode[20];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */


  bg95.handler = &huart2;

  memset(resultCode,'R',20);

  uint8_t invalidResponseBuffer[BUFFER_SIZE];
  //bg95.invalidResponse = invalidResponseBuffer;

  while ( resetDevice(&bg95) != 0);							// RESET DEVICE FOR FASTER DEBUGGING
  initializeApp ();											/*!< Initializes the application.	 */

  uint8_t myData = 115;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  socketApp ( myData );									/*!< Checks socket and sends data	 */
	  myData += 5;
	  HAL_Delay(15000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void USART2_IRQHandler (void) {
	/*********** STEPS FOLLOWED *************
	1. Check that interrupt is caused by RXNE
	2. If so read the data into the array member pointed by the pointer
	3. If interrupt is not caused by RXNE (which is not expected) run the ErrorHandler
	****************************************/

	if (USART2->ISR & USART_ISR_RXNE)
	{
		bg95.RxData.buffer[bg95.RxData.head] = USART2->RDR;
		bg95.RxData.head++;
		if (bg95.RxData.head == bg95.RxData.size) bg95.RxData.head = 0; // Circular buffer
	}
	else Error_Handler();
}

void initializeApp ( void )
{
	bg95.swState = 10;
	resultCode[0] = initDevice(&bg95);

	if ( resultCode[0] != 0 )
	{
	  app_Error_Handler( &bg95, &resultCode[0] );		// AT+CFUN=1 may response ERROR
	}

	bg95.swState = 20;
	resultCode[1] = getQgmr(&bg95);
	bg95.swState = 30;
	resultCode[2] = getCsq(&bg95);

	if ( (resultCode[2] != 0) || (bg95.csq.rssi == 0x52) || (bg95.csq.rssi == 99) )		/*!< 0x52: R : default app. reset value */
	{
	  app_Error_Handler( &bg95, &resultCode[2] );		// AT+CSQ may response two different ERRORs
	}

	bg95.swState = 40;
	resultCode[3] = sendWriteReceiveAtCommandT(&bg95, "CREG", "2", (uint8_t *) "AT+CREG\0", (uint8_t *) "OK\r\n\0", 350);  /*!< Max. spec. time 300 ms */

	if ( resultCode[3] != 0 )
	{
	  app_Error_Handler( &bg95, &resultCode[3] );		// AT+CREG=2 may response ERROR
	}

	resultCode[4] = getCreg(&bg95);

	if (bg95.creg.stat != 1)
	{
	  app_Error_Handler( &bg95, NULL );				// Device may not be registered to a home network
	}

	bg95.swState = 50;
	resultCode[5] = getCops(&bg95);

	if ( resultCode[5] != 0 )
	{
	  app_Error_Handler( &bg95, &resultCode[5] );		// AT+COPS? may response ERROR
	}

	bg95.swState = 60;
	resultCode[6] = setTcpIpContext(&bg95, 1, 1, "internet", "", "", 1);

	if ( resultCode[6] != 0 )
	{
	  app_Error_Handler( &bg95, &resultCode[6] );		// AT+QICSGP=... may response ERROR
	}

	/*! Activate PDP context with ID 1 				*/
	/*! Max. spec. time 150 s 						*/
	bg95.swState = 61;
	resultCode[7] = sendWriteReceiveAtCommandT(&bg95, "QIACT", "1", (uint8_t *) "AT+QIACT\0", (uint8_t *) "OK\r\n\0", 4000);

	if ( resultCode[7] != 0 )
	{
	  app_Error_Handler( &bg95, &resultCode[7] );		// AT+QIACT=1 may response ERROR
	}

	bg95.swState = 62;
	resultCode[8] = getQiact(&bg95);
	uint8_t size = strlen((char*)bg95.qiact.ip);
	uint8_t count = 0;
	for (int i=0; i<size; i++) if(bg95.qiact.ip[i] == 0x2E) count++; // 0x2E: .

	if( ( count != 3 ) || ( bg95.qiact.ip[0] == 0 ) || ( bg95.qiact.cState != 1 ) )
	{
	  app_Error_Handler( &bg95, NULL );				// IP may be unvalid or connection may not be established
	}
}

void socketApp ( uint8_t data )
{
	/*! Check for Internet connection. Check IP.			*/
	bg95.swState = 63;
	resultCode[8] = getQiact(&bg95);
	uint8_t size = strlen((char*)bg95.qiact.ip);
	uint8_t count = 0;
	for (int i=0; i<size; i++) if(bg95.qiact.ip[i] == 0x2E) count++; // 0x2E: .

	if( ( count != 3 ) || ( bg95.qiact.ip[0] == 0 ) || ( bg95.qiact.cState != 1 ) )
	{
		app_Error_Handler( &bg95, NULL );				// IP may be unvalid or connection may not be established
	}
	// Error handler activated the PDP context if it was not activated.



	/*! Open socket service.									*/
	bg95.swState = 70;
	resultCode[9] = openSocketService(&bg95, 1, 0, "TCP", "184.106.153.149", 80, 0, 1);

	if ( resultCode[9] != 0 )
	{
		app_Error_Handler( &bg95, &resultCode[9] );		// AT+QIOPEN=... ; <result> check needed for fault, ERROR may come too
	}

	/*! Check socket service.									*/
	// More stuff here...
	// AT+QISTATE=1,0	// 0 is the connectId of the socket

	/*! Send data through socket and verify with the response.	*/
	// More stuff here... detailed response verifying needed.
	volatile const uint8_t testVar = 0U;
	resultCode[10] = sendSocket(&bg95, testVar, data); // 0: bg95.qiact.cId

	if ( resultCode[10] != 0 )
	{
		app_Error_Handler( &bg95, &resultCode[10] );	// AFTER AT+SEND=0 and sending data, device may response SEND FAIL AND ERROR
	}
	//..

	/*! Close the socket service.								*/
	resultCode[11] = sendWriteReceiveAtCommandT(&bg95, "QICLOSE", "0,0", (uint8_t *) "AT+QICLOSE\0", (uint8_t *) "OK\r\n\0", 2000);
	if ( resultCode[11] != 0 )
	{
		app_Error_Handler( &bg95, &resultCode[11] );	// AFTER AT+QICLOSE=... device may response ERROR
	}

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
