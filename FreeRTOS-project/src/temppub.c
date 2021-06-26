/*
 * temppub.c
 *
 *  Created on: 2 Jun 2020
 *      Author: psampaio
 */

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <led.h>
#include <wait.h>
#include <ESP8266.h>

/* Kernel includes. */
#include "FreeRTOS.h"

/* Kernel includes. */
#include "task.h"
#include "queue.h"

#include "MQTTPacket.h"
#include "transport.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "temppub.h"
#include "wifi_credentials.h"

#define STATE_INIT 0
#define STATE_CONNECT 1
#define STATE_WAIT_CONNECT 2
#define STATE_PUBLISH 3
#define STATE_SUBSCRIBE 4
#define STATE_WAIT_SUBSCRIBE 5

#define READ_TIMEOUT 5000

bool NETWORK_Init(void) {


	if(!ESP8266_ConnectedAP()) {
		if(ESP8266_Init(3) != KW_OK_ID)
			return false;
		if(ESP8266_ConnectAP(WIFI_AP_SSID, WIFI_AP_PASS) != KW_OK_ID)
			return false;
	}
	return true;
}

bool NETWORK_Connect(const char *host, const unsigned short int port, const unsigned short int keepalive)
{
	int retval;
	switch(ESP8266_ConnectedAP()) {
	default:
	case 5:
		if( !NETWORK_Init() )
			return false;
		retval = ESP8266_ConnectTCP((char*)host, (int)port, (int)keepalive);
		if( retval != KW_OK_ID && retval != KW_ALREADY_CONNECTED)
			return false;
		break;

	case 2:
	case 4:
		retval = ESP8266_ConnectTCP((char*)host, (int)port, (int)keepalive);
		if( retval != KW_OK_ID && retval != KW_ALREADY_CONNECTED)
			return false;
		break;

	case 3:
		break;
	}

	return true;
}

int NETWORK_Send(unsigned char *address, unsigned int bytes) {
	return ESP8266_Write(address, bytes);
}

int NETWORK_Recv(unsigned char *address, unsigned int maxbytes) {
	return ESP8266_ReadData((char*)address, (int)maxbytes);
}

void vLedWrite(int port, bool state) {
	if(port == 0) {
		if(state)
			LED_On();
		else
			LED_Off();
	}
}

#define CONNECTION_KEEPALIVE_S 	60UL
#define MQTT_ADDRESS			"iot-se1920.ddns.net"
#define MQTT_PORT				1883
#define MQTT_DEVICE_TOKEN		"SE2-1920-G12-WS"
/*
void TemperaturePublisher_Task(void *pvParameters) {

	unsigned char buffer[128];
	MQTTTransport transporter;
	int result;
	int length;


	//while(!NETWORK_Init());

	// ESP8266 Transport Layer
	static transport_iofunctions_t iof = { NETWORK_Send, NETWORK_Recv };
	int transport_socket = transport_open(&iof);

	int state = STATE_INIT;
	while (true) {
		switch (state) {
		case STATE_INIT:
			if (NETWORK_Connect(MQTT_ADDRESS, MQTT_PORT, CONNECTION_KEEPALIVE_S)) {
				state++;
			}
			break;
		case STATE_CONNECT:
			transporter.sck = &transport_socket;
			transporter.getfn = transport_getdatanb;
			transporter.state = 0;

			MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
			connectData.MQTTVersion = 3;
			connectData.username.cstring = MQTT_DEVICE_TOKEN;
			connectData.clientID.cstring = MQTT_DEVICE_TOKEN;
			connectData.keepAliveInterval = CONNECTION_KEEPALIVE_S * 2;
			length = MQTTSerialize_connect(buffer, sizeof(buffer), &connectData);
			// Send CONNECT to the mqtt broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				state++;
			} else {
				state = STATE_INIT;
			}
			break;
		case STATE_WAIT_CONNECT:
			// Wait for CONNACK response from the MQTT broker.
			while (true) {
				if ((result = MQTTPacket_readnb(buffer, sizeof(buffer), &transporter)) == CONNACK) {
					// Check if the connection was accepted.
					unsigned char sessionPresent, connack_rc;
					if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, buffer,
							sizeof(buffer)) != 1) || (connack_rc != 0)) {
						state = STATE_INIT;
						break;
					} else {
						state++;
						break;
					}
				} else if (result == -1) {
					state = STATE_INIT;
					break;
				}
			}
			break;
		case STATE_PUBLISH:
			vLedWrite(0, true);
			MQTTString topicString = MQTTString_initializer;
			topicString.cstring = "v1/devices/me/telemetry";
			unsigned char payload[64];
			double temp_d = data -> temperature;
			double press_d = BMP280UI_UnitConvert(data -> pressure, HECTOPASCAL);
			int temp_i = (int)temp_d;
			int press_i = (int)press_d;
			int temp_dec = (int)((temp_d - (float)temp_i)*100);
			int press_dec = (int)((press_d - (float)press_i)*100);
			length = sprintf((char*)payload, "{\"temp\":%d.%d, \"press\":%d.%d}", temp_i, temp_dec, press_i, press_dec );
			length = MQTTSerialize_publish(buffer, sizeof(buffer), 0, 0, 0, 0, topicString, payload, length);

			// Send PUBLISH to the MQTT broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				vLedWrite(0, false);
				// Wait 5s.
				WAIT_Ms(5000);
			} else {
				state = STATE_INIT;
			}
			break;
		default:
			state = STATE_INIT;
		}
	}
}*/

