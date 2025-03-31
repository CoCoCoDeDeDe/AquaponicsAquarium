#include "ESP8266.h"

/*���� WiFi AT �궨��*/
#define WIFI_SSID_LEN	10
#define WIFI_SSID		"321"
#define WIFI_PWD_LEN	10
#define WIFI_PWD		"111000111"

/*���� MQTT AT �궨��*/
#define MQTT_CLIENT_ID	"AQAQ25032901_0_0_2025032907"
#define MQTT_USERNAME	"AQAQ25032901"
#define MQTT_PWD		"5eecee18613e8930c19fb93469c89b153fd48a3bb2fb606e1ce6b27fd35af531"
#define MQTT_HOSTNAME	"ad0ce5c71f.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define MQTT_PORT		"1883"
#define MQTT_DEVICE_ID	"AQAQ25032901"
#define AT_MQTTUSERCFG_LEN		127
#define AT_MQTTCLIENTID_LEN		127
#define AT_MQTTCONN_LEN			127
#define AT_MQTTSUB_LEN			127
#define AT_MQTTPUB_RPT_LEN		255
#define AT_MQTTPUB_UpRSP_LEN	127

/*���� ���� AT ����*/
char ATCMD_QuitTT[] = "+++\r\n";	//�˳�͸��ģʽ
char ATCMD_ATE0[] = "ATE0\r\n";		//�رջ���

/*���� WiFi AT �����ַ���*/
char ATCMD_CWMODE1[] = "AT+CWMODE=1\r\n";	//Station ģʽ
char ATCMD_CWQAP1[] = "AT+CWQAP=1\r\n";		//�Ͽ��� AP ������
char ATCMD_CIPMUX0[] = "AT+CIPMUX=0\r\n";	//������ģʽ
char ATCMD_CWJAP_C[10 + WIFI_SSID_LEN + WIFI_PWD_LEN] = "AT+CWJAP=";	//���� WiFi ����
char ATCMD_CWJAP_Q[] = "AT+CWJAP?\r\n";

/*���� MQTT AT �����ַ���*/
char ATCMD_MQTTUSERCFG_main[AT_MQTTUSERCFG_LEN]=	//���MQTT_USERNAME
	"AT+MQTTUSERCFG=0,1,\"NULL\",\"";
#define ATCMD_MQTTUSERCFG_part1 "\",\""				//���MQTT_PWS
#define ATCMD_MQTTUSERCFG_part2	"\",0,0,\"\"\r\n"	//ĩβ

char ATCMD_MQTTCLIENTID_main[AT_MQTTCLIENTID_LEN] =	//���MQTT_CLIENT_ID
	"AT+MQTTCLIENTID=0,\"";
#define ATCMD_MQTTCLIENTID_part1 "\"\r\n"			//ĩβ

char ATCMD_MQTTCONN_main[AT_MQTTCONN_LEN] = 		//���MQTT_HOSTNAME
	"AT+MQTTCONN=0,\"";
#define ATCMD_MQTTCONN_part1 "\",1883,1\r\n"		//ĩβ

char ATCMD_MQTTSUB_main[AT_MQTTSUB_LEN] = 			//���device_id
	"AT+MQTTSUB=0,\"";
#define ATCMD_MQTTSUB_part1 "\",0\r\n"				//ĩβ

char ATCMD_MQTTPUB_RPT_main[AT_MQTTPUB_RPT_LEN] = 	//���device_id
	"AT+MQTTPUB=0,\"$oc/devices/";
#define ATCMD_MQTTPUB_RPT_part1 "/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":"
													//���data1��data2
char ATCMD_MQTTPUB_RPT_data1[] = 			//���ڲ�������
	"{\"WSD\":4000\\,\"WQSV\":5.00\\,\"SMSV\":5.00\\,\"WT\":125\\,\"WPVR\":100\\,\"APRS\":10\\,\"WHRS\":10}";
char ATCMD_MQTTPUB_RPT_data2[] = 			//���ڲ�������
	"{\"ISV\":5.00\\,\"ALVR\":100\\,\"PGLVR\":100\\,\"FRS\":10\\,\"AT\":50\\,\"AH\":100}";
