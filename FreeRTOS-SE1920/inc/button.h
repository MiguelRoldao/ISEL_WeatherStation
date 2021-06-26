/*
 * button.h
 *
 *  Created on: 08/10/2019
 *      Author: MC
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//#include <stdio.h>
//#include <stdbool.h>
#include <counter.h>



/**	@defgroup BUTTON Button
 * 	This package provides the core capabilities of three momentary push buttons
 * 	in ports P0.23, P0.24 and P0.25 of the LPC1769 from NXP.
 * 	@{
 */

/**
 * @brief	Number of buttons.
 */
#define BUTTON_N 3
/**
 * @brief	Button connected to P0.25.
 */
#define BUTTON_ENTER 4
/**
 * @brief	Button connected to P0.24.
 */
#define BUTTON_DOWN 2
/**
 * @brief	Button connected to P0.23.
 */
#define BUTTON_UP 1



/**
 * @brief	Initializes the button API with pull up resistors.
 * @note	This function must be called prior to any other BUTTON functions.
 */
void BUTTON_Init( void );

/**
 * @brief	Gets bitmap of pressed buttons
 * @return	Bitmap is 3bits (BUTTON_ENTER BUTTON_UP BUTTON_DOWN).
 */
int BUTTON_Hit(void);

/**
 * @brief	Waits for a button to be pressed.
 * @return	Bitmap is 3bits (BUTTON_ENTER BUTTON_UP BUTTON_DOWN).
 */
int BUTTON_Read(void);

/**
 * @brief	Gets bitmap of pressed buttons over time.
 * @return	Bitmap is 6bits (A1 B1 C1 A0 B0 C0).
 * A -> BUTTON_ENTER, B -> BUTTON_UP, C -> BUTTON_DOWN.
 * 0 -> Newly read value, 1 -> Old read value.
 */
int BUTTON_GetButtonsEvents(void);

/**
 * @brief	Checks if a specific button combination is pressed for more than a certain amount of milliseconds, using its own Counter.
 * @param	comb: Combination of buttons. Bitmap is 3bits (BUTTON_ENTER BUTTON_UP BUTTON_DOWN).
 * @param	ms: Number of milliseconds the button combination has to be pressed.
 * @return	Returns 1 if the button combination was pressed for ms milliseconds or more. Returns 0 otherwise.
 */
int BUTTON_CombTimer(int comb, int ms );


/**
 * @}
 */

#endif /* BUTTON_H_ */
