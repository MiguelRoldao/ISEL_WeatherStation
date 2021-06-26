/*
 * RTCUI.c
 *
 *  Created on: Nov 12, 2019
 *      Author: Miguelito
 */

#include <RTCUI.h>

void UI_printDate( struct tm *dt){
	LCDText_Printf("%d/%2d/%2d      \n%2d:%2d:%2d        ", 1900+dt->tm_year, 1+dt->tm_mon, dt->tm_mday, dt->tm_hour, dt->tm_min, dt->tm_sec);
}

void UI_notifDelay( int delay, char * text){
	LCDText_Locate(2, 9);
	LCDText_WriteString( text );
	WAIT_Ms( delay );
	LCDText_Locate(2, 9);
	LCDText_WriteString( "       " );

}

void UI_notification( char * text){
	UI_notifDelay( 1000 , text );
}

void UI_setDateParam(  struct tm * dt, int * val, int *cursorPos, bool (*validComp)(int) ){// p = parametro a editar
	while(1){
		bool done = 0;
		struct tm temp_dt = *dt;
		//int edited = *val;
		while( BUTTON_GetButtonsEvents() != 0 ){ WAIT_Ms(10); } //espera largar botoes

		UI_printDate( &temp_dt );
		LCDText_Locate(cursorPos[0], cursorPos[1]);

		int temp = 0;
		while( temp != BUTTON_ENTER && temp != BUTTON_UP && temp != BUTTON_DOWN ){
				temp = BUTTON_GetButtonsEvents()&7;
		}

		switch (temp){
			case BUTTON_ENTER:
				if( validComp( *val ) ){
					done = 1;
					break;

				}
				else{
					UI_notification( "Invalid" );
					break;
				}
			case BUTTON_DOWN:
				if( validComp( *val + 1 ) ){
					++*val;
				}
				break;
			case BUTTON_UP:
				if( validComp( *val - 1 ) )
					--*val;
				break;
		}
		if(done)
			break;
	}
}
void UI_setDateParamDay(  struct tm * dt, int *cursorPos, bool (*validComp)(int, int, int) ){// p = parametro a editar
	while(1){
		bool done = 0;
		struct tm temp_dt = *dt;
		//int edited = dt->tm_mday;
		while( BUTTON_GetButtonsEvents() != 0 ){ WAIT_Ms(10); } //espera largar botoes

		UI_printDate( &temp_dt );
		LCDText_Locate(cursorPos[0], cursorPos[1]);

		int temp = 0;
		while( temp != BUTTON_ENTER && temp != BUTTON_UP && temp != BUTTON_DOWN ){
			temp = BUTTON_GetButtonsEvents()&7;
		}

		switch (temp){
			case BUTTON_ENTER:
				if( validComp( dt->tm_mday, dt->tm_mon, dt->tm_year ) ){
					done = 1;
					break;
				}
				else{
					UI_notification( "Invalid" );
					break;
				}
			case BUTTON_DOWN:
				if( validComp( dt->tm_mday + 1, dt->tm_mon, dt->tm_year ) )
					dt->tm_mday++;
				break;
			case BUTTON_UP:
				if( validComp( dt->tm_mday - 1, dt->tm_mon, dt->tm_year ) )
					dt->tm_mday--;
				break;
		}
		if(done)
			break;
	}
}

void UI_setDate( ){
	struct tm date;
	RTC_GetValue( &date );

	LCDText_Cursor(1); //show cursor

	UI_setDateParam( &date, &(date.tm_year), (int[2]){1,3}, validYear);
	UI_setDateParam( &date, &(date.tm_mon), (int[2]){1,6}, validMonth);
	UI_setDateParamDay( &date, (int[2]){1,9}, validDay);
	UI_setDateParam( &date, &(date.tm_hour), (int[2]){2,1}, validHour);
	UI_setDateParam( &date, &(date.tm_min), (int[2]){2,4}, validMinute);
	UI_setDateParam( &date, &(date.tm_sec), (int[2]){2,7}, validSecond);


	while( BUTTON_GetButtonsEvents() != 0 ){}
	RTC_SetValue( &date );
	LCDText_Cursor(0);
	UI_notification( "Success" );


}



bool validYear(int y){
	return y >= 0 && y < 4096;
}

bool validMonth(int m){
	return m >= 0 && m <= 11;
}

bool validDay(int d, int m, int y){
	return d >= 1 && d <= DaysInMonth(m, y);
}

bool validHour(int h){
	return h >= 0 && h < 24;
}

bool validMinute(int m){
	return m >= 0 && m < 60;
}

bool validSecond(int s){
	return validMinute(s);
}
