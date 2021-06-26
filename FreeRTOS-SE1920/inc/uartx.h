/*
 * uart.h
 *
 *  Created on: Apr 2, 2020
 *      Author: Miguelito
 */

#ifndef UART_H_
#define UART_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdbool.h>
#include <stdlib.h>

bool UART_Initialize(unsigned int baud);
bool UART_IsChar(void);
unsigned char UART_ReadChar(void);
bool UART_GetChar(unsigned char *ch);
void UART_WriteChar(unsigned char ch);
void UART_WriteString(char *str);
void UART_WriteBuffer(unsigned char *buffer, int len);
void UART_Printf(char *format, ...);

#endif /* UART_H_ */
