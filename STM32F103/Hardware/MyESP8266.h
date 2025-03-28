#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"
#include "OLED.h"

#define USARTMyESP8266 USART3

void MyESP8266_ATInit(USART_TypeDef* USARTx);//�ϵ����

void MyESP8266_FirstInitWiFi(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//��������

void MyESP8266_WiFiFirstInit(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password);//����WiFi

void MyESP8266_MQTTInit(USART_TypeDef* USARTx);//���û�Ϊ��MQTT

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//�ϱ�����
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);
	//ת���յ���ESP8266����Ϣ��TXtoPC,��while�е�RxFlag[3]==1ʱ����

void MyESP8266_MQTTExecute(USART_TypeDef* USARTx);//����������Ϣ��ִ������

void MyESP8266_MQTTRSP(USART_TypeDef* USARTx);//��ִ�����������һ��������Ӧ

void MyESP8266_ATSenderSM(void);

void MyESP8266_ATReceiverSM(void);









#endif
