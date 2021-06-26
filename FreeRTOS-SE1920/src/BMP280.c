/*
 * BMP280.c
 *
 *  Created on: Nov 21, 2019
 *      Author: Miguelito
 */



#include <BMP280.h>

#define PINCFG_PIN_CSB 13
#define PINCFG_PINGPIO_CSB 26
#define PINCFG_PIN_SCK 30
#define PINCFG_PINGPIO_SCK 15
#define PINCFG_PIN_MISO 2
#define PINCFG_PINGPIO_MISO 17
#define PINCFG_PIN_MOSI 4
#define PINCFG_PINGPIO_MOSI 18
#define PINCFG_NOT_GPIO 3
#define PINCFG_PULL_DOWN 3
#define PINCFG_OUTPUT 1


#define BMP280_BUFF_SIZE 3

double BMP280_Raw2Temp ( signed long rw, double *tmp );
double BMP280_Raw2Press ( signed long rw, double t_fine );

trimVals BMP280_tv;

void BMP280_Init(){

	LPC_PINCON -> PINSEL1 &= ~(PINCFG_NOT_GPIO << PINCFG_PIN_CSB);
	LPC_GPIO0 -> FIODIR |= (PINCFG_OUTPUT << PINCFG_PINGPIO_CSB);

	SPI_ConfigTransfer(8, 8, 0);

	if(BMP280_ReadTrimVals()<0)
		while(1);


}

void BMP280_ChipSel( bool cs ){ //Chip input is active low
	if(cs)
		LPC_GPIO0 -> FIOCLR = 1 << PINCFG_PINGPIO_CSB; //Clear
	else
		LPC_GPIO0 -> FIOSET = 1 << PINCFG_PINGPIO_CSB; //Write
}

int BMP280_Read( int address, int dataSize, int *buff){
	if(address < 0 || address > 255 || dataSize < 1)
		return -2;
	unsigned short rxBuffer[dataSize+1];
	unsigned short txBuffer[dataSize+1];

	txBuffer[0] = (1<<7) | (unsigned short)address;

	for(int i = 0; i<10; i++){
		BMP280_ChipSel(1);
		int flags = SPI_Transfer( txBuffer, rxBuffer, dataSize+1 );
		BMP280_ChipSel(0);
		if(flags == 0){
			for(int i = 0; i < dataSize; i++){
				buff[i] = (int)rxBuffer[i+1];
			}
			return 0;
		}
		WAIT_Ms(20);
	}
	return -1;

}

int BMP280_Write( int address, int dataSize, unsigned short *data){
	if(address < 0 || address > 255 || dataSize < 1)
		return -2;
	unsigned short rxBuffer[2];
	unsigned short txBuffer[2];

	for(int i = 0; i < dataSize; i++){
		for( int j = 0; j<10; j++){
			txBuffer[0] = (~(1<<7)) & (address+i);
			txBuffer[1] = data[i];
			BMP280_ChipSel(1);
			int flags = SPI_Transfer( txBuffer, rxBuffer, 2);
			BMP280_ChipSel(0);
			if(flags == 0)
				break;
			else if(i==9)
				return -1;
			WAIT_Ms(20);
		}
	}
	return 0;
}

int BMP280_ReadTrimVals(){
	int status;
	short *ptr = (short*)&(BMP280_tv.dig_T1);
	int buff[2];
	for( int i = 0x88; i< 0xA0; i+=2, ptr++){
		if((status = BMP280_Read( i, 2, buff)) != 0)
			return status;
		*ptr = buff[1]<<8 | buff[0];
	}
	return 0;
}

bool BMP280_Id(){
	int id = 1;
	BMP280_Read( 0xD0, 1, &id);
	return (id == 0x58);
}

void BMP280_Rst(){
	unsigned short code = 0xB6;
	BMP280_Write( 0xE0, 1, &code);
}

void BMP280_Status(bool *measuring, bool *im_update){
	int status;
	BMP280_Read( 0xF3, 1, &status);
	*measuring = ((status>>3)&1) == 1;
	*im_update = (status&1) == 1;
}

void BMP280_CtrlMeas(int temp, int press, int pwr){
	unsigned short data = (temp&7)<<5 | (press&7)<<2 | (pwr&3);
	BMP280_Write( 0xF4, 1, &data);
}

void BMP280_Config(int standby, int filter){
	int registers;
	BMP280_Read( 0xF5, 1, &registers);
	unsigned short data = (standby&7)<<5 | (filter&7)<<2 | (registers&2);
	BMP280_Write( 0xF5, 1, &data);
}



double BMP280_ReadTemp(double *temp){

	int registers[3];
	BMP280_Read( 0xFA, 3, registers);
	signed long raw = (registers[0]&0xFF)<<12 | (registers[1]&0xFF)<<4 | (registers[2]&0xFF)>>4;
	return BMP280_Raw2Temp(raw, temp);
}

void BMP280_ReadPress(double *press){
	double temp;
	double t_fine = BMP280_ReadTemp(&temp);

	int registers[3];
	BMP280_Read( 0xF7, 3, registers);
	signed long raw = (registers[0]&0xFF)<<12 | (registers[1]&0xFF)<<4 | (registers[2]&0xFF)>>4;

	*press = BMP280_Raw2Press(raw, t_fine);
}


double BMP280_Raw2Temp ( signed long rw, double *tmp ){ //temp in ºC
	double var1 = (((double)rw)/16384.0 - ((double)BMP280_tv.dig_T1)/1024.0)*((double)BMP280_tv.dig_T2);
	double var2 = ((((double)rw)/131072.0 - ((double)BMP280_tv.dig_T1)/8192.0)*(((double)rw)/131072.0-((double)BMP280_tv.dig_T1)/8192.0))*((double)BMP280_tv.dig_T3);
	double t_fine = var1-var2;
	*tmp = t_fine/5120.0;
	return t_fine;
}

double BMP280_Raw2Press ( signed long rw, double t_fine ){ //press in Pa
	double p, var1, var2;
	var1 = (t_fine/2.0)-64000.0;
	var2 = var1*var1*((double)BMP280_tv.dig_P6)/32768.0;
	var2 = var2+var1*((double)BMP280_tv.dig_P5)*2.0;
	var2 = (var2/4.0)+(((double)BMP280_tv.dig_P4)*65536.0);
	var1 = (((double)BMP280_tv.dig_P3)*var1*var1/524288.0+((double)BMP280_tv.dig_P2)*var1)/524288.0;
	var1 = (1.0+var1/32768.0)*((double)BMP280_tv.dig_P1);
	p = 1048576.0-(double)rw;
	p = (p-(var2/4096.0))*6250.0/var1;
	var1 = ((double)BMP280_tv.dig_P9)*p*p/2147483648.0;
	var2 = p*((double)BMP280_tv.dig_P8)/32768.0;
	p = p+(var1+var2+((double)BMP280_tv.dig_P7))/16.0;
	return p;
}


