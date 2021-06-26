/*
 * ntp.h
 *
 *  Created on: Jun 30, 2020
 *      Author: Miguelito
 */

#ifndef NTP_H_
#define NTP_H_

#include <LCD.h>
#include <uartINT.h>
#include <ESP8266.h>

/** @defgroup NTP NTP
 * This package provides connectivity to the pool.ntp.org ntp server, over the ESP8266 package.
 * @{
 */

/**
 * @brief	Data structure for the NTP packet.
 */
typedef struct
{
	uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
					 // li. Two bits. Leap indicator.
					 // vn. Three bits. Version number of the protocol.
					 // mode. Three bits. Client will pick mode 3 for client.
	uint8_t stratum; // Eight bits. Stratum level of the local clock.
	uint8_t poll; // Eight bits. Maximum interval between successive messages.
	uint8_t precision; // Eight bits. Precision of the local clock.

	uint32_t rootDelay; // 32 bits. Total round trip delay time.
	uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
	uint32_t refId; // 32 bits. Reference clock identifier.

	uint32_t refTm_s; // 32 bits. Reference time-stamp seconds.
	uint32_t refTm_f; // 32 bits. Reference time-stamp fraction of a second.

	uint32_t origTm_s; // 32 bits. Originate time-stamp seconds.
	uint32_t origTm_f; // 32 bits. Originate time-stamp fraction of a second.

	uint32_t rxTm_s; // 32 bits. Received time-stamp seconds.
	uint32_t rxTm_f; // 32 bits. Received time-stamp fraction of a second.

	uint32_t txTm_s; // 32 bits. Transmit time-stamp seconds. Important
	uint32_t txTm_f; // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet; // Total: 384 bits or 48 bytes.

/**
 * @brief	Data structure for the NTP timestamp.
 */
typedef struct
{
	uint32_t txTm_s;
	uint32_t txTm_f;
} ntp_timestamp;

/**
 * @brief	Connects to the NTP server.
 * @return	Returns 0 if successful.
 */
int NTP_Connect();

/**
 * @brief	Decodes the received message and gets the timestamp.
 * @param	buffer: Buffer containing the received message.
 * @param	buffer_len: Length of the buffer in bytes.
 * @param	pos: Start position of the NTP packet.
 * @param	dst: Structure to store the timestamp.
 * @return	Returns 0 if successful.
 */
int NTP_DecodePacket(char *buffer, int buffer_len, int pos, ntp_timestamp *dst);

/**
 * @brief	Requests a NTP message.
 * @param	li_vn_mode: Leap indicator, version number of the protocol and mode.\nLLVVVMMM
 * @param	stratum: Stratum level of the local clock.
 * @param	poll: Start position of the NTP packet.
 * @param	precision: Precision of the local clock.
 * @param	ts: Structure to store the timestamp.
 * @return	Returns 0 if successful.
 */
int NTP_RequestValues(uint8_t li_vn_mode, uint8_t stratum, uint8_t poll, uint8_t precision, ntp_timestamp *ts);

/**
 * @brief	Requests a NTP message.
 * @param	ts: Structure to store the timestamp.
 * @return	Returns 0 if successful.
 */
int NTP_Request(ntp_timestamp *ts);

/**
 * @brief	Requests a NTP message.
 * @return	Actual time in seconds since January 1st 1900.
 */
uint32_t NTP_RequestSeconds();

/**
 * @brief	Requests a NTP message.
 * @return	Actual time in seconds since January 1st 1970 (Epoch / Unix time).
 */
uint32_t NTP_RequestEpochUnix();

/**
 * @}
 */

#endif /* NTP_H_ */
