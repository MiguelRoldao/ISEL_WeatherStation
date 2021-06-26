/*
 * counter.c
 *
 *  Created on: Dec 10, 2019
 *      Author: Miguelito
 */


#include <counter.h>


Counter * COUNTER_Create(){
	Counter * p = pvPortMalloc(sizeof(Counter));
	p->start = WAIT_Elapsed();
	return p;
}

void COUNTER_Restart( Counter * p ){
	p->start = WAIT_Elapsed();
}

int COUNTER_Check( Counter * p, int time){ //retorna os milissegundos passados depois de time desde __START
	return (int)WAIT_Elapsed() - (int)p->start - time;
}

void COUNTER_Destroy( Counter * p ){
	vPortFree( p );
}


