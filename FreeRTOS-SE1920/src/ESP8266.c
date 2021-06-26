/*
 * ESP8266.c
 *
 *  Created on: May 14, 2020
 *      Author: Miguelito
 */
#include <ESP8266.h>

#define KEYWORD_NUMBER 7

typedef struct {
	int available;
	char *data;
	int len;
	int lastRead;
} ESP8266_data_t;

ESP8266_data_t read_data;
#define dec_places ((238370 * sizeof(int)) / 98981 + 1)

int keywordLen[KEYWORD_NUMBER] = { 4, 7, 7, 6, 2 };
/*char *keywords[KEYWORD_NUMBER] = {
		"OK\r\n",
		"ERROR\r\n",
		"READY\r\n",
		"FAIL\r\n",
		"> "
};*/
char *keywords[KEYWORD_NUMBER] = {
		"OK",
		"ERROR",
		"READY",
		"FAIL",
		"> ",
		"ALREADY CONNECTED",
		"busy"
};

int ESP8266_Init(int mode)
{
	if(mode < 1 || mode > 3)
		return -4;
	if( !UARTINT_Initialize(115200) )
		return -1;

	read_data.available = 0;
	read_data.data = NULL;

	char buffer[50];
	ESP8266_Cmd("AT");
	if(ESP8266_Read(1000, buffer, 50) != KW_OK_ID)
		return -2;

	ESP8266_Cmd("AT+RESTORE");
	ESP8266_Read(2000, buffer, 50);

	//buffer[20];
	strcpy(buffer, "AT+CWMODE_CUR=");
	char s_mode[2] = {mode + '0', '\0'};
	strcat(buffer, s_mode);
	ESP8266_Cmd(buffer);
	if(ESP8266_Read(2000, buffer, 50) != KW_OK_ID)
		return -5;

	/*
	strcpy(buffer, "AT+CWLAP");
	ESP8266_Cmd(buffer);
	if(ESP8266_Read(5000, buffer, 50) != KW_OK_ID)
		return -6;

	strcpy(buffer, "AT+UART_CUR?");
	ESP8266_Cmd(buffer);
	if(ESP8266_Read(2000, buffer, 50) != KW_OK_ID)
		return -7;*/

	return 0;
}

int ESP8266_ConnectAP(char *ssid, char *pwd)
{
	char buffer[70];
	strcpy(buffer, "AT+CWJAP=\"");
	strcat(buffer, ssid);
	strcat(buffer, "\",\"");
	strcat(buffer, pwd);
	strcat(buffer, "\"");

	ESP8266_Cmd(buffer);
	return ESP8266_Read(15000, buffer, 70);
}

int ESP8266_ConnectedAP()
{
	char data[50];
	ESP8266_Cmd("AT+CIPSTATUS");
	ESP8266_Read(2000, (char*)data, 50);
	if( strstr(data, "STATUS:2") != NULL )
		return 2;
	if( strstr(data, "STATUS:3") != NULL )
		return 3;
	if( strstr(data, "STATUS:4") != NULL )
		return 4;
	if( strstr(data, "STATUS:5") != NULL )
		return 5;
	return -1;
}

int ESP8266_ConnectTCP(char *ip, int port, int keepalive)
{
	char buffer[100];
	char num_buffer[15];
	strcpy(buffer, "AT+CIPSTART=\"TCP\",\"");
	strcat(buffer, ip);
	strcat(buffer, "\",");
	itoa(port, num_buffer, 10);
	strcat(buffer, num_buffer);
	strcat(buffer, ",");
	itoa(keepalive, num_buffer, 10);
	strcat(buffer, num_buffer);

	ESP8266_Cmd(buffer);
	return ESP8266_Read(15000, buffer, 100);
}

int ESP8266_CloseConnection()
{
	char data[50];
	ESP8266_Cmd("AT+CIPCLOSE");
	return ESP8266_Read(2000, (char*)data, 50);
}

int ESP8266_FindKeyword(char *buff, int len){
	/*int counter;
	for(int k = 0; k<KEYWORD_NUMBER; k++){
		counter = 0;
		for(int l = 0; l<keywordLen[k]; l++){
			if(keywords[k][keywordLen[k]-l-1] == buff[len-l-1]){
				++counter;
			}
			else
				break;
		}
		if(counter == keywordLen[k])
			return k;
	}
	return -1;*/
	for(int k = KEYWORD_NUMBER-1; k >= 0; k--){
		if (strstr(buff, keywords[k]) != NULL) {
			return k;
		}
	}
	return -1;
}

