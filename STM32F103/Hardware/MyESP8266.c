#include "MyESP8266.h"

void MyESP8266_FirstInit(USART_TypeDef* USARTx) {//基础配置
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+RESTORE\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWMODE=1\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWQAP\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CIPMUX=0\r");
}

void MyESP8266_WiFiConnect(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password) {//连接WiFi
	
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP=\"20220821\",\"111000111\"\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP?\r");
}
	
void MyESP8266_MQTTCFG(USART_TypeDef* USARTx);//配置华为云MQTT

void MyESP8266_MQTTCONN(USART_TypeDef* USARTx);//建立MQTT连接

void MyESP8266_MQTTSUB(USART_TypeDef* USARTx);//订阅下行消息主题

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//上报数据
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);//转发收到的ESP8266的消息到TXtoPC,主while中当RxFlag[3]==1时触发







