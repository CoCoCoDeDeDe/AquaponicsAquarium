#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"                  // Device header
#include "main.h"

void ESP8266_Serial_Init(
	uint32_t		USART_BaudRate, 	//115200
	uint8_t			NVIC_IRQChannelPreemptionPriority, 	//0
	uint8_t			NVIC_IRQChannelSubPriority,			//0
	FunctionalState	USART_Cmd_NewState);
void ESP8266_Serial_BuadRateCFG(uint32_t USART_BaudRate);

#define	Serial_Ch_ESP8266 3

extern char ATCMD_MQTTUSERCFG_main[];
extern char ATCMD_MQTTCLIENTID_main[];
extern char ATCMD_MQTTCONN_main[];
extern char ATCMD_MQTTSUB_main[];

void ESP8266_Init_Str(void);

void EPS8266_SM(void);

#endif


