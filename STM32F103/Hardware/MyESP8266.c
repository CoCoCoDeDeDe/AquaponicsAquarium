#include "MyESP8266.h"

#define	InitCmdNum	11

typedef enum {
	S_SHUT,	//0
	S_INIT,
	S_ROUTINE,
	S_RSP
} MyESP8266_SenderSM_StateTypeDef;

typedef enum {
	R_SHUT,	//0
	R_INIT,
	R_ROUTINE
} MyESP8266_ReceiverSM_StateTypeDef;


uint16_t MyESP8266_Limit_TIM3DIVz = 100;//100*0.01s=1s

uint32_t MyESP8266_Count_TIM3DIVz = 0;

void MyESP8266_TIM3DIVz(void) {
	MyESP8266_Count_TIM3DIVz++;
	if(MyESP8266_Count_TIM3DIVz >= MyESP8266_Limit_TIM3DIVz) {
		
		MyESP8266_SenderSM();
		
		MyESP8266_Count_TIM3DIVz = 0;
	}
}


char MyMQTT_PUBReport_Str1[256];//以'\0'判断结束和有效内容长度
char MyMQTT_PUBReport_Str2[256];//以'\0'判断结束和有效内容长度

void MyMQTT_PUBReport_Joint(void){
	
	snprintf(MyMQTT_PUBReport_Str1, 256, //'\0'占最后一字节
	"AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":{\"WSD\":%d\\,\"WQSV\":%.2f\\,\"SMSV\":%.2f\\,\"WT\":%d\\,\"WPVR\":%.2f\\,\"APRS\":%d\\,\"WHRS\":%d}}]}\",0,1", 
	WaterSD, //WaterSD
	MyADCAndDMA_Result[1]*434.78, //WaterQSV	//TDS=R*1000/2.3 ppm
	MyADCAndDMA_Result[2]*303.03, //SoilSV	//x=R*1000/3.3
	MyWaterTS_Result_12Bit_H7Bit, //WT
	WaterPVR,	//WPVR
	AirPRS,	//APRS
	WaterHRS	//WHRS
	);
	
	snprintf(MyMQTT_PUBReport_Str2, 256, 
	"AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":{\"ISV\":%.2f\\,\"ALVR\":%.2f\\,\"PGLVR\":%.2f\\,\"FRS\":%d\\,\"AT\":%d\\,\"AH\":%d}}]}\",0,1", 
	MyADCAndDMA_Result[3]*303.03,	//ISV	//x=R*1000/3.3
	AquariumLVR,	//ALVR
	PlantGLVR,	//PGLVR
	FeederRS,	//FRS
	AirT,	//AT
	AirH	//AH
	);
}


uint32_t MyESP8266_Limit_ReaceiverSMInitWairTimeList[InitCmdNum] = {
	3000000,
	3000000,
	3000000,
	3000000,
	3000000,
	3000000,
	10000000,
	3000000,
	3000000,
	3000000,
	3000000
};

char *MyESP8266_InitRSPList[InitCmdNum] = {
	"ready",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK",
	"OK"
};

char *MyESP8266_InitCMDList[InitCmdNum] = {
	"AT+RESTORE",
	"ATE0",
	"AT",
	"AT+CWMODE=1",
	"AT+CWQAP",
	"AT+CIPMUX=0",
	"AT+CWJAP=\"321\",\"111000111",
	"AT+MQTTUSERCFG=0\\,1\\,\"NULL\"\\,\"67e22c942902516e866abb29_AQAQ26032501\"\\,\"536b58686bf2b53f923ef6ceaf0094adfadb2da54911a508560b25398edf9097\"\\,0\\,0\\,\"\"",
	"AT+MQTTCLIENTID=0\\,\"67e22c942902516e866abb29_AQAQ26032501_0_0_2025032710\"",
	"AT+MQTTCONN=0\\,\"ad0ce5c71f.st1.iotda-device.cn-north-4.myhuaweicloud.com\"\\,1883\\,1",
	"AT+MQTTSUB=0\\,\"$oc/devices/67e22c942902516e866abb29_AQAQ26032501/sys/commands/#\"\\,0"
};

char MyESP8266_Str_PUBResponse[] = "AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/commands/response/request_id=af4d23a2-72c0-46aa-8b8b-29f467351c30\",\"{}\",0,1";

int8_t MyESP8266_State_SenderSM		= 0;

int8_t MyESP8266_State_ReceiverSM	= 0;

int8_t MyESP8266_InitListIndex		= 0;

uint32_t Limit_ReceiverSMWaitTime	= 1000000;	//us

char Keyword_OK[]			= "OK";
char Keyword_ERROR[]		= "ERROR";
char Keyword_MQTTSUBRECV[]	= "MQTTSUBRECV";

void MyESP8266_SenderSM(void) {
	switch(MyESP8266_State_SenderSM) {
		case S_SHUT:
			
			break;
		case S_INIT:
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyESP8266_InitCMDList[MyESP8266_InitListIndex]);//发送
		
			MyESP8266_State_ReceiverSM = R_INIT;//开接收
			MyESP8266_State_SenderSM = S_SHUT;//关发送
			
			break;
		case S_ROUTINE:
			
			MyMQTT_PUBReport_Joint();
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyMQTT_PUBReport_Str1);//发送Str1
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyMQTT_PUBReport_Str2);//发送Str2
		
			Serial_SendStringPacketV2(USARTPC, "PUBRPT");
			
			break;
		case S_RSP:
		
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyESP8266_Str_PUBResponse);//发送上行响应
		
			MyESP8266_State_SenderSM = S_ROUTINE;//改日常Report
		
			Serial_SendStringPacketV2(USARTPC, "PUBRSP");
		
			break;
	}
}

