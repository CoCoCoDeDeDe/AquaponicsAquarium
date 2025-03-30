#include "MyESP8266.h"

#define USARTNum_MyESP8266	3
#define	InitCmdNum	11
#define MYESP8266_UpRSP_LEN 127
#define MYESP8266_REQUEST_ID_MAXLEN 50
#define MYESP8266_PARA_NAME_MAXLEN 8
#define MYESP8266_PARA_VALUE_STR_MAXLEN 5

typedef struct {
	char request_id[MYESP8266_REQUEST_ID_MAXLEN];
	char para_name[MYESP8266_PARA_NAME_MAXLEN];
	union {
		char str_val[MYESP8266_PARA_VALUE_STR_MAXLEN];
		int int_val;
		float float_val;
	} para_value;
	int8_t para_value_type;	//0:char[];1:int;2:float
} MyESP8266_DownCmd_data_t;

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

const char Keyword_OK[]			= "OK";
const char Keyword_ERROR[]		= "ERROR";
const char Keyword_MQTTSUBRECV[]	= "MQTTSUBRECV";

uint32_t MyESP8266_Limit_ReaceiverSMInitWairTimeList[InitCmdNum] = {
	10000000,
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
	"AT+RESTORE\r\n",
	"ATE0\r\n",
	"AT\r\n",
	"AT+CWMODE=1\r\n",
	"AT+CWQAP\r\n",
	"AT+CIPMUX=0\r\n",
	"AT+CWJAP=\"321\",\"111000111\r\n",	//idx = 6
	"AT+MQTTUSERCFG=0,1,\"NULL\",\"AQAQ25032901\",\"5eecee18613e8930c19fb93469c89b153fd48a3bb2fb606e1ce6b27fd35af531\",0,0,\"\"\r\n",
	"AT+MQTTCLIENTID=0,\"AQAQ25032901_0_0_2025032907\"\r\n",
	"AT+MQTTCONN=0,\"ad0ce5c71f.st1.iotda-device.cn-north-4.myhuaweicloud.com\",1883,1\r\n",
	"AT+MQTTSUB=0,\"$oc/devices/AQAQ25032901/sys/commands/#\",0\r\n"
};

char MyESP8266_UpRSP[MYESP8266_UpRSP_LEN] = 
	"AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/commands/response/request_id=";

char MyESP8266_UpRSP_Tail[20] = 
	"\",\"{}\",0,1\r\n";

int8_t MyESP8266_State_SenderSM		= S_ROUTINE;
int8_t MyESP8266_State_ReceiverSM	= R_ROUTINE;
int8_t MyESP8266_InitListIndex		= 7;

static int8_t Extract_request_id(char *rxstr, char *request_id, uint16_t max_len) {
	
	if(rxstr == NULL || max_len == 0) {
		return -1;	//���������Ч
	}
	
	uint16_t idx_request_id_start = 0;
	uint16_t idx_request_id_end = 0;
	uint16_t request_id_len = 0;
	char eq = '=';
	char quote = '\"';
	
	idx_request_id_start = strcspn(rxstr, &eq);
	if((rxstr[idx_request_id_start] != '=') || (idx_request_id_start >= strlen(rxstr))) {
		return -2;	//��rxstr��δ�ҵ�'='
	}
	idx_request_id_start ++;
	
	idx_request_id_end = idx_request_id_start + strcspn(rxstr + idx_request_id_start, &quote);
	if((rxstr[idx_request_id_end ] != '\"') || (idx_request_id_end >= strlen(rxstr + idx_request_id_start))) {
		return -3;	//��rxstr+idx_para_value_start��δ�ҵ�'\"'
	}
	idx_request_id_end --;
	
	request_id_len = idx_request_id_end - idx_request_id_start + 1;
	if(request_id_len > max_len) {
		return -4;	//��ȡ����request_id���ȴ������Ƴ���
	}
	strncpy(request_id, rxstr + idx_request_id_start, request_id_len);
	request_id[request_id_len] = '\0';
	return 1;
}

