/**
 * @file	bg95.c
 * @author	Furkan KARAGOZ
 * @date	22/03/2022
 * @brief	Quectel BG95 module driver.
 *
 */

#include "bg95.h"

/*
uint8_t receiveAtResponse(BG95_TypeDef * device, uint8_t * buffer) {
	uint16_t size = strlen((char*)buffer);
	for(int i=0; i<size; i++) buffer[i]='~'; 				// we may change this as memset
	HAL_StatusTypeDef retVal = HAL_UART_Receive(device->handler, buffer, size, 30); //!<
	if (retVal == HAL_OK) return 0;
	else if (retVal == HAL_TIMEOUT) return 1;
	else return 2;
}
*/

uint8_t receiveAtResponse(BG95_TypeDef * device, uint8_t * buffer, uint8_t * startCheck, uint8_t * endCheck)
{
	uint16_t size = strlen((char*)buffer);
	for(int i=0; i<size; i++) buffer[i]='~'; 				// we may change this as memset
	//uint8_t startCheck[2] = { '<','\0' };
	//uint8_t endCheck[2] = { '>','\0' };

	while ( ( validateData( &(device->RxData), startCheck, endCheck ) % 10 ) != 0 );
	readData ( &(device->RxData), buffer );
	return 0;
}

/*
uint8_t receiveAtResponseT(BG95_TypeDef * device, uint8_t * buffer, uint32_t timeout) {
	uint16_t size = strlen((char*)buffer);
	for(int i=0; i<size; i++) buffer[i]='~'; 				// we may change this as memset
	HAL_StatusTypeDef retVal = HAL_UART_Receive(device->handler, buffer, size, timeout); //!<
	if (retVal == HAL_OK) return 0;
	else if (retVal == HAL_TIMEOUT) return 1;
	else return 2;
}
*/

uint8_t receiveAtResponseT(BG95_TypeDef * device, uint8_t * buffer, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout)
{
	uint16_t size = strlen((char*)buffer);
	for(int i=0; i<size; i++) buffer[i]='~'; 				// we may change this as memset
	//uint8_t startCheck[] = "AT\r\r\n\0";
	//uint8_t endCheck[] = "\r\n\0";
	uint32_t timeMark = HAL_GetTick();

	while ( HAL_GetTick() - timeMark < timeout )
	{
		if ( ( validateData( &(device->RxData), startCheck, endCheck ) % 10 ) == 0 )
		{
			readData ( &(device->RxData), buffer );
			return 0;
		}
	}
	if ( device->RxData.head != device->RxData.tail )
	{
		readData ( &(device->RxData), buffer );			/*!< Un-validated read. Received data may contain ERROR response.	*/
		return 1;
	}
	//device->RxData.tail = device->RxData.head;	/*!< In case we received some un-validated data we virtually clear the data field for future	*/
	return 2;
}

uint8_t getBetween(uint8_t * sourceBuf, uint8_t * targetBuf, uint8_t firstChar, uint8_t secondChar)
{
	uint16_t size = strlen((char*)sourceBuf);
	uint8_t firstIndx = 0;				/*!< Collects the index of the first character. For space, 0x20 or 32 or ' '	*/
	uint8_t lastIndx = 0;				/*!< Collects the index of the last character. For CR, 0x0D or 13 				*/

	for(int i=0; i<size; i++)
	{
		if (sourceBuf[i] == firstChar)
		{
			// first index found
			firstIndx=i+1;
			for(int j=i; j<size; j++)
			{
				if (sourceBuf[j] == secondChar)
				{
					// last index found
					// collect data
					lastIndx=j-1;
					uint8_t dataSize = lastIndx-firstIndx+1;
					for (int k=0; k<dataSize; k++)
					{
						targetBuf[k] = sourceBuf[firstIndx+k];
					}
					targetBuf[dataSize] = '\0'; // null terminator for the end of the target
					return 0;
				}
			}
			// end of array scan after first char detected but no last char detected
			// collect data
			uint8_t dataSize = strlen((char*)sourceBuf)-firstIndx;
			for (int k=0; k<dataSize; k++)
			{
				targetBuf[k] = sourceBuf[firstIndx+k];
			}
			targetBuf[dataSize] = '\0'; // null terminator for the end of the target buffer
			return 2;
		}
	}
	// end of array scan but no first char found
	return 1;
}