void WeatherStationMQTT_Task(void *pvParameters) {
	PROJECT_Data_t *data = (PROJECT_Data_t*)(((void**)pvParameters)[0]);		////	PROJECT_Data_t *data = (PROJECT_Data_t*)pvParameters;
	int *stop_flag = (int*)(((void**)pvParameters)[1]);							////
	unsigned char buffer[128];
	MQTTTransport transporter;
	int result;
	int length;

	MQTTString topicString = MQTTString_initializer;

	//while(!NETWORK_Init());

	// ESP8266 Transport Layer
	static transport_iofunctions_t iof = { NETWORK_Send, NETWORK_Recv };
	int transport_socket = transport_open(&iof);

	int state = STATE_INIT;
	while (true) {
		switch (state) {
		case STATE_INIT:
			if (NETWORK_Connect(MQTT_ADDRESS, MQTT_PORT, CONNECTION_KEEPALIVE_S)) {
				state++;
			}
			break;
		case STATE_CONNECT:
			transporter.sck = &transport_socket;
			transporter.getfn = transport_getdatanb;
			transporter.state = 0;

			MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
			connectData.MQTTVersion = 3;
			connectData.username.cstring = MQTT_DEVICE_TOKEN;
			connectData.clientID.cstring = MQTT_DEVICE_TOKEN;
			connectData.keepAliveInterval = CONNECTION_KEEPALIVE_S * 2;
			length = MQTTSerialize_connect(buffer, sizeof(buffer), &connectData);
			// Send CONNECT to the mqtt broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				state++;
			} else {
				state = STATE_INIT;
			}
			break;
		case STATE_WAIT_CONNECT:
			// Wait for CONNACK response from the MQTT broker.
			while (true) {
				if ((result = MQTTPacket_readnb(buffer, sizeof(buffer), &transporter)) == CONNACK) {
					// Check if the connection was accepted.
					unsigned char sessionPresent, connack_rc;
					if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, buffer,
							sizeof(buffer)) != 1) || (connack_rc != 0)) {
						state = STATE_INIT;
						break;
					} else {
						state = STATE_SUBSCRIBE;
						break;
					}
				} else if (result == -1) {
					state = STATE_INIT;
					break;
				}
			}
			break;

		case STATE_SUBSCRIBE:
			//MQTTString topicString = MQTTString_initializer;
			topicString.cstring = "v1/devices/me/attributes";
			int requestedQoS = 0;
			length = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, 0, 1, &topicString, &requestedQoS);

			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {						//Send
				state = STATE_WAIT_SUBSCRIBE;
				WAIT_Ms(5000);
			} else {
				state = STATE_INIT;
			}
			break;
		case STATE_WAIT_SUBSCRIBE:
			// Wait for SUBACK response from the MQTT broker.
			while (true) {
				if ((result = MQTTPacket_readnb(buffer, sizeof(buffer), &transporter)) == PUBACK) {
					// Check if the connection was accepted.
					unsigned short packetid;
					int count, grantedQoSs[3];
					if ((MQTTDeserialize_suback(&packetid, 3, &count, grantedQoSs, buffer,
							sizeof(buffer)) != 1) || (count <= 0)) {
						state = STATE_INIT;
						break;
					} else {
						state = STATE_PUBLISH;
						break;
					}
				} else if (result == -1) {
					state = STATE_INIT;
					break;
				}
			}

			//char buffy[100];
			//ESP8266_Read(3000, buffy, 100);
			break;

		case STATE_PUBLISH:
			if(*stop_flag == 0) {
				while(*stop_flag == 0)					/////
					WAIT_Ms(5000);						/////
				state = STATE_INIT;
				break;
			}

			vLedWrite(0, true);
			//MQTTString topicString = MQTTString_initializer;																//Message
			topicString.cstring = "v1/devices/me/telemetry";
			unsigned char payload[64];
			double temp_d = BMP280UI_UnitConvert(data -> temperature, CELSIUS);
			double press_d = BMP280UI_UnitConvert(data -> pressure, HECTOPASCAL);
			int temp_i = (int)temp_d;
			int press_i = (int)press_d;
			int temp_dec = (int)((temp_d - (float)temp_i)*100);
			int press_dec = (int)((press_d - (float)press_i)*100);
			length = sprintf((char*)payload, "{\"temp\":%d.%d, \"press\":%d.%d}", temp_i, temp_dec, press_i, press_dec );
			length = MQTTSerialize_publish(buffer, sizeof(buffer), 0, 0, 0, 0, topicString, payload, length);				//Publish

			// Send PUBLISH to the MQTT broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {						//Send
				vLedWrite(0, false);
				// Wait 5s.
				WAIT_Ms(5000);
			} else {
				state = STATE_INIT;
			}
			break;
		default:
			state = STATE_INIT;
		}
	}
}
