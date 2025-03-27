#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"
#include "OLED.h"

#define USARTMyESP8266 USART3

void MyESP8266_FirstInit(USART_TypeDef* USARTx);//基础配置

void MyESP8266_WiFiConnect(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//连接WiFi

void MyESP8266_MQTTCFG(USART_TypeDef* USARTx);//配置华为云MQTT

void MyESP8266_MQTTCONN(USART_TypeDef* USARTx);//建立MQTT连接

void MyESP8266_MQTTSUB(USART_TypeDef* USARTx);//订阅下行消息主题

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//上报数据
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);//转发收到的ESP8266的消息到TXtoPC,主while中当RxFlag[3]==1时触发

#endif
