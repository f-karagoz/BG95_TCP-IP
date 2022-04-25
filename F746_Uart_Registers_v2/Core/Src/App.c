#include "App.h"


uint8_t validateData(rxData_TypeDef * rxData, uint8_t startChar, uint8_t endChar) {
	// For char based validation this process can be shorter. We do not need to check if head overlapped.
	// If we desire sting based validation for start and end detection this method is ok but need to be updated for string.
	uint8_t resultCode = 9;
	if (rxData->head > rxData->tail) { 		// Default read
		// CHECK FOR VALIDATION AND UPDATE RESULTCODE ACCORDINGLY
		if (rxData->buffer[rxData->tail] == startChar) { // Start char found correctly
			resultCode = 1;
			if (rxData->buffer[rxData->head-1] == endChar) {
				resultCode = 0;
			}
		}
		return (10 + resultCode);
	}
	else if (rxData->head < rxData->tail) {	// Buffer completed its ring and started writing from start
		// CHECK FOR VALIDATION AND UPDATE RESULTCODE ACCORDINGLY
		if (rxData->buffer[rxData->tail] == startChar) { // Start char found correctly
			resultCode = 1;
			if (rxData->buffer[rxData->head-1] == endChar) {
				resultCode = 0;
			}
		}
		return (20 + resultCode);
	}
	else {									// head and tail point to same position, no new data
		return (30 + resultCode);
	}
}

uint8_t readData(rxData_TypeDef * rxData, uint8_t * buffer) {
	if (rxData->head > rxData->tail) { 				// Default read.
		for(int i = rxData->tail; i<rxData->head; i++) {
			buffer[i-rxData->tail] = rxData->buffer[i];
		}
		buffer[rxData->head - rxData->tail] = 0; 	// Null char for termination
		rxData->tail = rxData->head;				// Data read process completed move tail to head
		return 0;
	}
	else if (rxData->head < rxData->tail) {			// Looping read. Buffer completed its ring and started writing from start.
		uint8_t charCount=0;
		for (int i = rxData->tail; i<rxData->size; i++) {
			buffer[i-rxData->tail] = rxData->buffer[i];
			charCount++;
		}
		for (int i = 0; i<rxData->head; i++) {
			buffer[charCount+i] = rxData->buffer[i];
		}
		// Termination here
		buffer[charCount+rxData->head] = 0;			// Termination
		rxData->tail = rxData->head;				// Data read process completed move tail to head
		return 1;
	}
	else return 2; // Unexpected
}

/**
 *  @USAGE

while (( validateData(...) % 10 ) != 0);

readData(&rxData, myBuffer);

*/



