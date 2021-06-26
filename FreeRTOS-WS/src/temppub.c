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

#define STATE_INIT 0
#define STATE_CONNECT 1
#define STATE_WAIT_CONNECT 2
#define STATE_PUBLISH 3


bool NETWORK_Init(void) {
	// Init Network
	return true;
}

bool NETWORK_Connect(const char *host, const unsigned short int port,
		const unsigned short int keepalive) {
			// Connect to host
	return true;
}

int NETWORK_Send(unsigned char *address, unsigned int bytes) {
	// Send Packet
	return true;
}

int NETWORK_Recv(unsigned char *address, unsigned int maxbytes) {
	// Receive packet
	return true;
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
#define MQTT_DEVICE_TOKEN		"SE2-Test001"

static void TemperaturePublisher_Task(void *pvParameters) {
	unsigned char buffer[128];
	MQTTTransport transporter;
	int result;
	int length;

	if (!NETWORK_Init()) {
		printf("Network fail\n");
		return;
	}

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
			float temp = 25.5;
			float press = 1000.4;
			length = sprintf((char*)payload, "{\"temp\":%f, \"press\":%f}", temp, press);
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
}
