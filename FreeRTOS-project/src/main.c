
//#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* Kernel includes. */
#include "FreeRTOS.h"


/* Kernel includes. */
#include "task.h"

#include "LPC17xx.h"
#include <cr_section_macros.h>

#include <led.h>
#include <wait.h>
#include <button.h>
#include <LCD.h>
#include <RTC.h>
#include <RTCUI.h>
#include <BMP280.h>
#include <BMP280UI.h>
#include <counter.h>

#include <project.h>

#include <eeprom.h>
#include <ESP8266.h>
#include <ntp.h>



void * malloc (size_t size) {
	// Call the FreeRTOS version of malloc.
	return pvPortMalloc( size );
}
void free (void* ptr)
{
	// Call the FreeRTOS version of free.
	vPortFree( ptr );
}
//int t = 750;


int main(void)
{
	//printf("hi\n");


	TaskHandle_t xProjectHandle;
	if( xTaskCreate(vProjectTaskMain, "Project task", configMINIMAL_STACK_SIZE * 3, NULL, 3, &xProjectHandle) != pdPASS ) {
		//printf("Oopsy_project");
	}

	vTaskStartScheduler();

	for(;;);
	return 0;
}


/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vConfigureTimerForRunTimeStats( void )
{
const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */

	/* Power up and feed the timer. */
	LPC_SC->PCONP |= (1 << 2);
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3 << 4))) | (0x01 << 4);

	/* Reset Timer 1 */
	LPC_TIM1->TCR = TCR_COUNT_RESET;

	/* Just count up. */
	LPC_TIM1->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	LPC_TIM1->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

	/* Start the counter. */
	LPC_TIM1->TCR = TCR_COUNT_ENABLE;
}
/*-----------------------------------------------------------*/