static int8_t Extract_para_name(char *rxstr, char *para_name, uint16_t max_len) {
	
	if(rxstr == NULL || max_len == 0) {
		return -1;	//���������Ч
	}
	
	uint16_t para_name_idx_start = 0;
	uint16_t para_name_idx_end = 0;
	uint8_t para_name_len = 0;
	char quote = '\"';
	
	for(uint16_t i = 0; i < 5; i ++) {
		para_name_idx_start = para_name_idx_start + strcspn(rxstr + para_name_idx_start, &quote);
	}
	if(rxstr[para_name_idx_start] != '\"') {
		return -2;	//δ��rxstr���ҵ���5��'\"'
	}
	para_name_idx_start ++;
	
	para_name_idx_end = para_name_idx_start + strcspn(rxstr + para_name_idx_start, &quote);
	if(rxstr[para_name_idx_end] != '\"' || para_name_idx_end>= strlen(rxstr + para_name_idx_start)) {
		return -3;	//δ��rxstr+para_name_idx_start���ҵ�'\"'
	}
	para_name_idx_end --;
	
	para_name_len = para_name_idx_end - para_name_idx_start + 1;
	if(para_name_len > max_len) {
		return -4;	//��ȡ��para_name�ĳ��ȳ�����������
	}
	
	strncpy(para_name, rxstr + para_name_idx_start, para_name_len);
	para_name[para_name_len] = '\0';
	
	return  1;	//�ɹ�
}

static int8_t Extract_para_value(char *rxstr, char *para_value, uint16_t max_len) {//����max_len����'\0'
	
	if(rxstr == NULL || max_len <= 0) {
		return -1; //�����������
	}
	
	uint16_t para_value_idx_start = 0;
	uint16_t para_value_idx_end = 0;
	uint8_t para_value_len = 0;
	char sign_colon = ':';
	char sign_r_bracket = '}';
	
	for(uint8_t i = 0; i < 3; i++) {
		para_value_idx_start = para_value_idx_start + strcspn(rxstr + para_value_idx_start, &sign_colon);
	}
	if(rxstr[para_value_idx_start] == ':') {
		return -2;	//δ��rxstr���ҵ���3��':'
	}
	para_value_idx_start ++;
	
	para_value_idx_end = para_value_idx_start + strcspn(rxstr + para_value_idx_start, &sign_r_bracket);
	if(rxstr[para_value_idx_end] != '}' || para_value_idx_end >= strlen(rxstr + para_value_idx_start)) {
		return -3;	//δ��rxstr+para_value_idx_start���ҵ�'}'
	}
	para_value_idx_end --;
	
	para_value_len = para_value_idx_end - para_value_idx_start + 1;
	if(para_value_len > max_len) {
		return -4;	//��ȡ���Ĳ���ֵ���ȴ��ڴ���ĳ������Ʋ���
	}
	
	strncpy(para_value, rxstr + para_value_idx_start, para_value_len);
	para_value[para_value_len] = '\0';
	return 1;	//�ɹ�
}

static int8_t ExecuteDownCmd(MyESP8266_DownCmd_data_t DownCmd_data) {
	
	if(strncmp(DownCmd_data.para_name, "WPVR", 4)) {
		MyWaterP_SetVoltageRatio(atof(DownCmd_data.para_value.str_val));
		
	}else if(strncmp(DownCmd_data.para_name, "APRS", 4)) {
		MyAirP_SetRunStatus(atoi(DownCmd_data.para_value.str_val));
		
	}else if(strncmp(DownCmd_data.para_name, "WHRS", 4)) {
		MyWaterH_SetRunStatus(atoi(DownCmd_data.para_value.str_val));
		
	}else if(strncmp(DownCmd_data.para_name, "ALVR", 4)) {
		MyAquariumL_SetVoltageRatio(atof(DownCmd_data.para_value.str_val));
		
	}else if(strncmp(DownCmd_data.para_name, "PGLVR", 4)) {
		MyPlantGL_SetVoltageRatio(atof(DownCmd_data.para_value.str_val));
		
	}else if(strncmp(DownCmd_data.para_name, "FT", 4)) {
		MyFeeder_Triger(atoi(DownCmd_data.para_value.str_val));
	}
	return 1;	//�ɹ�
}

