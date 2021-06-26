/*
 * ESP8266.h
 *
 *  Created on: May 21, 2020
 *      Author: Miguelito
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <LCD.h>
#include <uartINT.h>

/** @defgroup ESP8266 ESP8266
 * This package provides the core capabilities to interface an AT commands compatible ESP-01 board, based on the ESP8266 mcu.
 * @{
 */

/**
 * @brief	Code for the OK keycode.
 */
#define KW_OK_ID 0

/**
 * @brief	Code for the ERROR keycode.
 */
#define KW_ERROR_ID -1

/**
 * @brief	Code for the READY keycode.
 */
#define KW_READY_ID -2

/**
 * @brief	Code for the FAIL keycode.
 */
#define KW_FAIL_ID -3

/**
 * @brief	Code for the SEND keycode.
 */
#define KW_SEND_ID -4

#define KW_ALREADY_CONNECTED -5

#define KW_busy -6


/**
 * @brief	ESP protocol string for TCP.
 */
#define ESP_TCP "TCP"

/**
 * @brief	ESP protocol string for UDP.
 */
#define ESP_UDP "UDP"

/**
 * @brief	ESP protocol string for SSL.
 */
#define ESP_SSL "SSL"

/**
 * @brief	Initializes the UART1 module @ 115200Hz, and the ESP.
 * @param	mode: The mode the ESP will be initialized: 1 - Station; 2 - AP; 3 - Station + AP.
 * @note	This function must be called prior to any other ESP8266 functions.
 * @return	Returns the read keywords code.
 */
int ESP8266_Init(int mode);

/**
 * @brief	Connects the ESP Station to an AP.
 * @param	ssid: the SSID of the target AP.
 * @param	pwd: password, MAX: 64-byte ASCII.
 * @return	Returns the read keywords code.
 */
int ESP8266_ConnectAP(char *ssid, char *pwd);

/**
 * @brief	Gets the Connection Status.
 * @return	2: The ESP8266 Station is connected to an AP and its IP is obtained.\n
 * 			3: The ESP8266 Station has created a TCP or UDP transmission.\n
 * 			4: The TCP or UDP transmission of ESP8266 Station is disconnected.\n
 * 			5: The ESP8266 Station does NOT connect to an AP.\n
 * 			>0: No valid response from the ESP.
 */
int ESP8266_ConnectedAP(void);

/**
 * @brief	Establishes a TCP Connection.
 * @param	ip: The remote IP address.
 * @param	port: The remote port number.
 * @param	keepalive: Detection time interval when TCP is kept alive. Range from 1 to 7200 seconds (if 0, TCP keep-alive is disabled).
 * @return	Returns the read keywords code.
 */
int ESP8266_ConnectTCP(char *ip, int port, int keepalive);

/**
 * @brief	Closes the TCP/UDP/SSL Connection.
 * @return	Returns the read keywords code.
 */
int ESP8266_CloseConnection(void);

/**
 * @brief	Finds a keyword in a buffer.
 * @param	buff: Buffer to look for a keyword.
 * @param	len: Length of the buffer.
 * @return	Returns the positive value of the keyword code.
 */
int ESP8266_FindKeyword(char *buff, int len);

/**
 * @brief	Sends an AT command to the ESP.
 * @param	str: String with the AT command to send.
 */
void ESP8266_Cmd(char * str);

/**
 * @brief	Commands the ESP to reset.
 */
void ESP8266_Rst(void);

/**
 * @brief	Reads from the UART buffer and saves on buffer the last buffer_size bytes read.
 * @param	timeout: Amount of time which the UART API waits for new data once last byte was received, in milliseconds.
 * @param	buffer: Buffer to store the last buffer_size bytes received.
 * @param	buffer_size: The size of buffer.
 * @return	Returns the number of bytes read if no keyword was received. Otherwise, it returns
 * the read keywords code.
 */
int ESP8266_Read(int timeout, char *buffer, int buffer_size);

/**
 * @brief	Reads data sent by the ESP saves it on a buffer. Allowing for n-byte data reads.
 * @param	data: Buffer to store the read data.
 * @param	data_size: Number of bytes to read.
 * @return	Returns the number of bytes read.
 */
int ESP8266_ReadData(char *data, int data_size);

/**
 * @brief	Writes data to the ESP.
 * @param	data: Data to send to the ESP.
 * @param	len: Length of the data to send.
 * @return	Returns the number of bytes written, or -1 if the ESP didn't
 * acknowledge the transmission.
 */
int ESP8266_Write(void *data, int len);

/**
 * @}
 */

#endif /* ESP8266_H_ */
