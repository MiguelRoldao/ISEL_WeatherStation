/*
 * project.c
 *
 *  Created on: Jul 8, 2020
 *      Author: Miguelito
 */

#include <project.h>


//Counter * printCounter;
Counter * dataCounter;
//Counter * buttonCounter;

TaskHandle_t xProjectWifi;

PROJECT_Data_t PROJECT_data;

int mqtt_run;				////////


void vProjectTaskBlinkLED(void * vpParameters)
{
	int * time_ms = (int*)vpParameters;
	for(;;)
	{
		if(LED_GetState())
			LED_Off();
		else
			LED_On();
		WAIT_Ms(*time_ms);
	}
}

void vProjectTaskWifi(void * vpParameters)
{
	int *wifi_connect_flag = ((int**)vpParameters)[0]; 			/////	int *wifi_connect_flag = (int*)vpParameters;
	int *mqtt_run_ = ((int**)vpParameters)[1];

	TaskHandle_t xProjectWifiSetupLed;
	int blink_t = 750;
	if( xTaskCreate(vProjectTaskBlinkLED, "Wifi Setup Led task", configMINIMAL_STACK_SIZE/2, &blink_t, 1, &xProjectWifiSetupLed) != pdPASS ) {
		//printf("Oopsy_wifiSetupLed\n");
	}
	LCDText_Clear();
	LCDText_Printf("ESP init...");
	ESP8266_Init(3);
	LCDText_Clear();
	LCDText_Printf("Connecting to\nAP...");
	while(ESP8266_ConnectAP(WIFI_AP_SSID, WIFI_AP_PASS) != KW_OK_ID)
		WAIT_Ms(5000);
	LCDText_Clear();
	LCDText_Printf("Connecting to\nNTP...");
	while(NTP_Connect() != KW_OK_ID)
		WAIT_Ms(5000);
	LCDText_Clear();
	LCDText_Printf("Getting\nNTP time...");
	uint32_t seconds = NTP_RequestEpochUnix();
	time_t time_sec = (time_t)seconds;
	RTC_SetSeconds(time_sec);
	ESP8266_CloseConnection();

	LCDText_Clear();
	LCDText_Printf("WiFi setup\ndone!");
	WAIT_Ms(500);
	*wifi_connect_flag = 1;
	vTaskDelete(xProjectWifiSetupLed);
	LED_Off();

	void *mqtt_params[] = { &PROJECT_data, mqtt_run_};							///////
	TaskHandle_t xProjectPublish;
	if( xTaskCreate(WeatherStationMQTT_Task, "Data publish task", configMINIMAL_STACK_SIZE*3, &mqtt_params, 3, &xProjectPublish) != pdPASS ) {					//////////
		//printf("Oopsy_dataPublisher\n");
	}
	//TaskHandle_t xProjectPublish1;
	//if( xTaskCreate(TemperaturePublisher1_Task, "Data publish1 task", configMINIMAL_STACK_SIZE*5, &PROJECT_data, 3, &xProjectPublish1) != pdPASS ) {
		//printf("Oopsy_dataPublisher\n");
	//}

	vTaskSuspend(NULL);
	for(;;)
	{
	}
}

void vProjectTaskGetData(void * vpParameters)
{
	PROJECT_Data_t * data = (PROJECT_Data_t*)vpParameters;
	for(;;)
	{
		BMP280_ReadTemp( &data->temperature );
		BMP280_ReadPress( &data->pressure );
		RTC_GetValue( &data->time );

		WAIT_Ms(REFRESH_TIME);
	}
}

void vProjectTaskPrintData(void * vpParameters)
{
	PROJECT_Data_t * data = (PROJECT_Data_t*)vpParameters;
	static bool viewFlag = true;
	for(;;)
	{
		if( mqtt_run == 0 ) {
			WAIT_Ms(REFRESH_TIME);
			//continue;
		}

		if( COUNTER_Check( dataCounter, IDLE_VIEW_TIME ) > 0 )
		{
			viewFlag = !viewFlag;
			COUNTER_Restart( dataCounter );
		}

		if( viewFlag )
			BMP280UI_PrintData( data->temperature, data->pressure );
		else
			UI_printDate( &data->time );

		WAIT_Ms(REFRESH_TIME);
	}
}


