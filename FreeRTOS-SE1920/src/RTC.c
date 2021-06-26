/*
 * RTC.c
 *
 *  Created on: Oct 24, 2019
 *      Author: Miguelito
 */

#include <time.h>
//#include <string.h>
#include <stdbool.h>
//#include <stdlib.h>
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void RTC_Gmtime(struct tm *time_tm, time_t seconds);


void RTC_GetValue(struct tm *dateTime){


	int reg0 = LPC_RTC -> CTIME0;
	int reg1 = LPC_RTC -> CTIME1;
	int reg2 = LPC_RTC -> CTIME2;

	dateTime -> tm_yday = (reg2-1) & 4095;

	dateTime -> tm_mday = reg1 & 31;
	dateTime -> tm_mon = (( reg1 >> 8 )- 1 ) & 15;
	dateTime -> tm_year = ((reg1 >> 16) - 1900) & 4095;

	dateTime -> tm_sec = reg0 & 63;
	dateTime -> tm_min = reg0 >> 8 & 63;
	dateTime -> tm_hour = reg0 >> 16 & 31;
	dateTime -> tm_wday = reg0 >> 24 & 7;

	dateTime -> tm_isdst = 0; //////depois ver isto
}



void RTC_SetValue(struct tm *dateTime){
	LPC_RTC -> CCR &= ~1;	//Set CLKEN-0

	LPC_RTC -> SEC = dateTime -> tm_sec;
	LPC_RTC -> MIN = dateTime -> tm_min;
	LPC_RTC -> HOUR = dateTime -> tm_hour;
	LPC_RTC -> DOW = dateTime -> tm_wday;

	LPC_RTC -> DOM = dateTime -> tm_mday;
	LPC_RTC -> MONTH = dateTime -> tm_mon + 1;
	LPC_RTC -> YEAR = dateTime -> tm_year + 1900;

	LPC_RTC -> DOY = dateTime -> tm_yday + 1;
	LPC_RTC -> CCR |= 1;	//Set CLKEN-1
}


void RTC_SetSeconds(time_t seconds){
	struct tm time; // = *gmtime( &seconds );
	RTC_Gmtime(&time, seconds);
	RTC_SetValue( &time );
}


void RTC_Init(time_t seconds){

	LPC_RTC -> ILR &= ~3;	//Set zeros
	LPC_RTC -> CCR &= ~31;	//Set CLKEN-0, CTCRST-0, CCALEN-0 (internal test mode)-0
	LPC_RTC -> CCR |= 17;	//Set CLKEN-1, CTCRST-0, CCALEN-1

	RTC_SetSeconds( seconds );
}


time_t RTC_GetSeconds(void){
	struct tm *reg = NULL;
	RTC_GetValue( reg );
	return mktime( reg );
}


bool LeapYear(int y){
	if(y%4 !=0 )
		return 0;
	else if(y%100 != 0)
		return 1;
	else if(y%400 != 0)
		return 0;
	else
		return 1;
}

int DaysInMonth( int m, int y){
	switch (m+1){
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		return 28 + (int)LeapYear(y);
	default:
		return 31;
	}
}


// Devido à funcao gmtime() da biblioteca time.h nao funcionar corretamente com qualquer ficheiro heap_x.c, utilizei a implementacao abaixo
// Source: https://doxygen.reactos.org/d6/da2/gmtime_8c_source.html

unsigned int g_monthdays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
unsigned int g_lpmonthdays[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

#define DIFFTIME 0x19db1ded53e8000ULL
#define DIFFDAYS (3 * DAYSPER100YEARS + 17 * DAYSPER4YEARS + 1 * DAYSPERYEAR)

#define DAYSPERYEAR 365
#define DAYSPER4YEARS (4*DAYSPERYEAR+1)
#define DAYSPER100YEARS (25*DAYSPER4YEARS-1)
#define DAYSPER400YEARS (4*DAYSPER100YEARS+1)
#define SECONDSPERDAY (24*60*60)
#define SECONDSPERHOUR (60*60)
#define LEAPDAY 59

static __inline long leapyears_passed(long days)
{
    long quadcenturies, centuries, quadyears;
    quadcenturies = days / DAYSPER400YEARS;
    days -= quadcenturies;
    centuries = days / DAYSPER100YEARS;
    days += centuries;
    quadyears = days / DAYSPER4YEARS;
    return quadyears - centuries + quadcenturies;
}

static __inline long leapdays_passed(long days)
{
    return leapyears_passed(days + DAYSPERYEAR - LEAPDAY + 1);
}

static __inline long years_passed(long days)
{
    return (days - leapdays_passed(days)) / 365;
}

void RTC_Gmtime(struct tm *time_tm, time_t seconds)
{

	struct tm *ptm = time_tm;
	time_t time = seconds;
	//int do_dst = 0;

	unsigned int days, daystoyear, dayinyear, leapdays, leapyears, years, month;
	unsigned int secondinday, secondinhour;
	unsigned int *padays;
	if (time < 0)
		return;

		 /* Divide into date and time */
	days = (unsigned int)(time / SECONDSPERDAY);
	secondinday = time % SECONDSPERDAY;
		/* Shift to days from 1.1.1601 */
	days += DIFFDAYS;
		 /* Calculate leap days passed till today */
	leapdays = leapdays_passed(days);
		 /* Calculate number of full leap years passed */
	leapyears = leapyears_passed(days);
		 /* Are more leap days passed than leap years? */
	if (leapdays > leapyears)
	{
		/* Yes, we're in a leap year */
		padays = g_lpmonthdays;
	}
	else
	{
		/* No, normal year */
		padays = g_monthdays;
	}
		 /* Calculate year */
	years = (days - leapdays) / 365;
	ptm->tm_year = years - 299;
		 /* Calculate number of days till 1.1. of this year */
	daystoyear = years * 365 + leapyears;
		 /* Calculate the day in this year */
	dayinyear = days - daystoyear;
		 /* Shall we do DST corrections? */
	ptm->tm_yday = dayinyear;
		 /* dayinyear < 366 => terminates with i <= 11 */
	for (month = 0; dayinyear >= padays[month+1]; month++);
		 /* Set month and day in month */
	ptm->tm_mon = month;
	ptm->tm_mday = 1 + dayinyear - padays[month];
		 /* Get weekday */
	ptm->tm_wday = (days + 1) % 7;
		 /* Calculate hour and second in hour */
	ptm->tm_hour = secondinday / SECONDSPERHOUR;
	secondinhour = secondinday % SECONDSPERHOUR;
		 /* Calculate minute and second */
	ptm->tm_min = secondinhour / 60;
	ptm->tm_sec = secondinhour % 60;

}