uint8_t sendCommand(BG95_TypeDef * device, char* command)
{
	uint8_t com_buf[100];
	memset(com_buf,'\0',100);
	sprintf((char*)com_buf, "%s\r\n",command);
	uint16_t size = strlen((char*)com_buf); 				// we may move this inside uart transmit para

	HAL_StatusTypeDef retVal = HAL_UART_Transmit(device->handler, com_buf, size, 10);
	if (retVal != HAL_OK) return 1;
	else return 0;
}

uint8_t send_plain(BG95_TypeDef * device, char* command)
{
	uint8_t com_buf[100];
	memset(com_buf,'\0',100);
	sprintf((char*)com_buf, "%s",command);
	uint16_t size = strlen((char*)com_buf); 				// we may move this inside uart transmit para

	HAL_StatusTypeDef retVal = HAL_UART_Transmit(device->handler, com_buf, size, 10);
	if (retVal != HAL_OK) return 1;
	else return 0;
}

uint8_t sendReceiveCommandT(BG95_TypeDef * device, char* command, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout)
{
	if ( sendCommand(device, command) == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, timeout) != 2)
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset

			if (getBetween(rxBuf, parBuf, 0x0A, 0x0D) != 1 )			/*!< LF: 0x0A, CR: 0x0D 		*/
			{
				if ((parBuf[0] == 'O') && (parBuf[1] == 'K')) return 0;
				/*! ERROR received	*/
				else if ((parBuf[0] == 'E') && (parBuf[1] == 'R') && (parBuf[2] == 'R') && (parBuf[3] == 'O') && (parBuf[4] == 'R') ) return 5;
				else return 4;						/*!< Did not receive OK					*/
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t sendAtCommand(BG95_TypeDef * device, char* command)
{
	uint8_t com_buf[100];
	memset(com_buf,'\0',100);
	sprintf((char*)com_buf, "AT+%s\r\n",command);
	uint16_t size = strlen((char*)com_buf); 				// we may move this inside uart transmit para

	HAL_StatusTypeDef retVal = HAL_UART_Transmit(device->handler, com_buf, size, 10);
	if (retVal != HAL_OK) return 1;
	else return 0;
}

/*
uint8_t sendReadAtCommand(BG95_TypeDef * device, char* command)
{
	uint8_t com_buf[100];
	memset(com_buf,'\0',100);
	sprintf((char*)com_buf, "AT+%s?\r\n",command);
	uint16_t size = strlen((char*)com_buf); 				// we may move this inside uart transmit para

	HAL_StatusTypeDef retVal = HAL_UART_Transmit(device->handler, com_buf, size, 10);
	if (retVal != HAL_OK) return 1;
	else return 0;
}
*/

uint8_t sendWriteAtCommand(BG95_TypeDef * device, char* command, char* parameters)
{
	uint8_t com_buf[100];
	memset(com_buf,'\0',100);
	sprintf((char*)com_buf, "AT+%s=%s\r\n",command,parameters);
	uint16_t size = strlen((char*)com_buf); 				// we may move this inside uart transmit para

	HAL_StatusTypeDef retVal = HAL_UART_Transmit(device->handler, com_buf, size, 10);
	if (retVal != HAL_OK) return 1;
	else return 0;
}

uint8_t sendWriteReceiveAtCommand(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck)
{
	if (sendWriteAtCommand(device, command, parameters) == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		if (receiveAtResponse(device, rxBuf, startCheck, endCheck) != 2)
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x0A, 0x0D) != 1 )
			{			/*!< LF: 0x0A, CR: 0x0D 		*/
				if ((parBuf[0] == 'O') && (parBuf[1] == 'K')) return 0;
				 /*! ERROR received	*/
				else if ((parBuf[0] == 'E') && (parBuf[1] == 'R') && (parBuf[2] == 'R') && (parBuf[3] == 'O') && (parBuf[4] == 'R') ) return 5;
				else return 4;						/*!< Did not receive OK					*/
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t sendWriteReceiveAtCommandT(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout)
{
	if (sendWriteAtCommand(device, command, parameters) == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, timeout) != 2)
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset

			if (getBetween(rxBuf, parBuf, 0x0A, 0x0D) != 1 )
			{			/*!< LF: 0x0A, CR: 0x0D 		*/
				if ((parBuf[0] == 'O') && (parBuf[1] == 'K')) return 0;
				/*!< ERROR received	*/
				else if ((parBuf[0] == 'E') && (parBuf[1] == 'R') && (parBuf[2] == 'R') && (parBuf[3] == 'O') && (parBuf[4] == 'R') ) return 5;
				else								/*!< Did not receive OK					*/
				{
					//uint8_t size = strlen( (char*) parBuf );
					//for ( int i = 0; i<size; i++ ) device->invalidResponse[i] = parBuf[i];
					return 4;
				}
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t sendWriteReceiveAtCommandS(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck)
{
	if (sendWriteAtCommand(device, command, parameters) == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		if (receiveAtResponse(device, rxBuf, startCheck, endCheck) != 2)
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x0A, 0x20) != 1 )	/*!< LF: 0x0A, space: 0x20 		*/
			{
				if ((parBuf[0] == '>')) return 0;
				else return 4;						/*!< Did not receive OK					*/
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t send_plain_receive_at_command_timeout(BG95_TypeDef * device, char* command, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout)
{
	if (send_plain(device, command) == 0 )
	{
		uint8_t rxBuf[150];
		for(int i=0; i<150; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[149] = '\0';

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, timeout) != 2)
		{
			uint8_t parBuf[150];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<150; i++) parBuf[i]='~'; 					// we may change this as memset

			if (getBetween(rxBuf, parBuf, 0x0A, 0x20) != 1 )	/*!< LF: 0x0A, space: 0x20 		*/
			{
				if ((parBuf[0] == '>')) return 0;	// Will be changed...
				else return 4;						/*!< Did not receive OK					*/
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

void resetData(BG95_TypeDef * device)
{
	uint8_t resetVal = 82;

	device->csq.rssi= resetVal; // 57 is arbitrary default value for testing
	device->csq.ber = resetVal; // 0 is arbitrary default value for testing

	memset(device->qgmr,resetVal,50);
	device->qgmr[50] = 0x00;

	device->creg.n = resetVal;
	device->creg.stat = resetVal;
	memset(device->creg.lac,resetVal,4);
	memset(device->creg.ci,resetVal,4);
	device->creg.act = resetVal;

	device->cops.mode = resetVal;
	device->cops.format = resetVal;
	memset(device->cops.oper,resetVal,19);
	device->cops.oper[20] = 0x00;
	device->cops.act = resetVal;

	device->qiact.cId = resetVal;
	device->qiact.cState = resetVal;
	device->qiact.cType = resetVal;
	memset(device->qiact.ip,resetVal,39);
	device->qiact.ip[40] = 0x00;

	device->cfun = resetVal;

	device->SR &= 0x00000000;

	device->swState = 0;
}

uint8_t resetDevice(BG95_TypeDef * device)
{
	/*! Enable RX interrupts */
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART2_IRQn,0);				// Set interrupt priority
	NVIC_EnableIRQ(USART2_IRQn);					// Enable the USART2 interrupts on NVIC line

	resetData(device);
	device->RxData.size = BUFFER_SIZE;

	resetData(device);
	uint8_t response = 0;
	if ( ( response = sendWriteReceiveAtCommandT( device, "CFUN", "0", (uint8_t *) "AT+CFUN\0", (uint8_t *) "OK\r\n\0", 3000 ) ) == 0 )
	{
		if ((response = sendWriteReceiveAtCommandT(device, "CFUN", "1", (uint8_t *) "AT+CFUN\0", (uint8_t *) "OK\r\n\0", 1500)) == 0) return 0;
		else return (20 + response); 	//Could not turn on the device
	}
	else return (10 + response); 		//Could not disable the device
}

uint8_t initDevice(BG95_TypeDef * device)
{
	/*! Enable RX interrupts */
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART2_IRQn,0);				// Set interrupt priority
	NVIC_EnableIRQ(USART2_IRQn);					// Enable the USART2 interrupts on NVIC line

	resetData(device);
	device->RxData.size = BUFFER_SIZE;
	uint8_t response = 9; 							// 9 is arbitrary not to conflict.
	uint8_t count = 0;

	uint8_t startCheck[] = "AT\r\r\n\0";
	uint8_t endCheck[] = "OK\r\n\0";

	while ((response != 0) && (count<5))
	{
		response = sendReceiveCommandT(device, "AT", startCheck, endCheck, 500);
		count++;
	}
	if (response != 0) return (10 + response); 		/*!< Could not get at response. Device might be turned off 	*/
	else
	{
		if ((response = getCfun(device)) == 0)
		{
			if (device->cfun != 1)
			{
				count = 0;
				while((device->cfun != 1) && (count<5))			 // Attempt to set CFUN to 1
				{
					device->swState = 11;
					response = sendWriteReceiveAtCommandT(device, "CFUN", "1", (uint8_t *) "AT+CFUN\0", (uint8_t *) "OK\r\n\0", 1500);
					getCfun(device);
				}
				if(device->cfun == 1) return 0; 	/*!< Functionality of the device changed. 					*/
				else return (30 + response);		/*!< Func. cannot be changed							 	*/
			}
			else return 0;							/*!< Device is responsive and CFUN is set for full func. 	*/
		}
		else return (20 + response); 				/*!< Could not get cfun 									*/
	}
}

uint8_t getCfun(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "CFUN?") == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		//memset(rxBuf,'~',99);
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+CFUN?\r\r\n";
		uint8_t endCheck[] = "OK\r\n";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 500) != 2)
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 				// we may change this as memset
			//memset(parBuf,'~',50);
			if (getBetween(rxBuf, parBuf, 0x20, 0x0D) != 1 )
			{			/*!< space: 0x20, CR: 0x0D 		*/
				device->cfun = (uint8_t)((parBuf[0])-0x30);
				return 0;
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t getCsq(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "CSQ") == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		//memset(rxBuf,'~',99);
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+CSQ\r\r\n";
		uint8_t endCheck[] = "OK\r\n";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 350) != 2) /*!< Max. spec. time 300 ms */
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 				// we may change this as memset
			//memset(parBuf,'~',50);
			if (getBetween(rxBuf, parBuf, 0x20, 0x0D) != 1 )			/*!< space: 0x20, CR: 0x0D 		*/
			{
				if (parBuf[2] == ',')
				{
					/*! Double digit significant char converted to decimal 	*/
					device->csq.rssi = ((10*((uint8_t)((parBuf[0])-0x30)))+((uint8_t)(parBuf[1]-0x30)));
					device->csq.ber = (uint8_t)((parBuf[3])-0x30);
					return 0;
				}
				else
				{
					device->csq.rssi = (uint8_t)(parBuf[0]-0x30);
					device->csq.ber = (uint8_t)((parBuf[2])-0x30);
					return 0;
				}
			}
			else return 3;							/*!< Failed to get between AT command 	*/
		}
		else return 2;								/*!< Failed to receive AT command 		*/
	}
	else return 1; 									/*!< Failed to send AT command 			*/
}