void ESP8266_Cmd(char * str)
{
	int len = 0;
	while(str[len] != '\0')
		len++;

	uint8_t cmd[len + 2];
	for(int i = 0; i<len; i++)
		cmd[i] = (uint8_t)str[i];
	cmd[len] = (uint8_t)'\r';
	cmd[len+1] = (uint8_t)'\n';

	UARTINT_WriteBuffer(cmd, len+2);
}

void ESP8266_Rst()
{
	ESP8266_Cmd("AT+RST");
}

int ESP8266_Read(int timeout, char *buffer, int buffer_size)
{
	//printf("\n<esp read_data>\n");
	int ret_val = 0;
	uint32_t bytes = -1;
	int total_bytes = 0;
	char local_buffer;
	int i = 0;
	while( 1 ){ //bytes != 0 /*bytes == buffer_size || bytes == -1*/ /*|| total_bytes < buffer_size*/){
		bytes = UARTINT_ReadBuffer((uint8_t*)&local_buffer, 1, timeout);
		//LCDText_Printf("data = %s\n", buffer);
		printf("%c", local_buffer);
		//fflush(stdout);

		//memset(buffer, 0, buffer_size);
		total_bytes += bytes;
		if (bytes == 0) {
			if(i >= buffer_size) {
				for(int j = 0; j < buffer_size-1; j++) {
					buffer[j] = buffer[j+1];
				}
				buffer[buffer_size-1] = '\0';
			}
			else {
				buffer[i] = '\0';
			}
			if((ret_val = ESP8266_FindKeyword(buffer, total_bytes < buffer_size ? total_bytes : buffer_size)) >= 0){
				//memset(buffer, 0, buffer_size);
				return -ret_val;
			}
			return total_bytes;
		}
		else {
			if(i >= buffer_size) {
				for(int j = 0; j < buffer_size-1; j++) {
					buffer[j] = buffer[j+1];
				}
				buffer[buffer_size-1] = local_buffer;
			}
			else {
				buffer[i] = local_buffer;
			}
		}
		i++;
	}
}


static char ESP8266_readDataBuffer[200];

int ESP8266_ReadData(char *data, int data_size) {
	//printf("\n<esp readData>\n");
	memset(data, 0, data_size);
	if(!read_data.available){
		//printf("\n<esp read_data_new>\n");
		ESP8266_Read(5000, ESP8266_readDataBuffer, 200);

		char * temp_pos, * data_pos;
		if((temp_pos = strstr(ESP8266_readDataBuffer, "+IPD,")) == NULL || (data_pos = strstr(ESP8266_readDataBuffer, ":")) == NULL)
			return -1;
		temp_pos += 5;
		/*
		char buffer[50];
		ESP8266_Read(5000, buffer, 50);

		char * temp_pos, * data_pos;
		if((temp_pos = strstr(buffer, "+IPD,")) == NULL || (data_pos = strstr(buffer, ":")) == NULL)
			return -1;
		temp_pos += 5;
		*/
		char s_received_bytes[5];
		memset(s_received_bytes, 0, 5);
		int received_bytes;
		for(int i = 0; i<(int)(data_pos-temp_pos); i++) {
			s_received_bytes[i] = temp_pos[i];
		}
		received_bytes = atoi(s_received_bytes);

		vPortFree(read_data.data);
		read_data.data = pvPortMalloc(received_bytes * sizeof(char));

		read_data.lastRead = 0;
		read_data.len = received_bytes;
		read_data.available = 1;

		data_pos++;
		for(int i = 0; i<received_bytes; i++) {
			read_data.data[i] = data_pos[i];
		}
	}
	if(read_data.available) {
		int read_offset = read_data.lastRead;
		int i;
		for(i = 0; i<data_size; i++, read_data.lastRead++){
			if(0 >= (read_data.len-read_data.lastRead)) {
				read_data.available = 0;
				break;
			}
			data[i] = read_data.data[read_offset+i];
		}
		//read_data.lastRead += i;
		return i;
	}
	return 0;


}

int ESP8266_Write(void *data, int len)
{
	//printf("\n<esp write_r>\n");
	if(len<=0 || len>2048) return -1;

	char sdata_len[dec_places]; //numero de casas decimais necessárias
	itoa(len, sdata_len, 10);

	char str[dec_places + 12] = {"AT+CIPSEND="};

	strcat(str, sdata_len);


	char buffer[50];
	int retval;

send_command:
	ESP8266_Cmd(str);
	retval = ESP8266_Read(1000, buffer, 50);
	if( retval == KW_busy ) {
		WAIT_Ms(50);
		goto send_command;
	}

	if( retval != KW_SEND_ID)
		return -1;
	//printf("\n<esp write_w>\n");
	UARTINT_WriteBuffer((unsigned char*)data, len);
	return len;
}