void vProjectTaskMain(void * vpParameters)
{
	int project_wifi_setup = 0;
	mqtt_run = 1;																//////
	int *project_init_params[] = {&project_wifi_setup, &mqtt_run};				//////

	PROJECT_Init(project_init_params);											//////	PROJECT_Init(&project_wifi_setup);

	//Configure BMP280 for good resolution
	BMP280_CtrlMeas(5, 5, 3);
	BMP280_Config(5, 16);


	TaskHandle_t xProjectGetData;
	TaskHandle_t xProjectPrintData;

	while(!project_wifi_setup) {
		WAIT_Ms(500);
	}

	BaseType_t retval = xTaskCreate(vProjectTaskGetData, "project Getdata task", configMINIMAL_STACK_SIZE, &PROJECT_data, 2, &xProjectGetData);
	if( retval != pdPASS ) {
		//printf("Oopsy_getData\n");
	}
	retval = xTaskCreate(vProjectTaskPrintData, "project Printdata task", configMINIMAL_STACK_SIZE, &PROJECT_data, 2, &xProjectPrintData);
	if( retval != pdPASS ) {
		//printf("Oopsy_getData\n");
	}

	for(;;)
	{
		if( BUTTON_CombTimer(BUTTON_UP|BUTTON_DOWN, 2000)){
			vTaskSuspend(xProjectPrintData);
			vTaskSuspend(xProjectGetData);
			PROJECT_Setup();
			vTaskResume(xProjectGetData);
			vTaskResume(xProjectPrintData);
		}
		WAIT_Ms(100);
	}
}

void PROJECT_Init(int **project_wifi_setup)	////////
{
	LED_Init( false );
	BUTTON_Init();
	WAIT_Init();
	//WAIT_TIM0Init();
	LCDText_Init();
	LCDText_Cursor(0);	//Hide cursor

	LCDText_Clear();
	LCDText_Printf("Initializing...");

	SPI_Init();
	RTC_Init(1);
	BMP280_Init();

	if( !BMP280_Id() ){ //Test for connectivity with BMP280
		LCDText_Printf("ERROR: Cannot\nidentify BMP280.");
		while(1);
	}

	EEPROM_Init();
	BMP280UI_Init();

	BaseType_t retval = xTaskCreate(vProjectTaskWifi, "project wifi task", configMINIMAL_STACK_SIZE * 4, project_wifi_setup, 2, &xProjectWifi);
	if( retval != pdPASS ) {
		//printf("Oopsy_wifi\n");
	}

	dataCounter = COUNTER_Create();
	//buttonCounter = COUNTER_Create();
	//printCounter = COUNTER_Create();
}

int PROJECT_Setup(){
	//// MAIN MENU
	int menuVal = 1;

	//Counter * buttonCounter = COUNTER_Create();

	//print 1st menu

	//LCDText_Printf(" Set date/time. \n Set units.     ");
	LCDText_Printf(" Get NTP time. \n Set units.     ");

	LCDText_Locate(1, 0);
	LCDText_WriteRightArrow(); // ->

	while( BUTTON_GetButtonsEvents() != BUTTON_ENTER ){ // Not pressed enter

		//pressing both up and down buttons simultaneously for 2 seconds or more exits menu
		if( BUTTON_CombTimer(BUTTON_UP|BUTTON_DOWN, 2000) > 0 ){
			//COUNTER_Destroy( buttonCounter );
			return 0;
		}
		switch( BUTTON_GetButtonsEvents() ){
		case BUTTON_UP:
			menuVal = 1;
			LCDText_Locate(2, 0);
			LCDText_WriteChar(' ');//erase old arrow
			LCDText_Locate(1, 0);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		case BUTTON_DOWN:
			menuVal = 2;
			LCDText_Locate(1, 0);
			LCDText_WriteChar(' ');//erase old arrow
			LCDText_Locate(2, 0);
			LCDText_WriteRightArrow(); //print new arrow
			break;
		default:
			break;
		}
	}
	int retval;
	switch( menuVal ){
	case 1:
		//UI_setDate();
		LCDText_Clear();
		LCDText_Printf("Connecting to\nNTP...");
		mqtt_run = 0;
		ESP8266_CloseConnection();
		while(NTP_Connect() != KW_OK_ID)
			WAIT_Ms(5000);
		LCDText_Clear();
		LCDText_Printf("Getting\nNTP time...");
		//WAIT_Ms(100);
		uint32_t seconds = NTP_RequestEpochUnix();
		time_t time_sec = (time_t)seconds;
		RTC_SetSeconds(time_sec);
		//WAIT_Ms(100);
		ESP8266_CloseConnection();

		mqtt_run = 1;
		retval = 1;
		break;
	case 2:
		BMP280UI_SetUnits();
		retval = 1;
		break;
	default:
		retval = 0;
		break;
	}
	//COUNTER_Destroy( buttonCounter );
	return retval;
}




