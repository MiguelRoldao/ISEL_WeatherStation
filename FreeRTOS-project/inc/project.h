/*
 * project.h
 *
 *  Created on: Jul 8, 2020
 *      Author: Miguelito
 */

#ifndef PROJECT_H_
#define PROJECT_H_

//#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* Kernel includes. */
#include "FreeRTOS.h"
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

#include <eeprom.h>
#include <ESP8266.h>
#include <ntp.h>

#include "wifi_credentials.h"
#include "temppub.h"

/**	\mainpage
 *
 * 	\section intro_sec Introduction
 *
 * 	This project was realized for the course of SE2.
 * 	It consists of an autonomous system based on the
 *	FreeRTOS operating system, that implements a meteorological station for monitoring the
 *	ambient temperature and atmospheric pressure values. with the possibility of sending this data
 *	to the Ethernet.\n\n
 *
 * 	\section image_sec Images
 *
 *
 *	\image html Project_Modules_Diagram.png "Image 1 - Layer Block Diagram"
 *	\n
 *
 *	\image html Project_Schematic.png "Image 2 - Project Hardware Schematic"
 *	\n\n
 *
 *	\image html Project_ASM_Chart.png "Image 3 - Project ASM charts"
 *	\n
 *
 *
 *
 * 	\section freertos_sec FreeRTOS
 *
 * 	FreeRTOS modules used:
 * 	 - FreeRTOS.h\n
 * 	 - semphr.h\n
 * 	 - task.h\n
 * 	 - timers.h
 *
 *	Macros added to the FreeRTOSConfig.h:
 *	\code{.cpp}
 *	#define configUSE_TIMERS					1
 *	#define configTIMER_TASK_PRIORITY			( 4 )
 *	#define configTIMER_QUEUE_LENGTH			( 100 )
 *	#define configTIMER_TASK_STACK_DEPTH		( configMINIMAL_STACK_SIZE * 5 )
 *	#define configSUPPORT_DYNAMIC_ALLOCATION	1
 *	\endcode
 *	\code{.cpp}
 *	#define INCLUDE_xTaskGetCurrentTaskHandle	1
 *	#define INCLUDE_eTaskGetState				1
 *	\endcode
 */

/** @defgroup PROJECT Project
 * This package contains functions developed specifically for the final project.
 * @{
 */

/**
 * @brief	Time in milliseconds between showing date and time and showing temperature and pressure.
 */
#define IDLE_VIEW_TIME	10000

/**
 * @brief	Time in milliseconds to refresh the time in the LCD.
 */
#define REFRESH_TIME	900

/**
 * @brief	Data structure to keep temperature, pressure and time.
 */
typedef struct {
	double temperature;
	double pressure;
	struct tm time;
} PROJECT_Data_t;

/**
 * @brief	Task function to blink the built in LED.
 * @param	vpParameters: Pointer to blink time in ms.
 */
void 	vProjectTaskBlinkLED	(void * vpParameters);

/**
 * @brief	This task function takes care of all wireless communications. Connecting to an AP, getting NTP time and start the MQTT capabilities.
 * @param	vpParameters: Pointer to a flag, that is set once the wireless setup is complete.
 */
void	vProjectTaskWifi		(void * vpParameters);

/**
 * @brief	Main task of the project.
 * @param	vpParameters: Unsused.
 * @note	Create this task in the main function, to run the whole project.
 */
void	vProjectTaskMain		(void * vpParameters);

/**
 * @brief	Initializes all modules require by the project, including wireless.
 * @param	project_wifi_setup: Pointer to a flag, that is set once the setup is complete.
 * @note	This function must be called prior to any other Project functions.
 */
void 	PROJECT_Init			(int **project_wifi_setup);	////////
//void	PROJECT_Init			(int *project_wifi_setup);

/**
 * @brief	This function allows the user to change the project parameters via the buttons and the LCD.
 * @return	1 if any parameter was changed, 0 otherwise.
 */
int 	PROJECT_Setup			(void);

/**
 * @}
 */

#endif /* PROJECT_H_ */