void MyESP8266_TIM3DIVz(void) {
	
	static uint16_t MyESP8266_Limit_TIM3DIVz = 300;//100*0.01s=1s
	static uint32_t MyESP8266_Count_TIM3DIVz = 0;
	
	MyESP8266_Count_TIM3DIVz++;
	
	if(MyESP8266_Count_TIM3DIVz >= MyESP8266_Limit_TIM3DIVz) {
		
		MyESP8266_SenderSM();
		
		MyESP8266_Count_TIM3DIVz = 0;
	}
}

void MyESP8266_SenderSM(void) {
	
	static char MyMQTT_PUBReport_Str1[250];//��'\0'�жϽ�������Ч���ݳ���
	static char MyMQTT_PUBReport_Str2[250];//��'\0'�жϽ�������Ч���ݳ���

	switch(MyESP8266_State_SenderSM) {
		case S_SHUT:
			
			break;
		
		case S_INIT:
			//Serial_SendStringPacketV2(USARTPC, "S_INIT_s");//��Debug��
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyESP8266_InitCMDList[MyESP8266_InitListIndex]);//����
		
			MyESP8266_State_ReceiverSM = R_INIT;//������
			MyESP8266_State_SenderSM = S_SHUT;//�ط���
			
			break;
		
		case S_ROUTINE:
			memset(MyMQTT_PUBReport_Str1, 0, sizeof(MyMQTT_PUBReport_Str1));
			memset(MyMQTT_PUBReport_Str2, 0, sizeof(MyMQTT_PUBReport_Str2));
			
			snprintf(MyMQTT_PUBReport_Str1, 255, //'\0'ռ���һ�ֽ�
				"AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":{\"WSD\":%d\\,\"WQSV\":%.2f\\,\"SMSV\":%.2f\\,\"WT\":%d\\,\"WPVR\":%.2f\\,\"APRS\":%d\\,\"WHRS\":%d}}]}\",0,1\r\n", 
				WaterSD, //WaterSD
				MyADCAndDMA_Result[1]*434.78, //WaterQSV	//TDS=R*1000/2.3 ppm
				MyADCAndDMA_Result[2]*303.03, //SoilSV	//x=R*1000/3.3
				MyWaterTS_Result_12Bit_H7Bit, //WT
				WaterPVR,	//WPVR
				AirPRS,	//APRS
				WaterHRS	//WHRS
				);
			
			Delay_s(3);
				
			snprintf(MyMQTT_PUBReport_Str2, 255, 
				"AT+MQTTPUB=0,\"$oc/devices/AQAQ25032901/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":{\"ISV\":%.2f\\,\"ALVR\":%.2f\\,\"PGLVR\":%.2f\\,\"FRS\":%d\\,\"AT\":%d\\,\"AH\":%d}}]}\",0,1\r\n", 
				MyADCAndDMA_Result[3]*303.03,	//ISV	//x=R*1000/3.3
				AquariumLVR,	//ALVR
				PlantGLVR,	//PGLVR
				FeederRS,	//FRS
				AirT,	//AT
				AirH	//AH
				);
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyMQTT_PUBReport_Str1);//����Str1
			
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyMQTT_PUBReport_Str2);//����Str2
		
			//Serial_SendStringPacketV2(USARTPC, "S_ROUTINE_e");//��Debug��
			
			break;
			
		case S_RSP:
			Serial_SendStringPacketV2(
				USARTESP8266,
				MyESP8266_UpRSP);//����������Ӧ
		
			MyESP8266_State_SenderSM = S_ROUTINE;//���ճ�Report
		
			Serial_SendStringPacketV2(USARTPC, "S_RSP_e");//��Debug��
		
			break;
	}
}