#define ATCMD_MQTTPUB_RPT_part2 "}]}\",0,1\r\n"		//ĩβ

char ATCMD_MQTTPUB_UpRSP_main[AT_MQTTPUB_UpRSP_LEN] = 
	"AT+MQTTPUB=0,\"$oc/devices/";			//���device_id
#define ATCMD_MQTTPUB_UpRSP_part1 "/sys/commands/response/request_id="//���request_id
#define ATCMD_MQTTPUB_UpRSP_part2 "\",\"{}\",0,1\r\n"//ĩβ

/*���� AT ������Ӧ ����ֶ�*/
char ATRSP_ERROR[]	= "ERROR";	//ERROR
char ATRSP_OK[]		= "OK";		//OK
char ATRSP_NECTED[]	= "NECTED";	//WIFI CONNECTED
char ATRSP_DISCON[]	= "DISCON";	//WIFI DISCONNECTED

/*���� �������� ����ֶ�*/
char ATDownCmd_RECV[] = "RECV";	//+MQTTSUBRECV:0,"{topic}",81,{"data"}

/*���峣���ֶ�*/
char ATCMD_PART_CRLF[] = "\r\n";

/*����״̬������*/
typedef enum {
	S_SerialOn,
	S_SerialOn_RECV,
	S_QuitTT_Send,
	S_QuitTT_RECV,
	S_ATE0_Send,
	S_ATE0_RECV,
	S_CWMODE_Send,
	S_CWMODE_RECV,
	S_CWQAP_Send,
	S_CWQAP_RECV,
	S_CWJAPC_Send,
	S_CWJAPC_RECV,
	S_CWJAPQ_Send,
	S_CWJAPQ_RECV,
	S_MQTTUSERCFG_Send,
	S_MQTTUSERCFG_RECV,
	S_MQTTCLIENTID_Send,
	S_MQTTCLIENTID_RECV,
	S_MQTTCONN_Send,
	S_MQTTCONN_RECV,
	S_ROUTINE_1,
	S_ROUTINE_2,
	S_ROUTINE_3,
	S_ROUTINE_4,
	S_ROUTINE_5,
	S_WiFiIssue
} ESP8266_SM_State_t;

int8_t ESP8266_SM_State = S_SerialOn;

void ESP8266_Init_Str(void) {
	
	/*ƴ�ӳ�ʼ���󲻻�䶯���ַ������ַ���Ƭ��*/
	//ƴ��AT+MQTTUSERCFG=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_USERNAME, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part1, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_PWD, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part2, AT_MQTTUSERCFG_LEN);
	//ƴ��AT+MQTTCLIENTID=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, MQTT_CLIENT_ID, AT_MQTTCLIENTID_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, ATCMD_MQTTCLIENTID_part1, AT_MQTTCLIENTID_LEN);
	//ƴ��AT+MQTTCONN=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, MQTT_HOSTNAME, AT_MQTTCONN_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, ATCMD_MQTTCONN_part1, AT_MQTTCONN_LEN);
	//ƴ��AT+MQTTSUB=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, MQTT_DEVICE_ID, AT_MQTTSUB_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, ATCMD_MQTTSUB_part1, AT_MQTTSUB_LEN);
	//ƴ��AT+MQTTPUB=...report...(��data��ĩβ)��֮��dataǱ�����ݺ�data1/2�����ٽ���part2
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main, MQTT_DEVICE_ID, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main, ATCMD_MQTTPUB_RPT_part1, AT_MQTTPUB_RPT_LEN);
	//ƴ��AT+MQTTPUB=...response...(��request_id��ĩβ)��֮���AT���ڽ��ܵ���Ϣ����ȡrequest_id��������ٽ���part1
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UpRSP_main, MQTT_DEVICE_ID, AT_MQTTPUB_UpRSP_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UpRSP_main, ATCMD_MQTTPUB_UpRSP_part1, AT_MQTTPUB_UpRSP_LEN);
	
	
}

