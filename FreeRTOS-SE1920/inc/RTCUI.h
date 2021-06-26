/*
 * RTCUI.h
 *
 *  Created on: Nov 12, 2019
 *      Author: Miguelito
 */

#ifndef RTCUI_H_
#define RTCUI_H_

#include <time.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdbool.h>
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <wait.h>
#include <button.h>
#include <LCD.h>
#include <RTC.h>

/** @defgroup RTCUI RTCUI
 * This package provides the core capabilities to adjust and visualize date and time from the RTC on an LCD.
 * @{
 */

/**
 * @brief	Prints to an LCD the date and time.
 * @param	dt: Structure with the date to be printed.
 */
void UI_printDate( struct tm *dt);

/**
 * @brief	Prints a string to an LCD for a specified amount of time before clearing it.
 * @param	delay: Time the string will be visible.
 * @param	text: String to be printed.
 */
void UI_notifDelay( int delay, char * text);

/**
 * @brief	Prints a string to an LCD for 1 second before clearing it.
 * @param	text: String to be printed.
 */
void UI_notification( char * text);

/**
 * @brief	Allows visual adjustment to the day.
 * @param	dt: Structure with the date to be adjusted.
 * @param	cursorPos: Position of the cursor while adjusting the day.
 * @param	validComp: Function do calculate whether the day is valid.
 */
void UI_setDateParamDay(  struct tm * dt, int *cursorPos, bool (*validComp)(int, int, int) );

/**
 * @brief	Allows visual adjustment of any date parameter (except the day).
 * @param	dt: Structure with the date to be adjusted.
 * @param	val: Date parameter to be adjusted.
 * @param	cursorPos: Position of the cursor while adjusting the parameter.
 * @param	validComp: Function do calculate whether the parameter is valid.
 */
void UI_setDateParam(  struct tm * dt, int * val, int *cursorPos, bool (*validComp)(int) );

/**
 * @brief	Allows for visual adjustment of all date parameters of the RTC.
 */
void UI_setDate( void );

/**
 * @brief	Checks if the year is valid.
 * @param	y: Year to be checked.
 * @return	True if year is valid, it returns false otherwise.
 */
bool validYear(int y);

/**
 * @brief	Checks if the month is valid.
 * @param	m: Month to be checked.
 * @return	True if month is valid, it returns false otherwise.
 */
bool validMonth(int m);

/**
 * @brief	Checks if the day is valid.
 * @param	y: Year.
 * @param	m: Month.
 * @param	d: Day to be checked.
 * @return	True if day is valid, it returns false otherwise.
 */
bool validDay(int d, int m, int y);

/**
 * @brief	Checks if the hour is valid.
 * @param	h: Hour to be checked.
 * @return	True if hour is valid, it returns false otherwise.
 */
bool validHour(int h);

/**
 * @brief	Checks if the minute is valid.
 * @param	m: Minute to be checked.
 * @return	True if minute is valid, it returns false otherwise.
 */
bool validMinute(int m);

/**
 * @brief	Checks if the second is valid.
 * @param	s: Second to be checked.
 * @return	True if second is valid, it returns false otherwise.
 */
bool validSecond(int s);

/**
 * @}
 */

#endif /* RTCUI_H_ */
