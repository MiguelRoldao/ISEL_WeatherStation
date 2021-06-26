/*
 * uartINT.h
 *
 *  Created on: Apr 30, 2020
 *      Author: Miguelito
 */

#ifndef UARTINT_H_
#define UARTINT_H_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdbool.h>
//#include <stdio.h>
#include <LCD.h>
#include <counter.h>

/** @defgroup UART UART
 * This package provides the core capabilities to interface the UART peripheral.
 * @{
 */

/**
 * @brief	Size of the UART ring buffer.
 */
#define UART_RBUFSIZE (1 << 11)

/**
 * @brief	UART ring buffer mask.
 */
#define RBUF_MASK (UART_RBUFSIZE-1)

/**
 * @brief	True if the UART ring buffer is full.
 */
#define RBUF_IS_FULL(head, tail) ((tail&RBUF_MASK)==((head+1)&RBUF_MASK))

/**
 * @brief	True if the UART ring buffer will be full.
 */
#define RBUF_WILL_FULL(head, tail) ((tail&RBUF_MASK)==((head+2)&RBUF_MASK))

/**
 * @brief	True if the UART ring buffer is empty.
 */
#define RBUF_IS_EMPTY(head, tail) ((head&RBUF_MASK)==(tail&RBUF_MASK))

/**
 * @brief	Reset UART ring buffer index.
 */
#define RBUF_RESET(bufidx) (bufidx=0)

/**
 * @brief	Increment UART ring buffer index.
 */
#define RBUF_INCR(bufidx) (bufidx=(bufidx+1)&RBUF_MASK)

/**
 * @brief	Data structure for the UART ring buffer.
 */
typedef struct {
__IO uint32_t txWrite;
__IO uint32_t txRead;
__IO uint32_t rxWrite;
__IO uint32_t rxRead;
__IO uint8_t tx[UART_RBUFSIZE];
__IO uint8_t rx[UART_RBUFSIZE];
} UART_RBUF_Type;

/**
 * @brief	Empties the Rx ring buffer
 */
void UARTINT_EmptyRBufferRx();

/**
 * @brief	Empties the Tx ring buffer
 */
void UARTINT_EmptyRBufferTx();

/**
 * @brief	Initializes the UART interface.
 * @param	baud: The baud frequency of the UART communications, in Hz.
 * @return	True if successful.
 * @note	This function must be called prior to any other UART functions.
 */
bool UARTINT_Initialize(unsigned int baud);

/**
 * @brief	Initializes the UART interface.
 * @param	buffer: Buffer to store read data from the Rx ring buffer.
 * @param	len: Length of buffer.
 * @param	timeOut: Time waiting for more data after the last byte was received, in ms.
 * @return	Number of bytes read.
 */
uint32_t UARTINT_ReadBuffer(uint8_t *buffer, uint32_t len, int timeOut);

/**
 * @brief	Initializes the UART interface.
 * @param	buffer: Data to be written to the Tx ring buffer.
 * @param	len: Length of buffer.
 * @return	Number of bytes written.
 */
uint32_t UARTINT_WriteBuffer(unsigned char *buffer, int len);

/**
 * @}
 */

#endif /* UARTINT_H_ */
