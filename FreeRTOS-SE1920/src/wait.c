#include <wait.h>
#include <timers.h>


#define WAIT_TIMER_PERIOD	pdMS_TO_TICKS( 1 )

volatile uint32_t __TS;

SemaphoreHandle_t xWAIT_sem;
TaskHandle_t xWAIT_TaskToSuspend;

static void prvWAIT_TimerCallback(TimerHandle_t xTimer);

int WAIT_Init(void){
	WAIT_TIM0Init();
	__TS = 0;
	TimerHandle_t xWaitTimer = xTimerCreate(
			"WaitTimer",
			WAIT_TIMER_PERIOD,
			pdTRUE,
			0,
			prvWAIT_TimerCallback );
	if((xWaitTimer != NULL) && (xTimerStart(xWaitTimer, 0) == pdPASS))
		return 1;
	return 0;
}

static void prvWAIT_TimerCallback(TimerHandle_t xTimer)
{
	__TS++;
}

uint32_t WAIT_Elapsed(){
	return __TS;
}

void WAIT_Ms( int time ){ //time in ms
	vTaskDelay( pdMS_TO_TICKS(time) );
}

void WAIT_TIM0Init(){
	LPC_SC -> PCONP |= (1<<1);
	LPC_SC -> PCLKSEL0 &= ~(3 << 2);
	LPC_SC -> PCLKSEL0 |= (1 << 2);
	LPC_TIM0 -> CTCR = 0;
	LPC_TIM0 -> MCR = 7;

	NVIC_EnableIRQ(TIMER0_IRQn);
	xWAIT_sem = xSemaphoreCreateMutex();
}

void WAIT_ChronoUs(int waitUs)
{ // uses timer 0.
	xSemaphoreTake(xWAIT_sem, portMAX_DELAY);
	xWAIT_TaskToSuspend = xTaskGetCurrentTaskHandle();

	LPC_TIM0 -> TCR = 2;
	LPC_TIM0 -> PR = 100;
	LPC_TIM0 -> MR0 = waitUs;
	LPC_TIM0 -> TCR = 1;

	vTaskSuspend(xWAIT_TaskToSuspend);
	xSemaphoreGive(xWAIT_sem);
}

void TIMER0_IRQHandler(void)
{
	uint32_t interrupts = LPC_TIM0 -> IR;
	LPC_TIM0 -> IR = interrupts;	//Clear interrupts
	if( (interrupts&1) && eTaskGetState(xWAIT_TaskToSuspend) == eSuspended){
		xTaskResumeFromISR(xWAIT_TaskToSuspend);
	}
}

