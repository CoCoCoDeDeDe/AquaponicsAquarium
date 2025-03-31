#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"                  // Device header
#include "main.h"

#define	Serial_Ch_ESP8266 3

extern char ATCMD_MQTTUSERCFG_main[];
extern char ATCMD_MQTTCLIENTID_main[];
extern char ATCMD_MQTTCONN_main[];
extern char ATCMD_MQTTSUB_main[];

void ESP8266_Init_Str(void);

void EPS8266_SM(void);

#endif


