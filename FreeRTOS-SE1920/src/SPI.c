/*
 * SPI.c
 *
 *  Created on: Nov 21, 2019
 *      Author: Miguelito
 */

#include <SPI.h>

#define SPICFG_POWER 2
#define SPICFG_PIN_SCK 30
#define SPICFG_PIN_SSEL 0
#define SPICFG_PIN_MISO 2
#define SPICFG_PIN_MOSI 4
#define SPICFG_SPI 3

SemaphoreHandle_t xSPI_sem;

void SPI_Init( void ){

	LPC_SC -> PCONP |= 8 << SPICFG_POWER;
	LPC_SC -> PCLKSEL0 |= 16 << 2; // CCLK/2

	LPC_PINCON -> PINSEL0 |= ( SPICFG_SPI << SPICFG_PIN_SCK );
	LPC_PINCON -> PINSEL1 |= ((SPICFG_SPI << SPICFG_PIN_MISO) | (SPICFG_SPI << SPICFG_PIN_MOSI));

	LPC_PINCON -> PINMODE0 |= ( SPICFG_SPI << SPICFG_PIN_SCK );
	LPC_PINCON -> PINMODE1 |= (	(SPICFG_SPI << SPICFG_PIN_MISO) | (SPICFG_SPI << SPICFG_PIN_MOSI));

	xSPI_sem = xSemaphoreCreateMutex();
}

/* Configura o ritmo de envio/receção,
 * o numero de bits de dados e o modo (CPOL, CPHA). */
void SPI_ConfigTransfer( int frequency, int bitData, int mode){
	bool BE = false;
	bool CPHA = mode&1;
	bool CPOL = mode&2;
	bool MSTR = true;
	bool LSBF = false;
	bool SPIE = false;
	int BITS = bitData&0xF;

	if( BITS >= 8 || BITS == 0)
		BE = true;

	int word = BE | CPHA<<1 | CPOL<<2 | MSTR<<3 | LSBF<<4 | SPIE<<5 | BITS<<6 ;

	LPC_SPI -> SPCR = word<<2;

	if( frequency < 8)
		frequency = 8;
	else if (frequency%2 != 0)
		frequency++;

	LPC_SPI -> SPCCR = frequency&255;

}



/* Realiza uma transferência. Retorna sucesso ou erro na transferência. */
int SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length)
{
	xSemaphoreTake(xSPI_sem, portMAX_DELAY);

	int flags = ((LPC_SPI -> SPSR)>>3)&0x1F;
	rxBuffer[0] = (LPC_SPI -> SPDR) & 0xFFFF;

	for( int i = 0; i < length; i++){

		LPC_SPI -> SPDR = txBuffer[i];

		flags = ((LPC_SPI -> SPSR)>>3)&0x1F;
		while( (flags&0x10) != 0x10 ){
			flags = ((LPC_SPI -> SPSR)>>3)&0x1F;
			if( (flags&0xF) != 0)
				return flags;
		}
		rxBuffer[i] = (LPC_SPI -> SPDR) & 0xFFFF;
	}

	xSemaphoreGive(xSPI_sem);
	return 0;
}
/*
int SPI_FlagChecker( int flags ){
	switch( flags ){
	case 0:
		return 1;
	default:
		return 0;
	}
}

void SPI_CompleteTransfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length, void (*ChipSel)(bool)){
	ChipSel(1);
	int flags = SPI_Transfer(txBuffer, rxBuffer, length);
	ChipSel(0);
	if(flags != 0 ){

	}
}*/

