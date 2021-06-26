#ifndef __WAIT_H__
#define __WAIT_H__

//#include <stdint.h>
//#include <stdio.h>

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

/* Kernel includes. */
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <semphr.h>


#include <cr_section_macros.h>

/** @defgroup WAIT Wait
 * This package provides the core capabilities for wait functions.
 * @{
 */

/**
 * @brief	Defines the SysTick interrupt function.
 * @return	Nothing.
 */
void SysTick_Handler( void );

/**
 * @brief	Initializes the wait API for 1 ms resolution.
 * @return	0 if initialization succeeded; 1 if fails.
 * @note	This function must be called prior to any other WAIT functions.
 */
int WAIT_Init(void);

/**
 * @brief	Waits a number of milliseconds.
 * @param	time: the whole number of milliseconds to wait.
 */
void WAIT_Ms( int time );


/**
 * @brief	Returns current system clock (in ms)
 * @return	Value of the local variable __TS.
 */
uint32_t WAIT_Elapsed(void);

/**
 * @brief	Initializes timer 0 for WAIT_ChronoUs.
 * @note	This function must be called prior to WAIT_ChronoUs to ensure
 * proper behavior.
 */
void WAIT_TIM0Init(void);

/**
 * @brief	Waits a number of microseconds. Utilizes timer 0.
 * @param	waitUs: the whole number of microseconds to wait.
 */
void WAIT_ChronoUs(int waitUs);

/**
 * @}
 */

#endif /* __WAIT_H__ */