uint8_t getQgmr(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "QGMR") == 0 )
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+QGMR\r\r\n";
		uint8_t endCheck[] = "OK\r\n";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 350) != 2) /*!< Max. spec. time 300 ms */
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x0A, 0x0D) != 1 )	/*!< LF: 0x0A, CR: 0x0D 			*/
			{
				uint8_t size = strlen((char*)parBuf);
				for (int i=0; i<size; i++) device->qgmr[i] = parBuf[i];
				device->qgmr[size] = 0x00;			/*!< Null terminator at the end of the array 	*/
				return 0;
			}
			else return 3;							/*!< Failed to get between AT command 			*/
		}
		else return 2;								/*!< Failed to receive AT command 				*/
	}
	else return 1; 									/*!< Failed to send AT command 					*/
}



uint8_t getCreg(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "CREG?") == 0 )					// change the at send function
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+CREG?\r\r\n\0";
		uint8_t endCheck[] = "OK\r\n\0";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 350) != 2) /*!< Max. spec. time 300 ms */
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x20, 0x0D) != 1 )	/*!< space: 0x20, CR: 0x0D 						*/
			{
				uint8_t size = strlen((char*)parBuf);
				uint8_t comma = 0;
				for (int i=0; i<size; i++)
				{
					if (parBuf[i] == ',') comma++;
				}
				if (comma == 1)
				{
					device->creg.n = (uint8_t)((parBuf[0])-0x30);
					device->creg.stat = (uint8_t)((parBuf[2])-0x30);
					return 0;
				}
				else if (comma == 4)
				{
					device->creg.n = (uint8_t)((parBuf[0])-0x30);
					device->creg.stat = (uint8_t)((parBuf[2])-0x30);
					for (int i=0;i<4;i++)
					{
						device->creg.lac[i] = parBuf[5+i]; // 2,1,"DATA","DATA",0
						device->creg.ci[i] = parBuf[12+i];
					}
					device->creg.lac[4] = 0x00;					/*!< Null terminator at the end of the array	*/
					device->creg.ci[4] = 0x00;					/*!< Null terminator at the end of the array 	*/
					device->creg.act = (uint8_t)((parBuf[18])-0x30);
					return 0;

				}
				else return 4; 									/*!< Unsupported result code.		 			*/
			}
			else return 3;										/*!< Failed to get between AT command 			*/
		}
		else return 2;											/*!< Failed to receive AT command 				*/
	}
	else return 1; 												/*!< Failed to send AT command 					*/
}

