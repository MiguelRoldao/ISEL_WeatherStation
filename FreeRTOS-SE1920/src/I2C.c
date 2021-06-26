/*
 * I2C.c
 *
 *  Created on: Mar 5, 2020
 *      Author: Miguelito
 */

#include <I2C.h>

#define I2C0_PCLK 0b11
#define I2C1_PCLK 0b01 //CLK = MCLK
#define I2C2_PCLK 0b11

#define AA_FLAG 0b100
#define SI_FLAG 0b1000
#define STO_FLAG 0b10000
#define STA_FLAG 0b100000
#define I2EN_FLAG 0b1000000


typedef struct {
	char **buffer;
	int *bufferSize;
	int *bufferCounter;
	bool *operation;
	char slaveAddr;
	int msgSize;
} I2C_Message_Type;

void _I2C_Start();
void _I2C_Stop();
int _I2C_Transfer(char addr, char *data, int size);

Counter * I2C_timeOutCounter;
int I2C_timeOutMs = 1000;

bool I2C_keepBus;
int I2C_status;
bool I2C_busy;
char I2C_slaveaddr;
bool I2C_operation;
int I2C_return;

int I2C_DataTxSize;
char *I2C_DataTx;
int I2C_DataTxCounter;

int I2C_DataRxSize;
char *I2C_DataRx;
int I2C_DataRxCounter;

LPC_I2C_TypeDef * LPC_I2C;

SemaphoreHandle_t xI2C_sem;
//TaskHandle_t xTaskToSuspend;

void _I2C_ResetGVs(){
	I2C_keepBus = 0;
	I2C_busy = 0;
}

int I2C_Init(int port){
	switch(port){
	case 0:
		LPC_I2C = LPC_I2C0;
		LPC_SC -> PCONP |= 1<<7;
		LPC_SC -> PCLKSEL0 |= I2C0_PCLK<<14;
		LPC_SC -> PCLKSEL0 &= ~((~I2C0_PCLK)<<14);
		LPC_PINCON -> PINSEL1 |= 0b0101<<22; //nao precisa pinmode/OD
		break;
	case 1:
		LPC_I2C = LPC_I2C1;
		LPC_SC -> PCONP |= 1<<19;
		LPC_SC -> PCLKSEL1 &= ~(0b11<<6);
		LPC_SC -> PCLKSEL1 |= I2C1_PCLK<<6;
		LPC_PINCON -> PINSEL0 |= 0b1111<<0;
		LPC_PINCON -> PINSEL1 |= 0b1111<<6;
		LPC_PINCON -> PINMODE0 &= ~(0b1111<<0);
		LPC_PINCON -> PINMODE0 |= 0b1010<<0;
		LPC_PINCON -> PINMODE1 &= ~(0b1111<<6);
		LPC_PINCON -> PINMODE1 |= 0b1010<<6;
		LPC_PINCON -> PINMODE_OD0 |= 0b11<<0;
		LPC_PINCON -> PINMODE_OD0 |= 0b11<<19;
		LPC_I2C -> I2SCLH = 125;
		LPC_I2C -> I2SCLL = 125;

		break;
	case 2:
		LPC_I2C = LPC_I2C2;
		LPC_SC -> PCONP |= 1<<26;
		LPC_SC -> PCLKSEL1 |= I2C0_PCLK<<20;
		LPC_SC -> PCLKSEL1 &= ~((~I2C0_PCLK)<<20);
		LPC_PINCON -> PINSEL0 |= 0b0101<<20;
		LPC_PINCON -> PINMODE0 |= 0b1010<<20;
		LPC_PINCON -> PINMODE_OD0 |= 0b11<<10;
		LPC_I2C -> I2SCLH = 500;
		LPC_I2C -> I2SCLL = 500;
		break;
	default:
		return -1;
	}
	_I2C_ResetGVs();
	LPC_I2C -> I2CONCLR = 0b1111100;
	LPC_I2C -> I2CONSET = I2EN_FLAG; //for slave accionar AA_FLAG;

	I2C_timeOutCounter = COUNTER_Create();
	_I2C_Stop();
	NVIC_EnableIRQ(I2C1_IRQn);

	xI2C_sem = xSemaphoreCreateMutex();

	return 0;
}

int I2C_Write(char addr, char *data, int size){
	xSemaphoreTake(xI2C_sem, portMAX_DELAY);
	I2C_operation = 0;
	return _I2C_Transfer(addr, data, size);
}

int I2C_Read(char addr, char *data, int size){
	xSemaphoreTake(xI2C_sem, portMAX_DELAY);
	I2C_operation = 1;
	return _I2C_Transfer(addr, data, size);
}


int _I2C_Transfer(char addr, char *data, int size){
	I2C_return = 0;
	I2C_slaveaddr = addr;
	if(I2C_operation){
		I2C_DataRx = data;
		I2C_DataRxSize = size;
	}
	else{
		I2C_DataTx = data;
		I2C_DataTxSize = size;
	}
	I2C_keepBus = 0;
	_I2C_Start();
	while(I2C_busy == 1){
		//int i = 0;
	}
	_I2C_Stop();
	int retval = I2C_return;
	xSemaphoreGive(xI2C_sem);
	return retval;
}

