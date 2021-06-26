/*
 * eeprom.c
 *
 *  Created on: Mar 13, 2020
 *      Author: Miguelito
 */

#include <eeprom.h>

#define PINCFG_PIN_CSB

#define PINCFG_PIN_DOWN 20
#define PINCFG_PINGPIO_DOWN 26

int EEPROM_Init(){
	int retval = 0;
	if((retval = I2C_Init(1)) != 0 )
		return retval;
	LPC_PINCON -> PINSEL1 &= ~(3 << PINCFG_PIN_DOWN);
	LPC_GPIO0 -> FIODIR |= 1 << PINCFG_PINGPIO_DOWN;
	LPC_PINCON -> PINMODE1 = ((LPC_PINCON -> PINMODE1) & (3 << PINCFG_PIN_DOWN)) | 2<<PINCFG_PIN_DOWN;
	EEPROM_WriteProtect(false);
	return retval;
}

void EEPROM_WriteProtect(bool b){
	if(b==true)
		LPC_GPIO0 -> FIOSET = 1 << PINCFG_PINGPIO_DOWN;
	else
		LPC_GPIO0 -> FIOCLR = 1 << PINCFG_PINGPIO_DOWN;
}

int EEPROM_WriteByte(int addr, char data){
	if(addr<0||addr>0x3FFFF)
		return ADDR_INV;

	char structured[] = {(addr&0x7F00)>>8, addr&0xFF, data};
	return I2C_Write(EEPROM_ADDRESS | ((addr&0x38000)>>15), structured, 3);
}

int EEPROM_WritePage(int addr, char *data, int size){
	if(addr<0||addr>0x3FFFF)
		return ADDR_INV;
	if(size<=0 || size>64)
		return PAGE_INV;

	char structured[size+2];
	structured[0] = (addr&0x7F00)>>8;
	structured[1] = addr&0xFF;
	for( int i = 0; i<size; i++)
		structured[i+2] = data[i];
	return I2C_Write(EEPROM_ADDRESS + ((addr&0x38000)>>15), structured, size + 2);
}

int EEPROM_ReadCurrent(int addr, char *data){
	if(addr<0||addr>0x3FFFF)
		return ADDR_INV;
	return I2C_Read(EEPROM_ADDRESS + ((addr&0x38000)>>15), data, 1);
}

int EEPROM_ReadSequential(int addr, char *data, int len){
	if(addr<0||addr>0x3FFFF)
		return ADDR_INV;
	char structured[] = {(addr&0x7F00)>>8, addr&0xFF};

	return I2C_WriteToRead(EEPROM_ADDRESS, structured, 2, data, len);
}

int EEPROM_ReadRandom(int addr, char *data){
	return EEPROM_ReadSequential(addr, data, 1);
}