uint8_t getCops(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "COPS?") == 0 )					// change the at send function
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+COPS?\r\r\n\0";
		uint8_t endCheck[] = "OK\r\n\0";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 1500) != 2) /*!< Max. spec. time 180 s */
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x20, 0x0D) != 1 )	/*!< space: 0x20, CR: 0x0D 						*/
			{
				uint8_t size = strlen((char*)parBuf);
				uint8_t comma = 0;
				uint8_t secondDitto = 0;
				for (int i=0; i<size; i++)
				{
					if (parBuf[i] == ',') comma++;
					if ((secondDitto == 0) && (i>4))
					{
						for (int j=i; j<size;j++)
						{
							if (parBuf[i] == '"')
							{
								secondDitto=j;
								break;
							}
						}
					}
				}
				if (comma == 3)
				{
					device->cops.mode = (uint8_t)((parBuf[0])-0x30);
					device->cops.format = (uint8_t)((parBuf[2])-0x30);
					for (int i=5; i<secondDitto; i++) device->cops.oper[i-5] = parBuf[i];
					device->cops.oper[secondDitto-5] = 0x00;	/*!< Null terminator at the end of the array 	*/
					device->cops.act = (uint8_t)((parBuf[secondDitto+2])-0x30);
					return 0;
				}
				else return 4; 									/*!< Unsupported result code.		 			*/
			}
			else return 3;										/*!< Failed to get between AT command 			*/
		}
		else return 2;											/*!< Failed to receive AT command 				*/
	}
	else return 1; 												/*!< Failed to send AT command 					*/
}


