/*
 * RTC.h
 *
 *  Created on: Oct 24, 2019
 *      Author: Miguelito
 */

#ifndef RTC_H_
#define RTC_H_

/** @defgroup RTC RTC
 * This package provides the core capabilities to utilize the built-in RTC.
 * @{
 */

/**
 * @brief	Initializes system to allow access to the RTC peripheral. The RTC is initializes with the value of the parameter seconds.
 * @param	seconds: Number of seconds since January 1st 1970 00:00:00 UTC.
 * @note	This function must be called prior to any other RTC functions.
 */
void RTC_Init(time_t seconds);

/**
 * @brief	Gets RTC current time.
 * @param	dateTime: Current time is attributed to this structure.
 */
void RTC_GetValue(struct tm *dateTime);

/**
 * @brief	Sets RTC current time with structure dateTime.
 * @param	dateTime: Structure containing time to set the RTC.
 */
void RTC_SetValue(struct tm *dateTime);

/**
 * @brief	Sets RTC current time with parameter seconds.
 * @param	seconds: Number of seconds since January 1st 1970 00:00:00 UTC.
 */
void RTC_SetSeconds(time_t seconds);

/**
 * @brief	Returns the number of seconds since January 1st 1970 00:00:00 UTC.
 * @return	The number of seconds since January 1st 1970 00:00:00 UTC.
 */
time_t RTC_GetSeconds(void);

/**
 * @brief	Returns whether the specified year is leap or not.
 * @param	y: Year.
 * @return	1 if y is leap, else 0.
 */
bool LeapYear(int y);

/**
 * @brief	Returns number of days in a specific month.
 * @param	y: Year.
 * @param	m: Month.
 * @return	Number of days in the specific month.
 */
int DaysInMonth( int m, int y);

/**
 * @}
 */

#endif /* RTC_H_ */
