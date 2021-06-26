/*
 * led.c
 *
 *  Created on: 03/10/2019
 *      Author: MC
 */

#include <led.h>


void LED_Init( bool state ){
	LPC_GPIO0 -> FIODIR |= 1 << 22;
	if( state )
		LED_On();
	else
		LED_Off();
}

bool LED_GetState( void ){
	if( LPC_GPIO0 -> FIOSET & 1<<22 )
		return 1;
	return 0;
}

void LED_On( void ){
	LPC_GPIO0 -> FIOSET = 1 << 22;
}

void LED_Off( void ){
	LPC_GPIO0 -> FIOCLR = 1 << 22;
}