uint8_t setTcpIpContext(BG95_TypeDef * device, uint8_t cId, uint8_t cType, char * Apn, char * name, char * pw, uint8_t aut)
{
	uint8_t parBuf[100];
	sprintf((char*)parBuf, "%d,%d,\"%s\",\"%s\",\"%s\",%d",cId,cType,Apn,name,pw,aut);
	return sendWriteReceiveAtCommandT(device, "QICSGP", (char*)parBuf, (uint8_t *) "AT+QICSGP\0", (uint8_t *) "OK\r\n", 350);  /*!< Max. spec. time 300 ms */
}

uint8_t getQiact(BG95_TypeDef * device)
{
	if (sendAtCommand(device, "QIACT?") == 0 )					// change the at send function
	{
		uint8_t rxBuf[100];
		for(int i=0; i<100; i++) rxBuf[i]='~'; 					// we may change this as memset
		rxBuf[99] = '\0';

		uint8_t startCheck[] = "AT+QIACT?\r\r\n";
		uint8_t endCheck[] = "OK\r\n";

		if (receiveAtResponseT(device, rxBuf, startCheck, endCheck, 1500) != 2)  /*!< Max. spec. time 150 s */
		{
			uint8_t parBuf[50];
			// fill up the buffer in order to remove the \0s
			for(int i=0; i<50; i++) parBuf[i]='~'; 					// we may change this as memset
			if (getBetween(rxBuf, parBuf, 0x20, 0x0D) != 1 )	/*!< space: 0x20, CR: 0x0D 						*/
			{
				uint8_t size = strlen((char*)parBuf);
				uint8_t comma = 0;
				uint8_t secondDitto = 0;
				for (int i=0; i<size; i++)
				{
					if (parBuf[i] == ',') comma++;
					if ((secondDitto == 0) && (i>6))
					{
						for (int j=i; j<size;j++)
						{
							if (parBuf[i] == '"')
							{
								secondDitto=j;
								break;
							}
						}
					}
				}
				if (comma == 3)
				{
					device->qiact.cId = (uint8_t)((parBuf[0])-0x30);
					device->qiact.cState = (uint8_t)((parBuf[2])-0x30);
					device->qiact.cType = (uint8_t)((parBuf[4])-0x30);
					for (int i=7; i<secondDitto; i++) device->qiact.ip[i-7] = parBuf[i];
					device->qiact.ip[secondDitto-7] = 0x00;		/*!< Null terminator at the end of the array 	*/
					return 0;
				}
				else return 4; 									/*!< Unsupported result code or more than one active PDP context.	*/
			}
			else												/*!< Failed to get between AT command 			*/
			{
				memset(device->qiact.ip,0,20);					// Reset the IP
				return 3;
			}
		}
		else return 2;											/*!< Failed to receive AT command 				*/
	}
	else return 1; 												/*!< Failed to send AT command 					*/
}

