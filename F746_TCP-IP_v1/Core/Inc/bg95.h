/**
 * @file	bg95.h
 * @author	Furkan KARAGOZ
 * @date	22/03/2022
 * @brief	Header file of BG95 module driver.
 *
 * @todo	AT+QIOPEN=... 	(DONE, PARTIAL CONTROL)
 * 			AT+QISTATE=... 	(TO DO)
 * 			AT+QISEND=...	(DONE)
 * 			AT+QICLOSE=...	(TO DO)
 *
 * @note	About the contextID and connectID. Context is related to PDP definition and contextID is for the ongoing PDP context.
 * 			Whereas, connectID is for an ongoing TCP-IP connection.
 *
 */

#ifndef BG95_H_
#define BG95_H_

#include "stm32f7xx_hal.h"
#include <string.h>	//for strlen()
#include <stdio.h>  // for sprintf()
//#include <stdlib.h> // for atoi()
#include "App.h"

/**
 * @brief CSQ command variables.
 *
 */
typedef struct {
	uint8_t rssi;	/*!< Received signal strength level.		*/
	uint8_t ber;	/*!< Channel bit error rate.				*/
}CSQ_TypeDef;

/**
 * @brief CREG output variables.
 *
 */
typedef struct {
	uint8_t n;		/*!< Network registration result code setting.						*/
	uint8_t stat;	/*!< Registration status.											*/
	uint8_t lac[5];	/*!< Location area code. 2 byte hex.								*/
	uint8_t ci[5];	/*!< GERAN/E-UTRAN cell ID. 4 byte hex.								*/
	uint8_t act;	/*!< Access technology of the serving cell. 0:GSM 8:eMTC 9:NB-IoT	*/
}CREG_TypeDef;

/**
 * @brief CREG output variables.
 *
 */
typedef struct {
	uint8_t mode;		/*!< Operator selection mode eg. automatic, manual etc.				*/
	uint8_t format;		/*!< Operator name format type.										*/
	uint8_t oper[20];	/*!< Operator name.													*/
	uint8_t act;		/*!< Access technology of the serving cell. 0:GSM 8:eMTC 9:NB-IoT	*/
}COPS_TypeDef;

/**
 * @brief CREG output variables.
 *
 */
typedef struct {
	uint8_t cId;		/*!< Context ID.									*/
	uint8_t cState;		/*!< Context state.									*/
	uint8_t cType;		/*!< Protocol type.									*/
	uint8_t ip[40];		/*!< Local IP address after context is activated.	*/
}QIACT_TypeDef;

/**
 * @brief BG95_TypeDef stores data related to the module operation.
 *
 */
typedef struct {
	rxData_TypeDef RxData;
	UART_HandleTypeDef * handler; 	/*!< Pointer to the UART handler that device is connected.		*/
	CSQ_TypeDef csq;				/*!< CSQ variables. RSSI and ber.								*/
	uint8_t qgmr[50];				/*!< Software version of the device.							*/
	CREG_TypeDef creg;				/*!< CREG variables. n, stat, lac, ci, and act.					*/
	COPS_TypeDef cops;				/*!< COPS variables. mode, format, oper, act.					*/
	QIACT_TypeDef qiact;			/*!< QIACT variables. 											*/
	uint8_t cfun;					/*!< Functionality level of UE. 								*/
	uint32_t SR;					/*!< Status Register of application. 							*/
	uint8_t swState;				/*!< Application software state 		 						*/
}BG95_TypeDef;

/**
 * @brief Used for collecting received UART data to a buffer.
 * 			Fills up the buffer with '~' first in order to detect the end of the received data package.
 *
 * @param 	device 		Pointer to the BG95 device object.
 * @param 	buffer		Pointer to a buffer to be filled.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @return 	0: Success
 * 			1: HAL_TIMEOUT. Whole or some data may be received. Also, this may occur due to high buffer size.
 * 			2: UART fails to receive data.
 */
uint8_t receiveAtResponse(BG95_TypeDef * device, uint8_t * buffer, uint8_t * startCheck, uint8_t * endCheck);

/**
 * @brief Used for collecting received UART data to a buffer.
 * 			Fills up the buffer with '~' first in order to detect the end of the received data package.
 *
 * @param 	device		Pointer to the BG95 device object.
 * @param 	buffer		Pointer to a buffer to be filled.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @param 	timeout		Timeout for UART Rx operation in ms.
 * @return	0: Success
 * 			1: HAL_TIMEOUT. Whole or some data may be received. Also, this may occur due to high buffer size.
 * 			2: UART fails to receive data.
 */
