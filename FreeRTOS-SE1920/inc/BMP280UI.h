/*
 * BMP280UI.h
 *
 *  Created on: Dec 19, 2019
 *      Author: Miguelito
 */

#ifndef BMP280UI_H_
#define BMP280UI_H_

#include <BMP280.h>
#include <button.h>
#include <LCD.h>
//#include <flash.h>
#include <eeprom.h>



/**	@defgroup BMP280UI BMP280UI
 * This package provides the core capabilities to adjust and visualize temperature and pressure from the BMP280 sensor on an LCD.
 * 	@{
 */

/**
 * @brief	ASCII value of degrees Celsius.
 */
#define CELSIUS 0x43 			//C

/**
 * @brief	ASCII value of degrees Fahrenheit.
 */
#define FARENHEIT 0x46 			//F

/**
 * @brief	ASCII value of degrees Kelvin.
 */
#define KELVIN 0x4B				//K

/**
 * @brief	ASCII value of the pressure unit Hectopascal.
 */
#define HECTOPASCAL 0x685061	//hPa

/**
 * @brief	ASCII value of the pressure unit PSI.
 */
#define PSI 0x505349			//PSI

/**
 * @brief	ASCII value of the pressure unit ATM.
 */
#define ATMOSPHERE 0x61746D		//atm

/**
 * @brief	EEPROM start address where units will be stored.
 */
#define EEPROM_ADDR 0


/**
 * @brief	Initializes the UI by configuring the temprature and pressure units.
 * @note	This function must be called prior to any other BMP280UI functions.
 */
void BMP280UI_Init(void);

/**
 * @brief	Converts a double value in ºC or Pa to ºF and ºK or hPa, PSI and atm.
 * @param	data: value to be converted.
 * @param	unit: Constant of the final unit.
 * @return	Returns the converted value.
 */
double BMP280UI_UnitConvert( double data, int unit );

/**
 * @brief	Attributes a number of decimal places to the specific unit.
 * @param	unit: Constant of the unit.
 * @return	Returns the number of decimals.
 */
int BMP280UI_UnitDecimals( int unit );

/**
 * @brief	Reads both temperature and pressure units from non-volatile memory.
 * @param	units: Array to save the read units.
 */
void BMP280UI_GetUnits(char units[4]);

/**
 * @brief	Prints to an LCD the values of temperature and pressure with the units set in memory.
 * @param	temp: Temperature to be printed in degrees Celcius.
 * @param	preePa: Pressure to be printed in Pascal.
 */
void BMP280UI_PrintData( double temp, long unsigned int pressPa );

/**
 * @brief	Prints a menu to an LCD to configure the units.
 */
void BMP280UI_SetUnits(void);

/**
 * @brief	Saves the units used to non-volatile memory.
 * @param	temp: Constant of the temperature unit.
 * @param	press: Constant of the pressure unit.
 */
void BMP280UI_SaveUnits( unsigned int temp, unsigned int press);


/**
 * @}
 */

#endif /* BMP280UI_H_ */
