/*
 * uart.c
 *
 *  Created on: Apr 2, 2020
 *      Author: Miguelito
 */

#include <uartx.h>

#define UART_FCR_FIFO_EN 0b1
#define UART_FCR_RX_RS 0b10
#define UART_FCR_TX_RS 0b100

#define UART_LCR_WLEN8 0b11
#define UART_LCR_BREAK_EN 0b1000000
#define UART_LCR_DLAB_EN 0b10000000
#define UART_LCR_BITMASK 0b11111111

#define UART_LSR_RDR 0b1
#define UART_LSR_OE 0b10
#define UART_LSR_PE 0b100
#define UART_LSR_FE 0b1000
#define UART_LSR_BI 0b10000
#define UART_LSR_THRE 0b100000
#define UART_LSR_TEMT 0b1000000
#define UART_LSR_RXFE 0b10000000

#define UART_TER_TXEN 0b10000000

LPC_UART_TypeDef *UARTx = LPC_UART2;

#define UART_TABLES_SIZE 72

double UART_FR_TABLE[UART_TABLES_SIZE] = {
		1.000, 1.067, 1.071, 1.077, 1.083, 1.091, 1.100, 1.111, 1.125,
		1.133, 1.143, 1.154, 1.167, 1.182, 1.200, 1.214, 1.222, 1.231,
		1.250, 1.267, 1.273, 1.286, 1.300, 1.308, 1.333, 1.357, 1.364,
		1.375, 1.385, 1.400, 1.417, 1.429, 1.444, 1.455, 1.462, 1.467,
		1.500, 1.533, 1.538, 1.545, 1.556, 1.571, 1.583, 1.600, 1.615,
		1.625, 1.636, 1.643, 1.667, 1.692, 1.700, 1.714, 1.727, 1.733,
		1.750, 1.769, 1.778, 1.786, 1.800, 1.818, 1.833, 1.846, 1.857,
		1.867, 1.875, 1.889, 1.900, 1.909, 1.917, 1.923, 1.929, 1.933
};

char UART_DAV_TABLE[UART_TABLES_SIZE] = {
		0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 3, 2, 3,
		1, 4, 3, 2, 3, 4, 1, 5, 4, 3, 5, 2, 5, 3, 4, 5, 6, 7,
		1, 8, 7, 6, 5, 4, 7, 3, 8, 5, 7, 9, 2, 9, 7, 5, 8, 11,
		3, 10, 7, 11, 4, 9, 5, 11, 6, 13, 7, 8, 9, 10, 11, 12, 13, 14
};

char UART_MV_TABLE[UART_TABLES_SIZE] = {
		1, 15, 14, 13, 12, 11, 10, 9, 8, 15, 7, 13, 6, 11, 5, 14, 9, 13,
		4, 15, 11, 7, 10, 13, 3, 14, 11, 8, 13, 5, 12, 7, 9, 11, 13, 15,
		2, 15, 13, 11, 9, 7, 12, 5, 13, 8, 11, 14, 3, 13, 10, 7, 11, 15,
		4, 13, 9, 14, 5, 11, 6, 13, 7, 15, 8, 9, 10, 11, 12, 13, 14, 15
};