void MyESP8266_ReceiverSM(void) {
	
	static uint32_t MyESP8266_Count_ReceiverSM = 0;
	
	switch(MyESP8266_State_ReceiverSM) {
		case R_SHUT:
			
			break;
		
		case R_INIT:
			MyESP8266_Count_ReceiverSM ++;
		
			if(MyESP8266_Count_ReceiverSM*1000 >= MyESP8266_Limit_ReaceiverSMInitWairTimeList[MyESP8266_InitListIndex]) {//��ʱ��ʧ�ܣ��ط�
				Serial_SendStringPacketV2(USARTPC, "R_INIT_OutTime");//��Debug��
				
				MyESP8266_State_ReceiverSM				= R_SHUT;//�ؽ���
				MyESP8266_State_SenderSM				= S_INIT;//������
				
				break;
			}
			//û�г�ʱ�����Խ���
				
			if(Serial_RxFlag[USARTNum_MyESP8266] == 1) {//������Ϣ������
				MyESP8266_Count_ReceiverSM = 0;//���㳬ʱ��ʱ��
				Serial_SendStringPacketV2(USARTPC, Serial_Rx3StringPacket);//��Debug��
				
				if(MyArray_IsContain(Serial_Rx3StringPacket, 
					"OK") == 1) {//�гɹ���Ӧ
					Serial_SendStringPacketV2(USARTPC, "InitRsp");//��Debug��
						
					if (MyESP8266_InitListIndex >= 10) {//Init����
						Serial_SendStringPacketV2(USARTPC, "InitAllOK");//��Debug��
						
						MyESP8266_InitListIndex		= 0;//����Initָ������,��ʼû��Ҫ
						
						MyESP8266_State_ReceiverSM	= R_ROUTINE;//ROUTINE�׶�Receiver����
						MyESP8266_State_SenderSM	= S_ROUTINE;//������
					} else {//Init������һ��
						Serial_SendStringPacketV2(USARTPC, "OneInitOK");//��Debug��
						
						MyESP8266_InitListIndex		++;//��һ��
						
						MyESP8266_State_ReceiverSM	= R_SHUT;//�ؽ���
						MyESP8266_State_SenderSM	= S_INIT;//������
					}
				} else {//û�гɹ���Ӧ��ʧ�ܣ��ط�
					Serial_SendStringPacketV2(USARTPC, "InitRspFailed\r\n");//��Debug��
					
					//MyESP8266_State_ReceiverSM		= R_SHUT;//�ؽ���
					MyESP8266_State_SenderSM		= S_INIT;//������
				}
				Serial_RxFlag[USARTNum_MyESP8266] = 0;//���ESP8266���ڽ��ձ�־λ
			}
			break;
		case R_ROUTINE:
			if(Serial_RxFlag[USARTNum_MyESP8266] == 1) {
				Serial_SendStringPacketV2(USARTPC, Serial_Rx3StringPacket);//��Debug��
				
				if(MyArray_IsContain(Serial_Rx3StringPacket, Keyword_OK) == 1) {//�ϱ��ɹ�
					Serial_SendStringPacketV2(USARTPC, "OK");//��Debug��
					
				} else if(MyArray_IsContain(Serial_Rx3StringPacket,Keyword_ERROR) == 1) {//�ϱ�ʧ��
					Serial_SendStringPacketV2(USARTPC, "ERROR");//��Debug��
					
				} else if(MyArray_IsContain(Serial_Rx3StringPacket, Keyword_MQTTSUBRECV) == 1) {//�յ�����
					Serial_SendStringPacketV2(USARTPC, "RECV_s");//��Debug��
					
					MyESP8266_DownCmd_data_t MyESP8266_DownCmd_data;
						
					Extract_request_id(Serial_Rx3StringPacket, 
						MyESP8266_DownCmd_data.request_id, 
						MYESP8266_REQUEST_ID_MAXLEN);
					
					Extract_para_name(Serial_Rx3StringPacket, 
						MyESP8266_DownCmd_data.para_name, 
						MYESP8266_PARA_NAME_MAXLEN);
					
					Extract_para_value(Serial_Rx3StringPacket, 
						MyESP8266_DownCmd_data.para_value.str_val, 
						MYESP8266_PARA_VALUE_STR_MAXLEN);
					
					MyArray_Char_CopyBToATail(MyESP8266_UpRSP, MyESP8266_DownCmd_data.request_id, sizeof(MyESP8266_UpRSP));
					MyArray_Char_CopyBToATail(MyESP8266_UpRSP, MyESP8266_UpRSP_Tail, sizeof(MyESP8266_UpRSP));
					
					ExecuteDownCmd(MyESP8266_DownCmd_data);
					
					MyESP8266_State_SenderSM = S_RSP;//��������Ӧ
					
				} 
				
			Serial_RxFlag[USARTNum_MyESP8266] = 0;//���ESP8266���ڽ��ձ�־λ
			}
			break;
	}
}











