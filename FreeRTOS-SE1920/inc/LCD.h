/*
 * LCD.h
 *
 *  Created on: Oct 10, 2019
 *      Author: Miguelito
 */

#ifndef LCD_H_
#define LCD_H_

#include <wait.h>

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

/** @defgroup LCD LCD
 * This package provides the core capabilities to communicate with an LCD screen
 * controlled by an HD44780U driver through pins P2.5, P2.6, P2.7, P2.8, P2.10, P2.11 of the LPC1769 from NXP.
 * @{
 */

/**
 * @brief	Writes 4 bits to the LCD controller.
 * @param	rs: selects register. If 0, writes on the instruction register.
 * If 1, writes on the data register.
 * @param	data4: 4 bits of data to be written.
 *
 */
void LCD_write4bits( bool rs, int data4 );

/**
 * @brief	Writes 8 bits to the LCD controller.
 * @param	rs: selects register. If 0, writes on the instruction register.
 * If 1, writes on the data register.
 * @param	data8: 8 bits of data to be written.
 * @note	As there is only only 4 pins in the data port,
 * the 8 bits will be split in two packages and sent individually.
 */
void LCD_write8bits( bool rs, int data8 );

/**
 * @brief	Writes a command to the LCD controller.
 * @param	cmd: code of the desired command according to
 * the HD44780U datasheet.
 */
void LCD_writeCMD(int cmd);

/**
 * @brief	Writes to the data register.
 * @param	data: data to be written.
 */
void LCD_writeData(int data);

/**
 * @brief	Initializes the LCD API to allow 4 bit communication.
 * @note	This function must be called prior to any other LCD functions.
 */
void LCDText_Init(void);

/**
 * @brief	Writes a character to the display, in the current cursor position.
 * @param	ch: code of the character to be written.
 *
 */
void LCDText_WriteChar(char ch);

/**
 * @brief	Writes a string to the display, in the current cursor position.
 * @param	str: string to be written.
 */
void LCDText_WriteString(char *str);

/**
 * @brief	Places the cursor in a certain position in the display.
 * @param	row: number of the row. 1st row is number 1.
 * @param	column: number of the column. 1st column is number 0.
 */
void LCDText_Locate(int row, int column);

/**
 * @brief	Clears all characters of the display, using the peripheral's command.
 * This writes the character "space" to all positions.
 */
void LCDText_Clear(void);

/**
 * @brief	Turns the cursor either on or off. Not blinking.
 * @param	on: if 1 turns cursor on, if 0 off.
 */
void LCDText_Cursor(bool on);

void LCDText_WriteRightArrow();

/**
 * @brief	Writes a string to the current cursor position. Implements function
 * printf() from C language stdio library, to be used with the display.
 * @param	fmt: String that contains the text to be written. May include embedded
 * format tags that are replaced by the values specified in subsequent additional
 * arguments, and formatted as requested. Format tags include:
 * 		%c - to be substituted by a char.
 * 		%s - to be substituted by a string.
 * 		%d - to be substituted by an integer value.
 * 		%f - to be substituted by a double value.
 * Optionally, integer tags may be written as %Xd. X -> filler 0s to the left (only implemented for '2');
 * and double tags may be written as %.Xf. X -> number of decimal places.
 */
void LCDText_Printf(char *fmt, ...);

char *double2str( double dval, char array[], int decimals);

/**
 * @}
 */

#endif /* LCD_H_ */
