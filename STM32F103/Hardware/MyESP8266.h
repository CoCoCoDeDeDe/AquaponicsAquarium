#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"
#include "OLED.h"

#define USARTMyESP8266 USART3

void MyESP8266_FirstInit(USART_TypeDef* USARTx);//��������

void MyESP8266_WiFiConnect(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//����WiFi

void MyESP8266_MQTTCFG(USART_TypeDef* USARTx);//���û�Ϊ��MQTT

void MyESP8266_MQTTCONN(USART_TypeDef* USARTx);//����MQTT����

void MyESP8266_MQTTSUB(USART_TypeDef* USARTx);//����������Ϣ����

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//�ϱ�����
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);//ת���յ���ESP8266����Ϣ��TXtoPC,��while�е�RxFlag[3]==1ʱ����

#endif