// in here we are only looking for OK response. Bu there is more. After OK we are expected to get "+QIOPEN=N1,N2" where N1 is connectID and N2 is the result.
// if N2 is 0 we get that service is opened successfully. So in the future we should check that too for a healthy operation.
uint8_t openSocketService(BG95_TypeDef * device, uint8_t cId, uint8_t connectId, char * sType, char * ip, uint8_t rPort, uint8_t lPort, uint8_t aMode)
{
	uint8_t parBuf[100];
	sprintf((char*)parBuf, "%d,%d,\"%s\",\"%s\",%d,%d,%d",cId,connectId,sType,ip,rPort,lPort,aMode);
	/*! Max. spec. time 150 s 	*/
	return sendWriteReceiveAtCommandT(device, "QIOPEN", (char*)parBuf, (uint8_t *) "AT+QIOPEN\0", (uint8_t *) "+QIOPEN: 0,0\r\n\0", 3000);
}

uint8_t sendSocket(BG95_TypeDef * device, uint8_t connectId, uint8_t data)
{
	// check for receiving ">"
	// then send the data, and maybe wait for the response
	uint8_t response = 9;
	char test[1] = "0";						// Will be changed...

	/*! Max. spec. time 150 s 	*/
	if ((response = sendWriteReceiveAtCommandS(device, "QISEND", test, (uint8_t *) "AT+QISEND\0", (uint8_t *) "> \0")) == 0)	// Will be changed...
	{
		// send data here
		uint8_t package[100];
		memset(package,0,100);
		//  GET https://api.thingspeak.com/update?api_key=3XX0ILLFNZCYAQNS&field1=%d\x1a
		/*! 0x1A needed to terminate the command */
		sprintf((char*)package,"GET https://api.thingspeak.com/update?api_key=UZ0ALREJ46G1IZED&field1=%d\r\n\x1a",data);

		//send_plain(device, (char*)package);

		 // We do not check response for now...
		//send_plain_receive_at_command_timeout(device, (char*)package, (uint8_t *) "GET \0", (uint8_t *) "+QIURC: \"closed\",0\r\n\0", 5000);
		uint8_t testRet = 0;
		testRet = send_plain_receive_at_command_timeout(device, (char*)package, (uint8_t *) "GET \0", (uint8_t *) ",0\r\n\0", 5000);
		//HAL_Delay(5000);
		return (0 + testRet);
	}
	else return response;
}

