/*
 * SPI.h
 *
 *  Created on: Nov 21, 2019
 *      Author: Miguelito
 */

#ifndef SPI_H_
#define SPI_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdbool.h>
#include <FreeRTOS.h>
#include <semphr.h>

/** @defgroup SPI SPI
 * This package provides the core capabilities for SPI functions.
 * @{
 */

/**
 * @brief	Initializes the SPI API.
 * @note	This function must be called prior to any other SPI functions.
 */
void SPI_Init( void );

/**
 * @brief	Configures the SPI transfer.
 * @param	frequency: Frequency of the clock signal.
 * @param	bitData: Amount of data bits to be sent/received in each transfer.
 * @param	mode: Data to clock phase relationship. CPHA = mode&0b01, CPOL = mode&0b10.
 * @note	This function should be called prior to SPI_Transfer.
 */
void SPI_ConfigTransfer( int frequency, int bitData, int mode);

/**
 * @brief	Performs SPI transfer.
 * @param	txBuffer: Array of data to be transfered.
 * @param	rxBuffer: Array of data to be received.
 * @param	length: dimension of previous data arrays.
 */
int SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length);

/**
 * @}
 */

#endif /* SPI_H_ */
