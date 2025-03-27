#include "MyESP8266.h"
	
void MyESP8266_ATInit(USART_TypeDef* USARTx) {//�ϵ����
	
	Serial_SendStringPacketV2(USARTMyESP8266, "ATE0\r");
}

void MyESP8266_FirstInit(USART_TypeDef* USARTx) {//��������
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+RESTORE\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWMODE=1\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWQAP\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CIPMUX=0\r");
}

void MyESP8266_FirstInitWiFi(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password) {//����WiFi
	
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP=\"20220821\",\"111000111\"\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP?\r");
}

void MyESP8266_MQTTInit(USART_TypeDef* USARTx);//���û�Ϊ��MQTT

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//�ϱ�����
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);
	//ת���յ���ESP8266����Ϣ��TXtoPC,��while�е�RxFlag[3]==1ʱ����

void MyESP8266_MQTTExecute(USART_TypeDef* USARTx);//����������Ϣ��ִ������

void MyESP8266_MQTTRSP(USART_TypeDef* USARTx);//��ִ�����������һ��������Ӧ


//�ڳ�ʼ��/setup�����У����ʵ�ʹ��delay��while