uint8_t receiveAtResponseT(BG95_TypeDef * device, uint8_t * buffer, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout);

/**
 * @brief When specified character encountered in an char array data collection starts.
 * 			If end character detects after the start character, all the characters between two
 * 			copied into another buffer.
 *
 * @param sourceBuf Source buffer to be inspected.
 * @param targetBuf	Target buffer that will be filled with the data collected.
 * @param firstChar First character specification that starts the data collection.
 * @param secondChar Last character specified that ends the data field.
 * @return 	0: Success
 * 			1: No start char detected
 * 			2: No end character detected. But the data is still collected.
 */
uint8_t getBetween(uint8_t * sourceBuf, uint8_t * targetBuf, uint8_t firstChar, uint8_t secondChar);

/**
 * @brief Sends command that needs to be terminated with CR LF.
 *
 * @param device	Pointer to the BG95 device object.
 * @param command	Pointer to the command char array.
 * @return	0: Success
 * 			1: UART fails to send the command. HAL_OK did not received.
 */
uint8_t sendCommand(BG95_TypeDef * device, char* command);

/**
 * @brief Sends plain text through UART.
 *
 * @param device	Pointer to the BG95 device object.
 * @param command	Pointer to the command char array.
 * @return	0: Success
 * 			1: UART fails to send the command. HAL_OK did not received.
 */
uint8_t sendPlain(BG95_TypeDef * device, char* command);

/**
 * @brief	Sends command through UART line and receives the response.
 *
 * @param device		Pointer to the BG95 device object.
 * @param command		Pointer to the command char array.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @param timeout		Timeout for UART RX.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive OK message.
 * 			5: ERROR received.
 */
uint8_t sendReceiveCommandT(BG95_TypeDef * device, char* command, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout);

/**
 * @brief 	Sends AT+command type EXECUTION command through UART line
 * 			also the command is ended with CR LF
 *
 * @param device	Pointer to the BG95 device object.
 * @param command	Pointer to the command char array.
 * @return 	0: Success
 * 			1: UART fails to send the command. HAL_OK did not received.
 */
uint8_t sendAtCommand(BG95_TypeDef * device, char* command);

/**
 * @brief 	Sends AT+command? type READ command through UART line
 * 			also the command is ended with CR LF
 *
 * @param device	Pointer to the BG95 device object.
 * @param command	Pointer to the command char array.
 * @return	0: Success
 * 			1: UART fails to send the command. HAL_OK did not received.
 */
uint8_t sendReadAtCommand(BG95_TypeDef * device, char* command);

/**
 * @brief 	Sends AT+command=parameters type READ command through UART line
 * 			also the command is ended with CR LF
 *
 * @param device		Pointer to the BG95 device object.
 * @param command		Pointer to the command char array.
 * @param parameters	Pointer to the parameters char array.
 * @return	0: Success
 * 			1: UART fails to send the command. HAL_OK did not received.
 */
uint8_t sendWriteAtCommand(BG95_TypeDef * device, char* command, char* parameters);

/**
 * @brief	Sends AT+command=parameters type READ command through UART line
 * 			also the command is ended with CR LF
 * 			also receives the response and looks for 'OK'.
 *
 * @param device		Pointer to the BG95 device object.
 * @param command		Pointer to the command char array.
 * @param parameters	Pointer to the parameters char array.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive OK message.
 * 			5: ERROR received.
 */
uint8_t sendWriteReceiveAtCommand(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck);

/**
 * @brief	Sends AT+command=parameters type READ command through UART line
 * 			also the command is ended with CR LF
 * 			also receives the response and looks for 'OK'.
 * 			also receives with a timeout
 *
 * @param 	device		Pointer to the BG95 device object.
 * @param 	command		Pointer to the command char array.
 * @param 	parameters	Pointer to the parameters char array.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @param 	timeout		Timeout for UART RX line in ms.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive OK message.
 * 			5: ERROR received.
 */
uint8_t sendWriteReceiveAtCommandT(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck, uint32_t timeout);

/**
 * @brief 	Sends AT+command=parameters type READ command through UART line
 * 			also the command is ended with CR LF
 * 			also receives the response and looks for '>'.
 *
 * @param 	device		Pointer to the BG95 device object.
 * @param 	command		Pointer to the command char array.
 * @param 	parameters	Pointer to the parameters char array.
 * @param 	startCheck	Start string needs to be detected for validation
 * @param 	endCheck	End string needs to be detected for validation
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive valid response character.
 */
uint8_t sendWriteReceiveAtCommandS(BG95_TypeDef * device, char* command, char* parameters, uint8_t * startCheck, uint8_t * endCheck);

