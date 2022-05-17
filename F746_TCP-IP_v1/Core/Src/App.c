#include "App.h"


uint8_t validateData ( rxData_TypeDef * rxData, uint8_t * startString, uint8_t * endString )
{
	uint8_t resultCode = 9;
	uint8_t startS_size = strlen( (char*) startString);
	uint8_t endS_size = strlen( (char*) endString);

	if ( rxData->head > rxData->tail ) 										/* DEFAULT READ													*/
	{
		if ( rxData->head - rxData->tail < startS_size + endS_size )		/* The received data is too short to be checked					*/
		{
			return 19;
		}

		// CHECK FOR VALIDATION AND UPDATE RESULTCODE ACCORDINGLY
		for ( int i = rxData->tail; i < (rxData->tail + startS_size); i++ )
		{		// Validate the start string
			if ( rxData->buffer[i] == startString[ i-rxData->tail ] )
			{
				resultCode = 0;
			}
			else return 18;
		}
		if ( resultCode == 0 )												/* If the start string validated check the end string as well.	*/
		{
			for ( int i = (rxData->head - endS_size); i<rxData->head; i++ )
			{
				if ( rxData->buffer[i] == endString[i-rxData->head + endS_size] )
				{
					resultCode = 0;
				}
				else return 17;
			}
		}
		return ( 10 + resultCode );
	}
	else if ( rxData->head < rxData->tail )									/* WRAPPING READ - Buffer looped to the beginning				*/
	{
		if ( rxData->head + (rxData->size - rxData->tail) < startS_size + endS_size ) /* The data is too short to be checked				*/
		{
			return 29;
		}

		// CHECK FOR VALIDATION AND UPDATE RESULTCODE ACCORDINGLY
		uint8_t tempB_size = rxData->size - rxData->tail + rxData->head;
		//uint8_t tempBuffer[tempB_size] = 0;
		uint8_t tempBuffer[BUFFER_SIZE];									/* Temporary buffer to order the looping received data filed	*/

		for ( int i = 0; i < tempB_size; i++ )								/* Fill the temp. buffer										*/
		{
			if ( i + rxData->tail < rxData->size )
			{
				tempBuffer[i] = rxData->buffer[i + rxData->tail];
			}
			else
			{
				tempBuffer[i] = rxData->buffer[ i - (rxData->size - rxData->tail) ];
			}
		}
		for ( int i=0; i < startS_size; i++ )								/* Validate the start string									*/
		{
			if ( tempBuffer[i] == startString[i] )
			{
				resultCode = 0;
			}
			else return 28;
		}
		if ( resultCode == 0 )												/* If the start string validated check the end string as well	*/
		{
			for ( int i = (tempB_size - endS_size); i < tempB_size; i++ )
			{
				if ( tempBuffer[i] == endString[ i - (tempB_size - endS_size) ] )
				{
					resultCode = 0;
				}
				else return 27;
			}
		}
		return ( 20 + resultCode );
	}
	else
	{																		/* NO READ. Head and tail point to same position - No new data.	*/
		return ( 30 + resultCode );
	}
}

uint8_t readData ( rxData_TypeDef * rxData, uint8_t * buffer )
{
	if ( rxData->head > rxData->tail ) 										/* Default read.												*/
	{
		for ( int i = rxData->tail; i < rxData->head; i++ )
		{
			buffer[ i - rxData->tail ] = rxData->buffer[i];
		}
		buffer[ rxData->head - rxData->tail ] = 0; 							/* Null char for termination									*/
		rxData->tail = rxData->head;										/* Data read process completed move tail to head				*/
		return 0;
	}
	else if ( rxData->head < rxData->tail )									/* Wrapping read.												*/
	{
		uint8_t charCount = 0;

		for ( int i = rxData->tail; i < rxData->size; i++ )
		{
			buffer[ i - rxData->tail ] = rxData->buffer[i];
			charCount++;
		}
		for ( int i = 0; i < rxData->head; i++ )
		{
			buffer[ charCount + i ] = rxData->buffer[i];
		}
		buffer[ charCount + rxData->head ] = 0;								/* Termination													*/
		rxData->tail = rxData->head;										/* Data read process completed move tail to head				*/

		return 1;
	}
	else return 2; 															/* Unexpected													*/
}