int I2C_WriteToRead(char addr, char *wData, int wSize, char *rData, int rSize)
{
	xSemaphoreTake(xI2C_sem, portMAX_DELAY);

	I2C_operation = 0;
	I2C_return = 0;
	I2C_slaveaddr = addr;

	I2C_DataRx = rData;
	I2C_DataRxSize = rSize;

	I2C_DataTx = wData;
	I2C_DataTxSize = wSize;

	I2C_keepBus = 1;

	_I2C_Start();

	while(I2C_busy == 1){
		//int i = 0;
	}
	_I2C_Stop();

	int retval = I2C_return;
	xSemaphoreGive(xI2C_sem);

	return retval;
}

void _I2C_Start(){
	//xTaskToSuspend = xTaskGetCurrentTaskHandle();
	I2C_busy = 0;
	LPC_I2C -> I2CONSET = STA_FLAG;
	//if(((LPC_I2C -> I2CONSET)&SI_FLAG) == SI_FLAG)
	//	LPC_I2C -> I2CONCLR = SI_FLAG;
	COUNTER_Restart( I2C_timeOutCounter );
	while( I2C_busy == 0){
		if( COUNTER_Check(I2C_timeOutCounter, I2C_timeOutMs) >= 0 ){
			I2C_return = -1;
			break;
		}
	}
}

void _I2C_Stop(){
	COUNTER_Restart( I2C_timeOutCounter );
	while(((LPC_I2C -> I2CONSET)&STO_FLAG) != 0){
		if( COUNTER_Check(I2C_timeOutCounter, I2C_timeOutMs) >= 0 ){
			I2C_return = -2;
			return;
		}
	}
	I2C_busy = 0;
}

void I2C1_IRQHandler(void){
	I2C_status = LPC_I2C -> I2STAT;
	switch( I2C_status ){
	case 0x0:
		LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		break;
	case 0x8:
		I2C_busy = 1;
		//vTaskSuspend(xTaskToSuspend);
		//I2C_bus = 1;
	case 0x10:
		LPC_I2C -> I2DAT = I2C_slaveaddr<<1 | I2C_operation;
		LPC_I2C -> I2CONSET = AA_FLAG;
		I2C_DataTxCounter=0;
		I2C_DataRxCounter=0;
		LPC_I2C -> I2CONCLR = STA_FLAG | SI_FLAG ;

		break;
	case 0x18:
		LPC_I2C -> I2DAT = I2C_DataTx[I2C_DataTxCounter];
		LPC_I2C -> I2CONSET = AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		I2C_DataTxCounter++;
		break;
	case 0x20:
		I2C_return = 1;
		if(!I2C_keepBus){
			LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_busy = 0;
			//xTaskResumeFromISR(xTaskToSuspend);
		}
		else{
			LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_operation = 1;
			I2C_keepBus = 0;
		}
		break;
	case 0x28:
		if( I2C_DataTxCounter >= I2C_DataTxSize){
			if(!I2C_keepBus){
				LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
				LPC_I2C -> I2CONCLR = SI_FLAG;
				I2C_busy = 0;
				//xTaskResumeFromISR(xTaskToSuspend);
			}
			else{
				LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
				LPC_I2C -> I2CONCLR = SI_FLAG;
				I2C_operation = 1;
				I2C_keepBus = 0;
			}
			break;
		}
		LPC_I2C -> I2DAT = I2C_DataTx[I2C_DataTxCounter];
		LPC_I2C -> I2CONSET = AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		I2C_DataTxCounter++;
		break;
	case 0x30:
		I2C_return = 1;
		if(!I2C_keepBus){
			LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_busy = 0;
			//xTaskResumeFromISR(xTaskToSuspend);
		}
		else{
			LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_operation = 1;
			I2C_keepBus = 0;
		}
		break;
	case 0x38:
		LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		I2C_return = 1;
		break;
	case 0x40:
		LPC_I2C -> I2CONSET = AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		break;
	case 0x48:
		I2C_return = 1;
		if(!I2C_keepBus){
			LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_busy = 0;
			//xTaskResumeFromISR(xTaskToSuspend);
		}
		else{
			LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_operation = 1;
			I2C_keepBus = 0;
		}
		break;
	case 0x50:
		I2C_DataRx[I2C_DataRxCounter] = LPC_I2C -> I2DAT;
		if(++I2C_DataRxCounter >= I2C_DataRxSize){
			LPC_I2C -> I2CONCLR = SI_FLAG | AA_FLAG;
			break;
		}
		LPC_I2C -> I2CONSET = AA_FLAG;
		LPC_I2C -> I2CONCLR = SI_FLAG;
		break;
	case 0x58:
		I2C_DataRx[I2C_DataRxCounter] = LPC_I2C -> I2DAT;
		I2C_return = 1;
		if(!I2C_keepBus){
			LPC_I2C -> I2CONSET = STO_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_busy = 0;
			//xTaskResumeFromISR(xTaskToSuspend);
		}
		else{
			LPC_I2C -> I2CONSET = STA_FLAG | AA_FLAG;
			LPC_I2C -> I2CONCLR = SI_FLAG;
			I2C_operation = 1;
			I2C_keepBus = 0;
		}
		break;
	}
}