/**
 * @brief Resets data fields of the BG95 object.
 *
 * @param device Device GSM device struct address.
 */
void resetData(BG95_TypeDef * device);

/**
 * @brief Turns off and on the functionality of the device.
 *
 * @param device Device GSM device struct address.
 * @return	0: Success
 * 			1: Turn off response fail.
 * 			2: Turn on response fail.
 */
uint8_t resetDevice(BG95_TypeDef * device);

/**
 * @brief Initializes the device.
 *
 * @param device 	Device GSM device struct address.
 * @return	0: Success
 * 							11: Failed to send AT command 			#AT		21: Failed to send AT command. 			#CFUN		31: #CFUN=1
 * 							12: Failed to receive AT command		#AT		22: Failed to receive AT command. 		#CFUN		32: #CFUN=1
 * 							13: Failed to get between AT command	#AT		23: Failed to get between AT command.  	#CFUN		33: #CFUN=1
 * 							14: Did not receive OK message.			#AT															34: #CFUN=1
 * 							15: ERROR received.						#AT															35: #CFUN=1
 */
uint8_t initDevice(BG95_TypeDef * device);

/**
 * @brief Gets functionality level of the device.
 *
 * @param device 	Device GSM device struct address.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 */
uint8_t getCfun(BG95_TypeDef * device);

/**
 * @brief 	Gets the RSSI of the device.
 *
 * @param 	device	Device GSM device struct address.
 * @return 	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 */
uint8_t getCsq(BG95_TypeDef * device);

/**
 * @brief 	Gets software model of the device.
 *
 * @param 	device	Device GSM device struct address.
 * @return  0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 */
uint8_t getQgmr(BG95_TypeDef * device);

/**
 * @brief 	Gets Network registration status of the device.
 *
 * @param 	device	Device GSM device struct address.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Invalid received data format
 */
uint8_t getCreg(BG95_TypeDef * device);

/**
 * @brief  	Gets the selected operator name.
 *
 * @param 	device 	Device GSM device struct address.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Invalid received data format
 */
uint8_t getCops(BG95_TypeDef * device);

/**
 * @brief 	Configures TCP-IP context.
 *
 * @param 	device	Device GSM device struct address.
 * @param 	cId		Context ID. Range 1-16.
 * @param 	cType	Protocol type.
 * @param 	Apn		Access point name
 * @param 	name	Username
 * @param 	pw		Password
 * @param 	aut		Authentication methods
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive OK message.
 */
uint8_t setTcpIpContext(BG95_TypeDef * device, uint8_t cId, uint8_t cType, char * Apn, char * name, char * pw, uint8_t aut);

/**
 * @brief 	Gets active PDP contexts' IP address'.
 *
 * @param 	device	Device GSM device struct address.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Unsupported result code or more than one active PDP context.
 */
uint8_t getQiact(BG95_TypeDef * device);
/**
 * @brief	Opens a socket service.
 *
 * @param 	device		Device GSM device struct address.
 * @param 	cId			Context ID. 1-16
 * @param 	connectId	Socket service index. 0-11
 * @param 	sType		The socket service type.
 * @param	ip			Domain name address of the remote server.
 * @param 	rPort		Port number of the remote server.
 * @param 	lPort		Local port number.
 * @param 	aMode		Data access mode of the socket service.
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Unsupported result code.
 */
uint8_t openSocketService(BG95_TypeDef * device, uint8_t cId, uint8_t connectId, char * sType, char * ip, uint8_t rPort, uint8_t lPort, uint8_t aMode);

/**
 * @brief	Sends data through previously opened socket service.
 *
 * @details	We are sending data as plain without CR LF terminations.
 * 			Otherwise the message context will not be interpreted correctly.
 *
 * @param device	Device GSM device struct address.
 * @param connectId	Socket connect id
 * @param data		Data to be sent to the TS field
 * @return	0: Success
 * 			1: Failed to send AT command
 * 			2: Failed to receive AT command
 * 			3: Failed to get between AT command
 * 			4: Did not receive valid response character.
 */
uint8_t sendSocket(BG95_TypeDef * device, uint8_t connectId, uint8_t data);

/**
 * @brief 	Error handler for application function.
 * @details	These errors are due to the un-validated data.
 * @todo	Alter the receive functions in order to not to discard the unvalidated data fields.
 * 			Switch case for error handler needed sw states.
 *
 * @return	Will be defined...
 */
uint8_t app_Error_Handler (uint8_t * resultCode);


#endif /* BG95_H_ */
