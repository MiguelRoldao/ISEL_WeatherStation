/*
 * ntp.c
 *
 *  Created on: Jun 30, 2020
 *      Author: Miguelito
 */

#include <ntp.h>

uint32_t ntohl(uint32_t netlong) {
	return __builtin_bswap32(netlong);
}

int NTP_Connect()
{
	ESP8266_Cmd("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123");
	char buffer[200];
	return ESP8266_Read(10000, buffer, 200);
}

int NTP_DecodePacket(char *buffer, int buffer_len, int pos, ntp_timestamp *dst)
{
	if(buffer_len < pos + 48)
		return -1;

	dst->txTm_s = (uint32_t)(buffer[pos+40]<<0x18 | buffer[pos+41]<<0x10 | buffer[pos+42]<<0x8 | buffer[pos+43]);
	dst->txTm_f = (uint32_t)(buffer[pos+44]<<0x18 | buffer[pos+45]<<0x10 | buffer[pos+46]<<0x8 | buffer[pos+47]);

	return 0;
}

int NTP_RequestValues(uint8_t li_vn_mode, uint8_t stratum, uint8_t poll, uint8_t precision, ntp_timestamp *ts)
{
	ntp_packet pack;
	pack.li_vn_mode = li_vn_mode;
	pack.stratum = stratum;
	pack.poll = poll;
	pack.precision = precision;

	ESP8266_Write(&pack, sizeof(pack));
	/*
	int buffer_size = 1000;
	char buffer[buffer_size];
	memset(buffer, 0, buffer_size);

	int retval = ESP8266_Read(2000, buffer, buffer_size);
	if(retval >= 0)
		NTP_DecodePacket(buffer, retval, retval-48, ts);
	return retval;*/

	char ntpp[48];
	int retval = ESP8266_ReadData(ntpp, 48);
	if(retval >= 0)
		NTP_DecodePacket(ntpp, 48, 0, ts);
	return retval;
}

int NTP_Request(ntp_timestamp *ts)
{
	int retval = NTP_RequestValues(0xE3, 0, 6, 0xEC, ts);
	return retval;
}

uint32_t NTP_RequestSeconds()
{
	ntp_timestamp ts;
	if(NTP_Request(&ts)>=0)
		return ts.txTm_s;
	return 0x83AA7E81;
}

uint32_t NTP_RequestEpochUnix()
{
	uint32_t retval = NTP_RequestSeconds() - 0x83AA7E80;
	return retval;
}
