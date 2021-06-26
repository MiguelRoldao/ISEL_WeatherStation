/*
 * I2C.h
 *
 *  Created on: Mar 13, 2020
 *      Author: Miguelito
 */

#ifndef I2C_H_
#define I2C_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdbool.h>
#include <counter.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

/** @defgroup I2C I2C
 * This package provides the core capabilities to interface the I2C peripherals.
 * @{
 */

/**
 * @brief	Initializes the I2C interface.
 * @param	port: The port number to initialize the I2C bus.
 * @return	Returns 0 if successful.
 * @note	This function must be called prior to any other I2C functions.
 */
int I2C_Init(int port);

/**
 * @brief	Writes data through I2C.
 * @param	addr: Target slave address.
 * @param	data: Data to write to the slave.
 * @param	size: Size of the data array.
 * @return	0 if ACK received.\n
 * 1 if NACK received.\n
 * -1 if start condition timeout.\n
 * -2 is stop condition timeout.
 */
int I2C_Write(char addr, char *data, int size);

/**
 * @brief	Reads data through I2C.
 * @param	addr: Target slave address.
 * @param	data: Buffer to store read data.
 * @param	size: Size of the data array.
 * @return	0 if ACK received.\n
 * 1 if NACK received.\n
 * -1 if start condition timeout.\n
 * -2 is stop condition timeout.
 */
int I2C_Read(char addr, char *data, int size);

/**
 * @brief	Peformrs a write operation followed by a read operation, using a repeated start condition.
 * @param	addr: Target slave address.
 * @param	wData: Data to write to the slave.
 * @param	wSize: Size of the wData array.
 * @param	rData: Buffer to store read data.
 * @param	rSize: Size of the rData array.
 * @return	0 if ACK received.\n
 * 1 if NACK received.\n
 * -1 if start condition timeout.\n
 * -2 is stop condition timeout.
 */
int I2C_WriteToRead(char addr, char *wData, int wSize, char *rData, int rSize);

/**
 * @}
 */

#endif /* I2C_H_ */
