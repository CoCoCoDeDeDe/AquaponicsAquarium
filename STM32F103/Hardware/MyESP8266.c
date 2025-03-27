#include "MyESP8266.h"
	
void MyESP8266_ATInit(USART_TypeDef* USARTx) {//上电操作
	
	Serial_SendStringPacketV2(USARTMyESP8266, "ATE0\r");
}

void MyESP8266_FirstInit(USART_TypeDef* USARTx) {//基础配置
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+RESTORE\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWMODE=1\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWQAP\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CIPMUX=0\r");
}

void MyESP8266_FirstInitWiFi(USART_TypeDef* USARTx, 
	char *WiFi_SSID, 
	char *WiFi_Password) {//配置WiFi
	
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP=\"20220821\",\"111000111\"\r");
	Serial_SendStringPacketV2(USARTMyESP8266, "AT+CWJAP?\r");
}

void MyESP8266_MQTTInit(USART_TypeDef* USARTx);//配置华为云MQTT

void MyESP8266_MQTTPUB(USART_TypeDef* USARTx);//上报数据
	
void MyESP8266_Forward(USART_TypeDef* USART_Tx, USART_TypeDef* USAR_Rx);
	//转发收到的ESP8266的消息到TXtoPC,主while中当RxFlag[3]==1时触发

void MyESP8266_MQTTExecute(USART_TypeDef* USARTx);//解析串口消息，执行命令

void MyESP8266_MQTTRSP(USART_TypeDef* USARTx);//在执行完命令后发送一条上行响应


//在初始化/setup代码中，可适当使用delay、while




