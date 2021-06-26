/*
 * led.h
 *
 *  Created on: 03/10/2019
 *      Author: MC
 */

#ifndef LED_H_
#define LED_H_

#include <stdbool.h>
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

/** @defgroup LED LED
 * This package provides the core capabilities such as on / off the LED in
 * port P0.22 of the LPC1769 from NXP.
 * @{
 */


/**
 * @brief	Initializes the LED API.
 * @param	state: State of the LED. The 0 indicate led off and 1 LED on.
 * @return	Nothing.
 * @note	This function must be called prior to any other LED functions. The
 * LED will be started with the value passed in state.
 */
void LED_Init( bool state );

/**
 * @brief	Get LED state.
 * @return	Status of LED. The 0 indicate led is off and 1 LED is on.
 */
bool LED_GetState( void );

/**
 * @brief	Turn LED on.
 * @return	Nothing.
 */
void LED_On( void );

/**
 * @brief	Turn LED off.
 * @return	Nothing.
 */
void LED_Off( void );

/**
 * @}
 */

#endif /* LED_H_ */
