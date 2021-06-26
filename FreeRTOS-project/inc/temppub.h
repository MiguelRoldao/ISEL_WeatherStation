/*
 * temppub.h
 *
 *  Created on: Jul 9, 2020
 *      Author: Miguelito
 */

#ifndef TEMPPUB_H_
#define TEMPPUB_H_

#include "project.h"

/** @defgroup temppub MQTT
 * This package contains the MQTT task.
 * @{
 */

//void TemperaturePublisher_Task(void *pvParameters);

/**
 * @brief	This task function maintains the WiFi connection, connects to the MQTT broker and publishes temperature and pressure data in ºC and hPa respectively.
 * @param	vpParameters: Pointer to a flag, that is set once the wireless setup is complete.
 */
void WeatherStationMQTT_Task(void *pvParameters);

/**
 * @}
 */

#endif /* TEMPPUB_H_ */
