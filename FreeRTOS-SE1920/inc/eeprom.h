/*
 * eeprom.h
 *
 *  Created on: Mar 13, 2020
 *      Author: Miguelito
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <I2C.h>
#include <stdbool.h>

/** @defgroup EEPROM EEPROM
 * This package provides the core capabilities to interface an array of up to 8 24AA256 EEPROM chips.
 * @{
 */

/**
 * @brief	7-bit I2C address of the EEPROM chip.
 */
#define EEPROM_ADDRESS 0b1010000

/**
 * @brief	Invalid address return code.
 */
#define ADDR_INV 0x10

/**
 * @brief	Invalid page number return code.
 */
#define PAGE_INV 0x11




/**
 * @brief	Initializes the pins and configures I2C to communicate with the EEPROM chip.
 * @note	This function must be called prior to any other EEPROM functions.
 * @return	Returns 0 if successful.
 */
int EEPROM_Init();

/**
 * @brief	Enables/disables eeprom hardware write-protect.
 * @param	b: 1 -> write operations are inhibited, 0 -> normal memory operation.
 */
void EEPROM_WriteProtect(bool b);

/**
 * @brief	Writes a single byte to EEPROM.
 * @param	addr: EEPROM memory address (0x00000-0x3FFFF){A2,A1,A0,a14,a13,a12,a11,a10,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0}
 * @param	data: byte of data to be written.
 * @return	Returns: ADDR_INV - Invalid address,  0 - ACK received, 1 - ACK not received, -1 - START condition timeout, -2 - STOP condition timeout.
 */
int EEPROM_WriteByte(int addr, char data);

/**
 * @brief	Writes to an EEPROM page.
 * @param	addr: EEPROM memory address (0x00000-0x3FFFF){A2,A1,A0,a14,a13,a12,a11,a10,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0}
 * @param	data: data bytes to be written.
 * @param	size: Number of bytes to be written (up to 64).
 * @return	Returns: PAGE_INV - Invalid page size, ADDR_INV - Invalid address,  0 - ACK received, 1 - ACK not received, -1 - START condition timeout, -2 - STOP condition timeout.
 */
int EEPROM_WritePage(int addr, char *data, int size);

/**
 * @brief	Reads one byte from the current address in the EEPROM chip's internal address counter. The address counter is incremented by 1 after each read access.
 * @param	addr: EEPROM memory address (0x00000-0x3FFFF){A2,A1,A0,a14,a13,a12,a11,a10,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0}. Used to select the EEPROM chip.
 * @param	data: buffer where the data will be read to.
 * @return	Returns: ADDR_INV - Invalid address,  0 - ACK received, 1 - ACK not received, -1 - START condition timeout, -2 - STOP condition timeout.
 */
int EEPROM_ReadCurrent(int addr, char *data);

/**
 * @brief	Reads len bytes from the address addr to the buffer data.
 * @param	addr: EEPROM memory address (0x00000-0x3FFFF){A2,A1,A0,a14,a13,a12,a11,a10,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0}.
 * @param	data: buffer where the data will be read to.
 * @param	len: number of bytes to be read from EEPROM. Must not exceed data's length.
 * @return	Returns: ADDR_INV - Invalid address,  0 - ACK received, 1 - ACK not received, -1 - START condition timeout, -2 - STOP condition timeout.
 */
int EEPROM_ReadSequential(int addr, char *data, int len);

/**
 * @brief	Reads one byte from the address addr to the buffer data.
 * @param	addr: EEPROM memory address (0x00000-0x3FFFF){A2,A1,A0,a14,a13,a12,a11,a10,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0}.
 * @param	data: buffer where the data will be read to.
 * @return	Returns: ADDR_INV - Invalid address,  0 - ACK received, 1 - ACK not received, -1 - START condition timeout, -2 - STOP condition timeout.
 */
int EEPROM_ReadRandom(int addr, char *data);

/**
 * @}
 */


#endif /* EEPROM_H_ */