uint8_t app_Error_Handler ( BG95_TypeDef * device, uint8_t * resultCode )
{

	switch (device->swState)
	{
	case 0:
		while (1); // For now...
		break;
	case 10:
		while (1); // For now...
		break;
	case 11:									/*!< AT+CFUN=1 step in the sw state */
		if ( *resultCode%10 == 5 )				/*!< ERROR response 			*/
		{
			//stuff
		}
		else if ( *resultCode%10 == 4 )			/*!< Undefined response 			*/
		{
			//stuff
		}
		else if ( *resultCode%10 == 2 )			/*!< No data gathered 				*/
		{
			//stuff
		}
		else									/*!< Not expected to enter here 	*/
		{
			//stuff
		}
		break;
	case 20:
		//statements
		break;
	case 30:									/*!< CSQ error handling				*/

		if ( *resultCode != 0)
		{
			while (1); // Will be changed...
		}
		HAL_Delay(500); // Delay for device to get signal if not got it
		*resultCode = getCsq(device);

		if ( (*resultCode != 0) || (device->csq.rssi == 0x52) || (device->csq.rssi == 99) )		/*!< 0x52: R : default app. reset value */
		{
		  app_Error_Handler( device, resultCode );		// AT+CSQ may response two different ERRORs
		}

		break;
	case 40:
		//statements
		break;
	case 50:
		//statements
		break;
	case 60:
		//statements
		break;
	case 63:									/*!< Socket application function 	*/

		// AT+QIDEACT=1 	// Deactivate the PDP context with id 1 to be sure if the pdp context closed properly
		// AT+QIACT=1		// Open the socket
		// AT+QIACT?		// Check again
		// do the above two steps in loop
		// if success within timing return 0 o.w. 1

		device->swState = 64;
		sendWriteReceiveAtCommandT(device, "QIDEACT", "1", (uint8_t *) "AT+QIDEACT\0", (uint8_t *) "OK\r\n\0", 2000);
		sendWriteReceiveAtCommandT(device, "QIACT", "1", (uint8_t *) "AT+QIACT\0", (uint8_t *) "OK\r\n\0", 2000);

		getQiact(device);
		uint8_t size = strlen((char*)device->qiact.ip);
		uint8_t count = 0;
		for (int i=0; i<size; i++) if(device->qiact.ip[i] == 0x2E) count++; // 0x2E: .

		if( ( count != 3 ) || ( device->qiact.ip[0] == 0 ) || ( device->qiact.cState != 1 ) )
		{
			app_Error_Handler( device, NULL );				// IP may be unvalid or connection may not be established
		}
		break;
	case 64:									/*!< Will enter if case 63 failed. 	*/
		while (1); // For now...
		break;
	case 70:
		//statements
		break;
	}

	return 0;
}
