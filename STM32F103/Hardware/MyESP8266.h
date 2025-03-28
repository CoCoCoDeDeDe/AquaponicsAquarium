#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"
#include "OLED.h"

#define USARTMyESP8266 USART3

void MyESP8266_ATInit(USART_TypeDef* USARTx);//上电操作

void MyESP8266_FirstInitWiFi(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//基础配置

void MyESP8266_WiFiFirstInit(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//配置WiFi

void MyESP8266_MQTTInit(USART_TypeDef* USARTx);//配置华为云MQTT

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//上报数据
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);
	//转发收到的ESP8266的消息到TXtoPC,主while中当RxFlag[3]==1时触发

void MyESP8266_MQTTExecute(USART_TypeDef* USARTx);//解析串口消息，执行命令

void MyESP8266_MQTTRSP(USART_TypeDef* USARTx);//在执行完命令后发送一条上行响应

void MyESP8266_ATSenderSM(void);

void MyESP8266_ATReceiverSM(void);









#endif
