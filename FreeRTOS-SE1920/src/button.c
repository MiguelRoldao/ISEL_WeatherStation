/*
 * button.c
 *
 *  Created on: 08/10/2019
 *      Author: MC
 */
#include <button.h>

#define PINCFG_NOT_GPIO 3
#define PINCFG_PULL_DOWN 3
#define PINCFG_PIN_DOWN 14
#define PINCFG_PIN_UP 16
#define PINCFG_PIN_ENTER 18
#define PINCFG_PINGPIO_DOWN 23 //Down
#define PINCFG_PINGPIO_UP 24 //Up
#define PINCFG_PINGPIO_ENTER 25 //Enter
#define PINCFG_OUTPUT 1

//bool BUTTON_TaskSuspended = false;
//TaskHandle_t xButtonTask;
static int BUTTON_past = 0;
static int BUTTON_present = 0;
Counter * BUTTON_CombinationCounter;

int _BUTTON_Hit(void);
int _BUTTON_GetButtonsEvents(void);

void BUTTON_Init( void ){
	//xButtonTask = xTaskGetCurrentTaskHandle();
	BUTTON_CombinationCounter = COUNTER_Create();

	LPC_PINCON -> PINSEL1 &= ~((PINCFG_NOT_GPIO << PINCFG_PIN_DOWN) | (PINCFG_NOT_GPIO << PINCFG_PIN_UP) | (PINCFG_NOT_GPIO << PINCFG_PIN_ENTER));
	LPC_GPIO0 -> FIODIR &= ~(( PINCFG_OUTPUT << PINCFG_PINGPIO_DOWN ) | ( PINCFG_OUTPUT << PINCFG_PINGPIO_UP ) | ( PINCFG_OUTPUT << PINCFG_PINGPIO_ENTER ));
	LPC_PINCON -> PINMODE1 &= ~((PINCFG_PULL_DOWN << PINCFG_PIN_DOWN) | (PINCFG_PULL_DOWN << PINCFG_PIN_UP) | (PINCFG_PULL_DOWN << PINCFG_PIN_ENTER)); //not pull_down = pull_up

	LPC_GPIOINT -> IO0IntEnR |= 1<<PINCFG_PINGPIO_DOWN | 1<<PINCFG_PINGPIO_UP | 1<<PINCFG_PINGPIO_ENTER;
	LPC_GPIOINT -> IO0IntEnF |= 1<<PINCFG_PINGPIO_DOWN | 1<<PINCFG_PINGPIO_UP | 1<<PINCFG_PINGPIO_ENTER;

	NVIC_EnableIRQ(EINT3_IRQn);
}


void EINT3_IRQHandler(void){
	//uint32_t pending =  LPC_GPIOINT -> IntStatus;
	//printf("BTN INT\n");
	uint32_t rise_status = LPC_GPIOINT -> IO0IntStatR;
	uint32_t fall_status = LPC_GPIOINT -> IO0IntStatF;
	LPC_GPIOINT -> IO0IntClr = rise_status | fall_status;

	_BUTTON_GetButtonsEvents();
	if(BUTTON_past != BUTTON_present){
		COUNTER_Restart(BUTTON_CombinationCounter);
	}
}

int BUTTON_Hit(void){
	return BUTTON_present;
}

int BUTTON_Read(void){
	int b_map;
	do{
		b_map = BUTTON_Hit();
	} while(b_map == 0);
	return b_map;
}

int BUTTON_GetButtonsEvents(void){
	return (BUTTON_past<<BUTTON_N) | BUTTON_present; // bbb bbb = past present
}

int BUTTON_CombTimer(int comb, int ms){
	if(comb == BUTTON_present && COUNTER_Check(BUTTON_CombinationCounter, ms) > 0){ // Ha butoes durante 2sec ou mais)
		COUNTER_Restart(BUTTON_CombinationCounter);
		return 1;
	}
	return 0;
}

int _BUTTON_Hit(void){
	uint32_t r;
	r = ~(LPC_GPIO0 -> FIOPIN);
	r = ((r >> PINCFG_PINGPIO_DOWN)&1) | (((r >> PINCFG_PINGPIO_UP)&1)<<1) | (((r >> PINCFG_PINGPIO_ENTER)&1)<<2);
	return (int)r ;
}

int _BUTTON_GetButtonsEvents(void){
	BUTTON_past = BUTTON_present;
	BUTTON_present = _BUTTON_Hit();
	int output = (BUTTON_past << BUTTON_N) + BUTTON_present; // bbb bbb = past present
	return output;
}