void EPS8266_SM(void) {
	const static uint16_t Count_RunTimes_Period = 100;//Count_RunTimes����1�εĵ�Чʱ�䣬��λus
	static uint32_t Count_RunTimes = 0;
	static int8_t IsWiFiConnected = 0;
	static int8_t PowerOnMsg_Count = 0;
	static int8_t WitchReportToSend = -1;
	
	Count_RunTimes ++;
		
	switch(ESP8266_SM_State) {
		case S_SerialOn:
			
			ESP8266_Init_Str();
			
			Serial_Init(USART3, 115200, 0, 0);	//Serial1����ESP8266
			Serial_SendStringV2(USARTPC, "Serial3_On\r\n");//��Debug��
		
			ESP8266_SM_State = S_SerialOn_RECV;
			break;
		
		case S_SerialOn_RECV://��ʼ����ʼ�����û���ϵ��֪ͨ����WiFi���ӳɹ���Ϣ
			if(10000000 <= Count_RunTimes*Count_RunTimes_Period) {//����ܾ�û���յ���Ϣ���ж�Ϊû��������WiFi��û��WiFi����
				Serial_SendStringV2(USARTPC, "State_Serial_NoRSP\r\n");//��Debug������
				IsWiFiConnected = 0;
				ESP8266_SM_State = S_QuitTT_Send;//һ�ɰ���û��WiFi���ô���
			
			} else{
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
					Serial_SendStringV2(USARTPC, Serial_Rx3StringPacket);//��Debug����¼
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_NECTED) == 1) {//WiFi�ɹ�����
						Serial_SendStringV2(USARTPC, "WIFI CONNECTED\r\n");//��Debug����¼
						IsWiFiConnected = 1;
						ESP8266_SM_State = S_MQTTUSERCFG_Send;
						
					} else if(1 <= PowerOnMsg_Count) {//�ж��Ƿ��Ѿ����ܹ��ϵ�֪ͨ
						Serial_SendStringV2(USARTPC, "State_PowerOnCheck_MsgUnknow\r\n");//��Debug������
						
					} else {//�Ʋ��յ���ϢΪ�ϵ�֪ͨ
						PowerOnMsg_Count ++;
					}
					Serial_RxFlag[Serial_Ch_ESP8266] = 0;
				}//δ�յ���Ϣ�������һCount_RunTimes_Period
			}
			break;
			
		case S_QuitTT_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_QuitTT);	//������
			ESP8266_SM_State = S_QuitTT_RECV;				//�»ؿ�ʼ����
			Count_RunTimes = 0;
			break;
		
		case S_QuitTT_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {
				ESP8266_SM_State = S_QuitTT_Send;
				Serial_SendStringV2(USARTPC, "QuitTT_NoRSP\r\n");	//��Debug������
				
				Count_RunTimes = 0;
			} else{
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
					if(MyArray_IsContain(Serial_Rx3StringPacket, "ERROR") == 1) {	//�ɹ�
						ESP8266_SM_State = S_ATE0_Send;
						Serial_SendStringV2(USARTPC, "QuitTTSucced\r\n");	//��Debug��
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, "OK") == 1) {
						ESP8266_SM_State = S_ATE0_Send;	//û�д���͸��ģʽ
						Serial_SendStringV2(USARTPC, "NotInTT\r\n");	//��Debug��
						
					} else {
						ESP8266_SM_State = S_QuitTT_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "QuitTT_RspUnknow\r\n");	//��Debug������
					}
				}
			}
			break;
			
		case S_ATE0_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_ATE0);	//������
			ESP8266_SM_State = S_ATE0_RECV;				//�»ؿ�ʼ����
			Count_RunTimes = 0;
			
			break;
		
		case S_ATE0_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {
				ESP8266_SM_State = S_QuitTT_Send;
				Serial_SendStringV2(USARTPC, "ATE0_NoRSP\r\n");	//��Debug������
				
				Count_RunTimes = 0;
			} else{
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//�ɹ�
						if(IsWiFiConnected == 1) {
							ESP8266_SM_State = S_MQTTUSERCFG_Send;
							
						} else if(IsWiFiConnected == 0) {
							ESP8266_SM_State = S_CWMODE_Send;
							
						} else{
							Serial_SendStringV2(USARTPC, "ParaError_IsWiFiConnected\r\n");	//��Debug������
						}
						ESP8266_SM_State = S_ATE0_Send;
						Serial_SendStringV2(USARTPC, "ATE0_RspOK\r\n");	//��Debug��
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = S_ATE0_Send;	//û�д���͸��ģʽ
						Serial_SendStringV2(USARTPC, "ATE0_RspError\r\n");	//��Debug��
						
					} else {
						ESP8266_SM_State = S_QuitTT_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "ATE0_RspUnknow\r\n");	//��Debug������
					}
				}
			}
			break;
		
		case S_MQTTUSERCFG_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_MQTTUSERCFG_main);	//������
			ESP8266_SM_State = S_MQTTUSERCFG_RECV;	//��һ����ʼ����
			Count_RunTimes = 0;
			break;
		
		case S_MQTTUSERCFG_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//��ʱ��δ����
				ESP8266_SM_State = S_MQTTUSERCFG_Send;	//�ط�
				Serial_SendStringV2(USARTPC, "MQTTUSERCFG_NoRSP\r\n");	//��Debug������
				
				Count_RunTimes = 0;
			} else{	//ʱ��δ�ﵽ����
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {	//�յ���Ϣ
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//�ɹ�
						ESP8266_SM_State = S_MQTTCLIENTID_Send;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = S_MQTTUSERCFG_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTUSERCFG_Error\r\n");	//��Debug������
						
					} else {
						ESP8266_SM_State = S_MQTTUSERCFG_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTUSERCFG_RspUnknow\r\n");	//��Debug������
					}
				}//���غ�����Ϣ�������»غ�
			}
			break;
		
		case S_MQTTCLIENTID_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_MQTTCLIENTID_main);	//������
			ESP8266_SM_State = S_MQTTCLIENTID_RECV;	//��һ����ʼ����
			Count_RunTimes = 0;
			break;
		
		case S_MQTTCLIENTID_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//��ʱ��δ����
				ESP8266_SM_State = S_MQTTCLIENTID_Send;	//�ط�
				Serial_SendStringV2(USARTPC, "MQTTCLIENTID_NoRSP\r\n");	//��Debug������
				
				Count_RunTimes = 0;
			} else{	//ʱ��δ�ﵽ����
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {	//�յ���Ϣ
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//�ɹ�
						ESP8266_SM_State = S_MQTTCONN_Send;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = S_MQTTCLIENTID_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTCLIENTID_ERROR\r\n");	//��Debug������
						
					} else {
						ESP8266_SM_State = S_MQTTCLIENTID_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTCLIENTID_RSPUnKnow\r\n");	//��Debug������
					}
				}//���غ�����Ϣ�������»غ�
			}
			break;
			
		case S_MQTTCONN_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_MQTTCONN_main);	//������
			ESP8266_SM_State = S_MQTTCONN_RECV;	//��һ����ʼ����
			Count_RunTimes = 0;
			break;
		
		case S_MQTTCONN_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//��ʱ��δ����
				ESP8266_SM_State = S_MQTTCONN_Send;	//�ط�
				Serial_SendStringV2(USARTPC, "MQTTCONN_NoRSP\r\n");	//��Debug������
				
				Count_RunTimes = 0;
			} else{	//ʱ��δ�ﵽ����
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {	//�յ���Ϣ
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//�ɹ�
						ESP8266_SM_State = S_ROUTINE_1;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = S_MQTTCONN_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTCONN_ERROR\r\n");	//��Debug������
						
					} else {
						ESP8266_SM_State = S_MQTTCONN_Send;	//�ط�
						Serial_SendStringV2(USARTPC, "MQTTCONN_RSPUnKnow\r\n");	//��Debug������
					}
				}//���غ�����Ϣ�������»غ�
			}
			break;
			
		case S_ROUTINE_1:
			if(IsWiFiConnected == 1) {
				ESP8266_SM_State = S_ROUTINE_2;
				Count_RunTimes = 0;
				
			} else if(IsWiFiConnected == 0) {
				ESP8266_SM_State = S_WiFiIssue;
				Count_RunTimes = 0;
				
			} else {
				Serial_SendStringV2(USARTPC, "IsWiFiConnected_ParaError\r\n");	//��Debug������
			}
			//��һ�غ�
			break;
			
		case S_ROUTINE_2:
			if(3000000 <= Count_RunTimes*Count_RunTimes_Period) {//�����������ϱ�
				//��TODO�����Count_RunTimes�ı仯
				switch(WitchReportToSend) {
					case 1:
						//��TODO��ʵ��ƴ�� report data
						
						Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main);//��test��
						
						WitchReportToSend = 2;
						
						//ESP8266_SM_State = S_ROUTINE_2;//����
						//Count_RunTimes = 0;
						break;
					
					case 2:
						//��TODO��ʵ��ƴ�� report data
						
						Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main);//��test��
						
						WitchReportToSend = 1;
						
						//ESP8266_SM_State = S_ROUTINE_2;//����
						//Count_RunTimes = 0;
						break;
					
					default:
						Serial_SendStringV2(USARTPC, "WitchReportToSend_ParaError\r\n");	//��Debug������
				}
				Count_RunTimes = 0;
				
			} else {
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_DISCON) == 1) {
						Serial_SendStringV2(USARTPC, "WiFi_Disconnect\r\n");	//��Debug������
						IsWiFiConnected = 0;
						ESP8266_SM_State = S_WiFiIssue;
						Count_RunTimes = 0;//תS_WiFiIssue�ر�
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATDownCmd_RECV) == 1){
						//��ȡrequest_id
						//�ж�para_name
						//��ȡpara_value
						//����para_name��para_valueִ������
						//����request_idƴ��������Ӧ
						//����������Ӧ������ESP8266
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1){
						Serial_SendStringV2(USARTPC, "ROUTINE_RxOK\r\n");	//��Debug����¼
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1){
						//��TODO��WiFi��MQTT���Ӽ���
						Serial_SendStringV2(USARTPC, "ROUTINE_RxERROR\r\n");	//��Debug������
						
					} else {
						//��TODO��WiFi��MQTT���Ӽ���
						Serial_SendStringV2(USARTPC, "ROUTINE_RxUnknown\r\n");	//��Debug������
					}
				}//����û�е��� report data ��ʱ����Ҳ����Ϣ�������ظ�
			}
			break;
			
		case S_WiFiIssue:
			if(3000000 <= Count_RunTimes*Count_RunTimes_Period) {//ÿ3s����1��
				Serial_SendStringV2(USARTPC, "S_WiFiIssue_WaitingRx\r\n");	//��Debug������
				Count_RunTimes = 0;
				
			} else{
				if(Serial_RxFlag[Serial_Ch_ESP8266] == 1){
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_NECTED) == 1) {//WiFi����
						IsWiFiConnected = 1;
						Serial_SendStringV2(USARTPC, "WiFiConnected\r\n");	//��Debug����¼
						ESP8266_SM_State = S_ROUTINE_2;//�ָ�����ROUTINE
						Count_RunTimes = 0;//תS_ROUTINE_2�ر�
						
					} else{
						Serial_SendStringV2(USARTPC, "S_WiFiIssue_RxUnknow\r\n");	//��Debug������
					}
					Serial_RxFlag[Serial_Ch_ESP8266] = 0;
				}//���غ�����Ϣ����»غ�
			}
			
			break;
	}
}





//6�·��������ʦ
//��18��ʦ

//����ѧ
//17�ܿ���
//16���������
//����
//���⿨


//��Ŀ����
//16�ܿ���
//17�ܴ�磬��Ŀ���棬һ��һ��
//���ÿ�����˵���һ��¼�����ɹ�չʾ��


