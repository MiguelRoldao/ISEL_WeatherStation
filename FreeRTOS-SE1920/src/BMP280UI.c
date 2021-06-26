/*
 * BMP280UI.c
 *
 *  Created on: Dec 19, 2019
 *      Author: Miguelito
 */

#include <BMP280UI.h>



unsigned int BMP280UI_TempUnits[]={CELSIUS, FARENHEIT, KELVIN};
unsigned int BMP280UI_PressUnits[]={HECTOPASCAL, PSI, ATMOSPHERE};

void BMP280UI_Init(){
	char units[4];
	BMP280UI_GetUnits(units);
	int unit_p = units[1]*0x10000 + units[2]*0x100 + units[3];
	if(!(units[0] == CELSIUS||units[0] == FARENHEIT||units[0] == KELVIN) ||
			!(unit_p == HECTOPASCAL||unit_p == PSI||unit_p == ATMOSPHERE)){
		BMP280UI_SaveUnits( BMP280UI_TempUnits[0], BMP280UI_PressUnits[0]);
	}
}

double BMP280UI_UnitConvert( double data, int unit ){
	switch( unit ){
	case HECTOPASCAL:
		return data/100;
	case PSI:
		return data/6895;
	case ATMOSPHERE:
		return data/101325;
	case FARENHEIT:
		return data*9/5 + 32;
	case KELVIN:
		return data + 273.15;
	case CELSIUS:
		return data;
	}
	return 404;
}

int BMP280UI_UnitDecimals( int unit ){
	switch( unit ){
	case HECTOPASCAL:
		return 1;
	case PSI:
		return 3;
	case ATMOSPHERE:
		return 4;
	case FARENHEIT:
		return 2;
	case KELVIN:
		return 2;
	case CELSIUS:
		return 2;
	}
	return 0;
}

void BMP280UI_GetUnits(char units[4]){
	/*char unit_t, unit_p[3];

	unsigned int * flash = (unsigned int *)FLASH_Sector2Pointer(FLASH_SECTOR);
	unit_t = (char) flash[0]&0xFF;
	for( int i = 0; i<3; i++){
		unit_p[i] = (char) flash[i+1]&0xFF;
	}
	units[0] = unit_t;
	units[1] = unit_p[0];
	units[2] = unit_p[1];
	units[3] = unit_p[2];*/
	EEPROM_ReadSequential(EEPROM_ADDR, units, 4);

}

void BMP280UI_PrintData( double temp, long unsigned int pressPa ){

	char units[4];
	BMP280UI_GetUnits(units);

	char unit_t = units[0];

	char buffer[11];

	int unit_p = units[1]*0x10000 + units[2]*0x100 + units[3];

	double2str( BMP280UI_UnitConvert( pressPa, unit_p), buffer, BMP280UI_UnitDecimals(unit_p));

	LCDText_Cursor(0);

	//LCDText_Clear();
	LCDText_Printf("Temp: %.1fº%c    \nPress: ", BMP280UI_UnitConvert( temp, unit_t), unit_t);

	LCDText_Locate(2, 7);

	for(int i = 0; i<6 && buffer[i]!='\0'; i++){
		LCDText_WriteChar( buffer[i] );
	}
	char unit_p_string[3] = {units[1], units[2], units[3]};
	LCDText_WriteString(unit_p_string);
	LCDText_WriteString("   ");
}

void BMP280UI_SetUnits(){
	int t_unit=0, p_unit=0;
	LCDText_Clear();
	LCDText_Printf("Temp unit:\n ºC  ºF  ºK");
	LCDText_Locate(2, 0);
	LCDText_WriteRightArrow();
	while( BUTTON_Hit() != 0){
		WAIT_Ms(10);
	}//wait button release

	bool loop = true;
	//set temperature unit
	while( loop ){ // Not pressed enter
		while( BUTTON_Hit() != 0 ){} //espera largar botoes
		int temp = 0;
		while( temp != BUTTON_ENTER && temp != BUTTON_UP && temp != BUTTON_DOWN ){
				temp = BUTTON_Hit();
		}
		//get button events
		switch( temp /*BUTTON_GetButtonsEvents()*/ ){
		case BUTTON_DOWN:
			if(t_unit < 2) t_unit++;
			LCDText_Locate(2, (t_unit-1)*4);
			LCDText_WriteChar(' '); //erase old arrow
			LCDText_Locate(2, t_unit*4);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		case BUTTON_UP:
			if(t_unit > 0) t_unit--;
			LCDText_Locate(2, (t_unit+1)*4);
			LCDText_WriteChar(' '); //erase old arrow
			LCDText_Locate(2, t_unit*4);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		case BUTTON_ENTER:
			loop = false;
			break;
		default:
			break;
		}
	}
	LCDText_Clear();
	LCDText_Printf("Press unit:\n hPa  PSI  atm");
	LCDText_Locate(2, 0);
	LCDText_WriteRightArrow();
	/*while( BUTTON_GetButtonsEvents() != 0){
		WAIT_Ms(10);
	}*/ //wait button release

	loop = true;
	//set pressure unit
	while( loop ){ // Not pressed enter
		while( BUTTON_Hit() != 0 ){ } //espera largar botoes
		int temp = 0;
		while( temp != BUTTON_ENTER && temp != BUTTON_UP && temp != BUTTON_DOWN ){
				temp = BUTTON_Hit();
		}
		//get button events
		switch( temp ){
		case BUTTON_DOWN:
			if(p_unit < 2) p_unit++;
			LCDText_Locate(2, (p_unit-1)*5);
			LCDText_WriteChar(' '); //erase old arrow
			LCDText_Locate(2, p_unit*5);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		case BUTTON_UP:
			if(p_unit > 0) p_unit--;
			LCDText_Locate(2, (p_unit+1)*5);
			LCDText_WriteChar(' '); //erase old arrow
			LCDText_Locate(2, p_unit*5);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		case BUTTON_ENTER:
			loop = false;
			break;
		default:
			break;
		}

	}
	//save to flash
	BMP280UI_SaveUnits( BMP280UI_TempUnits[t_unit], BMP280UI_PressUnits[p_unit]);

}

void BMP280UI_SaveUnits( unsigned int temp, unsigned int press){
	char units[]= {(char)temp, (char)((press&0xFF0000)>>0x10), (char)((press&0xFF00)>>0x8), (char)(press&0xFF)};
	//FLASH_WriteData( FLASH_Sector2Pointer(FLASH_SECTOR), units, 4*sizeof(unsigned int));
	EEPROM_WritePage(0, units, 4);
}

