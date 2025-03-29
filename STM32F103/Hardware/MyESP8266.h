#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header

#include "main.h"

#include "MySerial.h"

#include "OLED.h"

#define USARTNum_MyESP8266	3

extern uint16_t MyESP8266_Limit_TIM3DIVz;

extern uint32_t MyESP8266_Count_TIM3DIVz;

void MyESP8266_TIM3DIVz(void);

extern char MyMQTT_PUBReport_Str1[];
extern char MyMQTT_PUBReport_Str2[];	

void MyMQTT_PUBReport_Data_Joint(void);

extern uint32_t MyESP8266_Limit_ReaceiverSMInitWairTimeList[];
extern char *MyESP8266_InitRSPList[];
extern char *MyESP8266_InitCMDList[];

extern char MyESP8266_Str_PUBResponse[];
extern int8_t MyESP8266_State_SenderSM;
extern int8_t MyESP8266_State_ReceiverSM;
extern int8_t MyESP8266_InitListIndex;
extern uint32_t Limit_ReceiverSMWaitTime;
extern char Keyword_OK[];
extern char Keyword_ERROR[];
extern char Keyword_MQTTSUBRECV[];

void MyESP8266_SenderSM(void);

void MyESP8266_CommandHandler(void);

extern uint32_t MyESP8266_Count_ReceiverSM;

void MyESP8266_ReceiverSM(void);


#endif