void MyESP8266_CommandHandler(void) {//解析Serial_Rx3StringPacket
	
	
}

uint32_t MyESP8266_Count_ReceiverSM = 0;

void MyESP8266_ReceiverSM(void) {
	switch(MyESP8266_State_ReceiverSM) {
		case R_SHUT:
			
			break;
		case R_INIT:
			
			MyESP8266_Count_ReceiverSM ++;
		
			if(MyESP8266_Count_ReceiverSM*1000 >= MyESP8266_Limit_ReaceiverSMInitWairTimeList[MyESP8266_InitListIndex]) {//超时，失败，重发
				
				MyESP8266_State_ReceiverSM				= R_SHUT;//关接收
				MyESP8266_State_SenderSM				= S_INIT;//开发送
				
			} else {//没有超时，可以接收
				
				if(Serial_RxFlag[USARTNum_MyESP8266] == 1) {//有新消息接收完
					
					if(MyArray_IsContain(Serial_Rx3StringPacket, 
						MyESP8266_InitRSPList[MyESP8266_InitListIndex]) == 1) {//有成功响应
							
						if (MyESP8266_InitListIndex >= 10) {//Init发完
							
							MyESP8266_InitListIndex		= 0;//重置Init指令索引,起始没必要
							
							MyESP8266_State_ReceiverSM	= R_ROUTINE;//ROUTINE阶段Receiver常开
							MyESP8266_State_SenderSM	= S_ROUTINE;//开发送
						} else {//Init继续下一条
							
							MyESP8266_InitListIndex		++;//下一条
							
							MyESP8266_State_ReceiverSM	= R_SHUT;//关接收
							MyESP8266_State_SenderSM	= S_INIT;//开发送
						}
					} else {//没有成功响应，失败，重发
						
						MyESP8266_State_ReceiverSM		= R_SHUT;//关接收
						MyESP8266_State_SenderSM		= S_INIT;//开发送
					}
				}
			}
			break;
		case R_ROUTINE:
			if(Serial_RxFlag[USARTNum_MyESP8266] == 1) {
				
				if(MyArray_IsContain(Serial_Rx3StringPacket,
					
					Keyword_OK) == 1) {//上报成功
					
					Serial_SendStringPacketV2(USARTPC, "OK");
					
				} else if(MyArray_IsContain(Serial_Rx3StringPacket,Keyword_ERROR) == 1) {//上报失败
					
					Serial_SendStringPacketV2(USARTPC, "ERROR");
					
				} else if(MyArray_IsContain(Serial_Rx3StringPacket,
					
					Keyword_MQTTSUBRECV) == 1) {//收到命令
					
					MyESP8266_CommandHandler();	//分析和完成命令
					
					MyESP8266_State_SenderSM	= S_RSP;//发上行响应
					
					Serial_SendStringPacketV2(USARTPC, "RECV");
				} 
			}
			break;
	}
}





