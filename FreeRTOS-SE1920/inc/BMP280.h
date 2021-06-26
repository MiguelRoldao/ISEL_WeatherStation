/*
 * BMP280.h
 *
 *  Created on: Nov 21, 2019
 *      Author: Miguelito
 */

#ifndef BMP280_H_
#define BMP280_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//#include <stdbool.h>
#include <wait.h>
#include <SPI.h>

/** @defgroup BMP280 BMP280
 * This package provides the core capabilities to interface the BMP280 sensor.
 * @{
 */

/**
 * @brief	Data structure to keep the trimming values.
 */
typedef struct TrimVals{
	unsigned short dig_T1;
	short dig_T2;
	short dig_T3;
	unsigned short dig_P1;
	short dig_P2;
	short dig_P3;
	short dig_P4;
	short dig_P5;
	short dig_P6;
	short dig_P7;
	short dig_P8;
	short dig_P9;
} trimVals;

//double BMP280_t_fine;


/**
 * @brief	Initializes the pins and configures SPI to communicate with the sensor
 * @note	This function must be called prior to any other BMP280 functions.
 */
void BMP280_Init();

/**
 * @brief	Controls state of the BMP280 chip select pin. The pin is active low.
 * @param	cs: 1 -> Selected, 0 -> Not selected.
 */
void BMP280_ChipSel( bool cs );

/**
 * @brief	Reads from BMP280 registers.
 * @param	address: Address of the BMP280 register to be read (0x80-0xFF)
 * @param	dataSize: Number of bytes to be read.
 * @param	buff: array to store read data.
 */
int BMP280_Read( int address, int dataSize, int *buff);

/**
 * @brief	Writes to BMP280 registers.
 * @param	address: Address of the BMP280 register to be written (0x00-0x7F)
 * @param	dataSize: Number of bytes to be written.
 * @param	data: array of data to be written.
 */
int BMP280_Write( int address, int dataSize, unsigned short *data);

/**
 * @brief	Reads the trimming values from the BMP280.
 * @param	tv: Data structure to store the values.
 */
int BMP280_ReadTrimVals( void );

/**
 * @brief	Checks for connectivity by reading the BMP280 ID value (register 0xD0).
 * @return	1 if the ID was successful, otherwise returns 0.
 */
bool BMP280_Id();

/**
 * @brief	Resets the BMP280.
 */
void BMP280_Rst();

/**
 * @brief	Gets status of the BMP280.
 * @param	measuring: is set to 1 whenever a conversion is running and back to 0 when
 * the results have been transferred to the data registers.
 * @param	im_update: is set to 1 when the non-volatile memory data is being copied
 * to image registers and back to 0 when the copying is done.
 */
void BMP280_Status(bool *measuring, bool *im_update);

/**
 * @brief	Sets the data acquisition options of the device.
 * @param	temp: Controls over-sampling of temperature data. Refer to datasheet for more info.
 * @param	press: Controls oversampling of pressure data. Refer to datasheet for more info.
 * @param	pwr: Controls the power mode of the device. Refer to datasheet for more info.
 */
void BMP280_CtrlMeas(int temp, int press, int pwr);

/**
 * @brief	Sets the rate, filter and interface options of the device.
 * @param	standby: Controls inactive duration (t-standby). Refer to datasheet for more info.
 * @param	filter: Controls the time constant of the IIR filter. Refer to datasheet for more info.
 */
void BMP280_Config(int standby, int filter);

/**
 * @brief	Reads the temperature in ºC from the BMP280
 * @param	tv: Structure with the trimming values.
 * @param	tmp: Temperature value will be assigned to this variable.
 * @return	Returns the temperature value in ºC.
 */
double BMP280_ReadTemp(double *tmp);

/**
 * @brief	Reads the pressure in Pascal from the BMP280
 * @param	tv: Structure with the trimming values.
 * @return	Returns the pressure value in Pa.
 */
void BMP280_ReadPress(double *press);

/**
 * @}
 */



#endif /* BMP280_H_ */