bool UARTSetDivisors(unsigned int baud, uint32_t pclock)
{

	if( baud > 0xFFFFFFFF )
		return false;

	double br = (double)baud;
	double pclk = (double)pclock;
	int i;

	double fr_est;
	uint32_t dl_est;

	int bestIndex = 0;
	uint32_t bestDL = 0;
	double bestTest = 0;

	for(i = 0; i<UART_TABLES_SIZE; i++){
		fr_est = 1 + ((double)(UART_DAV_TABLE[i]))/((double)(UART_MV_TABLE[i]));
		dl_est = pclk/(16 * br * fr_est);
		double test = pclk/(16 * fr_est * (double)((int)dl_est));

		if( abs(br-bestTest) > abs(br-test) ){
			bestDL = dl_est;
			bestIndex = i;
			bestTest = test;
		}
	}

	if (!bestDL)
		return false;

	UARTx->LCR |= UART_LCR_DLAB_EN;

	UARTx -> DLL = (uint8_t)(bestDL & 0xFF);
	UARTx -> DLM = (uint8_t)((bestDL>>8) & 0xFF);
	UARTx -> FDR = (UART_DAV_TABLE[bestIndex]&0xFF) | ((UART_MV_TABLE[bestIndex]&0xFF) << 4);

	UARTx->LCR &= ~UART_LCR_DLAB_EN & UART_LCR_BITMASK;
	return true;
}

bool UART_Initialize(unsigned int baud)
{
	uint8_t tmp;
	uint32_t pclk;

	LPC_PINCON -> PINSEL0 &= ~(0b1111<<20);
	LPC_PINCON -> PINSEL0 |= 0b0101<<20;	//UART 2
	LPC_PINCON -> PINMODE0 &= ~(0b1111<<20); //PULL UP
	LPC_PINCON -> PINMODE_OD0 |= 0b11<<10;	//OPEN DRAIN
	// Initialise UART Configuration to default state: 8, 1, None

	LPC_SC -> PCONP |= 1<<24;			//Set up power
	LPC_SC -> PCLKSEL1 &= ~(0b11<<16);	// Set up clock
	LPC_SC -> PCLKSEL1 |= 0b01<<16;
	switch((LPC_SC -> PCLKSEL1 & (0b11<<16))>>16){
	case 0b00:
		pclk = SystemCoreClock/4;
		break;
	case 0b01:
		pclk = SystemCoreClock;
		break;
	case 0b10:
		pclk = SystemCoreClock/2;
		break;
	case 0b11:
		pclk = SystemCoreClock/8;
		break;
	}


	// FIFOs are empty
	UARTx -> FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);
	UARTx -> FCR = 0; // Disable FIFO



	UARTx -> TER = UART_TER_TXEN;

	while (!(UARTx -> LSR & UART_LSR_THRE)); // Wait for tx complete

	UARTx -> TER = 0;		// Disable Tx
	UARTx -> IER = 0; 		// Disable interrupt
	UARTx -> LCR = 0;		// Set LCR to default state
	UARTx -> ACR = 0;		// Set ACR to default state

	tmp = UARTx -> LSR; // Clean status
	if (!UARTSetDivisors(baud, pclk))
		return false;

	tmp = (UARTx -> LCR & (UART_LCR_DLAB_EN | UART_LCR_BREAK_EN)) & UART_LCR_BITMASK;
	tmp |= UART_LCR_WLEN8;
	UARTx->LCR = (uint8_t) (tmp & UART_LCR_BITMASK);
	UARTx->TER |= UART_TER_TXEN;

	while (UARTx -> LSR & UART_LSR_RDR) { // Dummy reading - empty rx!
		tmp = UARTx -> RBR;
	}
	return true;
}


bool UART_GetChar(unsigned char *ch)
{
	if ((UARTx->LSR & UART_LSR_RDR) == 0)
		return false;
	*ch = UARTx->RBR;
	return true;
}

bool UART_IsChar(void)
{
	return (UARTx->LSR & UART_LSR_RDR) != 0;
}

unsigned char UART_ReadChar(void)
{
	while ((UARTx->LSR & UART_LSR_RDR) == 0);
	return UARTx->RBR;
}

void UART_WriteChar(unsigned char ch)
{
	while ((UARTx->LSR & UART_LSR_THRE) == 0);
	UARTx->THR = ch;
}

void UART_WriteString(char *str)
{
	while (*str)
	{
		UART_WriteChar(*str++);
	}
}

void UART_WriteBuffer(unsigned char *buffer, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		UART_WriteChar(*buffer++);
	}
}
