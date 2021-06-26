/*
 * counter.h
 *
 *  Created on: Dec 10, 2019
 *      Author: Miguelito
 */

#ifndef COUNTER_H_
#define COUNTER_H_

//#include <stdlib.h>
#include <wait.h>

/** @defgroup COUNTER Counter
 * This package provides the core capabilities to use time counters.
 * @{
 */



/**
 * @brief	Data structure contains time in milliseconds.
 */
typedef struct _Counter {
	uint32_t start;
} Counter;

/**
 * @brief	Creates a counter structure whose memory is allocated through malloc.
 * @return	Returns a pointer to the created counter.
 * @note	Always destroy created counters after used.
 */
Counter * COUNTER_Create(void);

/**
 * @brief	The counter time is restarted with current time.
 * @param	p: pointer to counter to be restarted.
 */
void COUNTER_Restart( Counter * p );

/**
 * @brief	Checks whether a certain time has passed since the counter start.
 * @param	p: pointer to counter to be restarted.
 * @param	time: Time to check for in milliseconds.
 * @return	The difference in milliseconds between current time and since the counter started + "time".
 */
int COUNTER_Check( Counter * p, int time); //retorna os milissegundos passados depois de time desde __START

/**
* @brief	Frees memory allocated by COUNTER_Create.
*/
void COUNTER_Destroy( Counter * p );

/**
 * @}
 */


#endif /* COUNTER_H_ */
