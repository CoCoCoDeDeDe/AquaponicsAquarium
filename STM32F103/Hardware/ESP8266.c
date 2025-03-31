#include "ESP8266.h"

/*定义 WiFi AT 宏定义*/
#define WIFI_SSID_LEN	10
#define WIFI_SSID		"321"
#define WIFI_PWD_LEN	10
#define WIFI_PWD		"111000111"

/*定义 MQTT AT 宏定义*/
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

/*定义 常规 AT 命令*/
char ATCMD_QuitTT[] = "+++\r\n";	//退出透传模式
char ATCMD_ATE0[] = "ATE0\r\n";		//关闭回显

/*定义 WiFi AT 命令字符串*/
char ATCMD_CWMODE1[] = "AT+CWMODE=1\r\n";	//Station 模式
char ATCMD_CWQAP1[] = "AT+CWQAP=1\r\n";		//断开与 AP 的连接
char ATCMD_CIPMUX0[] = "AT+CIPMUX=0\r\n";	//单连接模式
char ATCMD_CWJAP_C[10 + WIFI_SSID_LEN + WIFI_PWD_LEN] = "AT+CWJAP=";	//配置 WiFi 连接
char ATCMD_CWJAP_Q[] = "AT+CWJAP?\r\n";

/*定义 MQTT AT 命令字符串*/
char ATCMD_MQTTUSERCFG_main[AT_MQTTUSERCFG_LEN]=	//后接MQTT_USERNAME
	"AT+MQTTUSERCFG=0,1,\"NULL\",\"";
#define ATCMD_MQTTUSERCFG_part1 "\",\""				//后接MQTT_PWS
#define ATCMD_MQTTUSERCFG_part2	"\",0,0,\"\"\r\n"	//末尾

char ATCMD_MQTTCLIENTID_main[AT_MQTTCLIENTID_LEN] =	//后接MQTT_CLIENT_ID
	"AT+MQTTCLIENTID=0,\"";
#define ATCMD_MQTTCLIENTID_part1 "\"\r\n"			//末尾

char ATCMD_MQTTCONN_main[AT_MQTTCONN_LEN] = 		//后接MQTT_HOSTNAME
	"AT+MQTTCONN=0,\"";
#define ATCMD_MQTTCONN_part1 "\",1883,1\r\n"		//末尾

char ATCMD_MQTTSUB_main[AT_MQTTSUB_LEN] = 			//后接device_id
	"AT+MQTTSUB=0,\"";
#define ATCMD_MQTTSUB_part1 "\",0\r\n"				//末尾

char ATCMD_MQTTPUB_RPT_main[AT_MQTTPUB_RPT_LEN] = 	//后接device_id
	"AT+MQTTPUB=0,\"$oc/devices/";
#define ATCMD_MQTTPUB_RPT_part1 "/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":"
													//后接data1或data2
char ATCMD_MQTTPUB_RPT_data1[] = 			//用于插入数据
	"{\"WSD\":4000\\,\"WQSV\":5.00\\,\"SMSV\":5.00\\,\"WT\":125\\,\"WPVR\":100\\,\"APRS\":10\\,\"WHRS\":10}";
char ATCMD_MQTTPUB_RPT_data2[] = 			//用于插入数据
	"{\"ISV\":5.00\\,\"ALVR\":100\\,\"PGLVR\":100\\,\"FRS\":10\\,\"AT\":50\\,\"AH\":100}";
#define ATCMD_MQTTPUB_RPT_part2 "}]}\",0,1\r\n"		//末尾

char ATCMD_MQTTPUB_UpRSP_main[AT_MQTTPUB_UpRSP_LEN] = 
	"AT+MQTTPUB=0,\"$oc/devices/";			//后接device_id
#define ATCMD_MQTTPUB_UpRSP_part1 "/sys/commands/response/request_id="//后接request_id
#define ATCMD_MQTTPUB_UpRSP_part2 "\",\"{}\",0,1\r\n"//末尾

/*定义 AT 常见响应 检测字段*/
char ATRSP_ERROR[]	= "ERROR";	//ERROR
char ATRSP_OK[]		= "OK";		//OK
char ATRSP_NECTED[]	= "NECTED";	//WIFI CONNECTED
char ATRSP_DISCON[]	= "DISCON";	//WIFI DISCONNECTED

/*定义 下行命令 检测字段*/
char ATDownCmd_RECV[] = "RECV";	//+MQTTSUBRECV:0,"{topic}",81,{"data"}

/*定义常用字段*/
char ATCMD_PART_CRLF[] = "\r\n";

/*定义状态机变量*/
typedef enum {
	State_SerialOn,
	State_SerialOn_RECV,
	State_QuitTT_Send,
	State_QuitTT_RECV,
	State_MQTTUSERCFG_Send,
	State_MQTTUSERCFG_RECV,
	State_MQTTCLIENTID_Send,
	State_MQTTCLIENTID_RECV,
	State_MQTTCONN_Send,
	State_MQTTCONN_RECV,
	State_ROUTINE_1,
	State_ROUTINE_2,
	State_ROUTINE_3,
	State_ROUTINE_4,
	State_ROUTINE_5,
	WiFiIssue
	
} ESP8266_SM_State_t;

int8_t ESP8266_SM_State = State_SerialOn;


static void ESP8266_Init_Str(void) {
	
	/*拼接初始化后不会变动的字符串及字符串片段*/
	//拼接AT+MQTTUSERCFG=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_USERNAME, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part1, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_PWD, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part2, AT_MQTTUSERCFG_LEN);
	//拼接AT+MQTTCLIENTID=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, MQTT_CLIENT_ID, AT_MQTTCLIENTID_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, ATCMD_MQTTCLIENTID_part1, AT_MQTTCLIENTID_LEN);
	//拼接AT+MQTTCONN=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, MQTT_HOSTNAME, AT_MQTTCONN_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, ATCMD_MQTTCONN_part1, AT_MQTTCONN_LEN);
	//拼接AT+MQTTSUB=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, MQTT_DEVICE_ID, AT_MQTTSUB_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, ATCMD_MQTTSUB_part1, AT_MQTTSUB_LEN);
	//拼接AT+MQTTPUB=...report...(无data和末尾)。之后data潜入数据后将data1/2接上再街上part2
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main, MQTT_DEVICE_ID, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main, ATCMD_MQTTPUB_RPT_part1, AT_MQTTPUB_RPT_LEN);
	//拼接AT+MQTTPUB=...response...(无request_id和末尾)。之后从AT串口接受的消息中提取request_id后将其接上再接上part1
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UpRSP_main, MQTT_DEVICE_ID, AT_MQTTPUB_UpRSP_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UpRSP_main, ATCMD_MQTTPUB_UpRSP_part1, AT_MQTTPUB_UpRSP_LEN);
	
	
}

void EPS8266_SM(void) {
	const static uint16_t Count_RunTimes_Period = 100;//Count_RunTimes增加1次的等效时间，单位us
	static uint32_t Count_RunTimes = 0;
	static int8_t IsWiFiConnected = 0;
	static int8_t PowerOnMsg_Count = 0;
	static int8_t WitchReportToSend = -1;
	
	Count_RunTimes ++;
		
	switch(ESP8266_SM_State) {
		case State_SerialOn:
			
			ESP8266_Init_Str();
			
			Serial_Init(USART3, 115200, 0, 0);	//Serial1——ESP8266
			Serial_SendStringV2(USARTPC, "Serial3_On\r\n");//【Debug】
		
			ESP8266_SM_State = State_SerialOn_RECV;
			break;
		
		case State_SerialOn_RECV://初始化后开始检查有没有上电后通知或者WiFi连接成功消息
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {//如果很久没有收到消息，判断为没有连接上WiFi或没有WiFi配置
				Serial_SendStringV2(USARTPC, "State_Serial_NoRSP\r\n");//【Debug】报错
				ESP8266_SM_State = State_QuitTT_Send;//一律按照没有WiFi配置处理
			
			} else{
				if(Serial_RxFlag[3] == 1) {
					Serial_SendStringV2(USARTPC, Serial_Rx3StringPacket);//【Debug】记录
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_NECTED) == 1) {//WiFi成功连接
						Serial_SendStringV2(USARTPC, "WIFI CONNECTED\r\n");//【Debug】记录
						IsWiFiConnected = 1;
						ESP8266_SM_State = State_MQTTUSERCFG_Send;
						
					} else if(1 <= PowerOnMsg_Count) {//判断是否已经接受过上电通知
						Serial_SendStringV2(USARTPC, "State_PowerOnCheck_MsgUnknow\r\n");//【Debug】报错
						
					} else {//推测收到消息为上电通知
						PowerOnMsg_Count ++;
					}
					Serial_RxFlag[3] = 0;
				}//未收到消息则进入下一Count_RunTimes_Period
			}
			break;
			
		case State_QuitTT_Send:
			Serial_SendStringV2(USARTESP8266, ATCMD_QuitTT);	//发命令
			ESP8266_SM_State = State_QuitTT_RECV;				//下回开始接收
			Count_RunTimes = 0;
			break;
		
		case State_QuitTT_RECV:
			
			
			break;
		
		case State_MQTTUSERCFG_Send:
			Serial_SendStringV2(USARTPC, ATCMD_MQTTUSERCFG_main);	//发命令【test】
			ESP8266_SM_State = State_MQTTUSERCFG_RECV;	//下一步开始接收
			Count_RunTimes = 0;
			break;
		
		case State_MQTTUSERCFG_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//长时间未接收
				ESP8266_SM_State = State_MQTTUSERCFG_Send;	//重发
				Serial_SendStringV2(USARTPC, "MQTTUSERCFG_NoRSP\r\n");	//【Debug】报错
				
				Count_RunTimes = 0;
			} else{	//时间未达到上限
				if(Serial_RxFlag[2] == 1) {	//收到消息
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//成功
						ESP8266_SM_State = State_MQTTCLIENTID_Send;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = State_MQTTUSERCFG_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTUSERCFG_ERROR\r\n");	//【Debug】报错
						
					} else {
						ESP8266_SM_State = State_MQTTUSERCFG_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTUSERCFG_RSPUnKnow\r\n");	//【Debug】报错
					}
				}//本回合无消息，进入下回合
			}
			break;
		
		case State_MQTTCLIENTID_Send:
			Serial_SendStringV2(USARTPC, ATCMD_MQTTCLIENTID_main);	//发命令【test】
			ESP8266_SM_State = State_MQTTCLIENTID_RECV;	//下一步开始接收
			Count_RunTimes = 0;
			break;
		
		case State_MQTTCLIENTID_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//长时间未接收
				ESP8266_SM_State = State_MQTTCLIENTID_Send;	//重发
				Serial_SendStringV2(USARTPC, "MQTTCLIENTID_NoRSP\r\n");	//【Debug】报错
				
				Count_RunTimes = 0;
			} else{	//时间未达到上限
				if(Serial_RxFlag[3] == 1) {	//收到消息
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//成功
						ESP8266_SM_State = State_MQTTCONN_Send;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = State_MQTTCLIENTID_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTCLIENTID_ERROR\r\n");	//【Debug】报错
						
					} else {
						ESP8266_SM_State = State_MQTTCLIENTID_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTCLIENTID_RSPUnKnow\r\n");	//【Debug】报错
					}
				}//本回合无消息，进入下回合
			}
			break;
			
		case State_MQTTCONN_Send:
			Serial_SendStringV2(USARTPC, ATCMD_MQTTCONN_main);	//发命令【test】
			ESP8266_SM_State = State_MQTTCONN_RECV;	//下一步开始接收
			Count_RunTimes = 0;
			break;
		
		case State_MQTTCONN_RECV:
			if(5000000 <= Count_RunTimes*Count_RunTimes_Period) {	//长时间未接收
				ESP8266_SM_State = State_MQTTCONN_Send;	//重发
				Serial_SendStringV2(USARTPC, "MQTTCONN_NoRSP\r\n");	//【Debug】报错
				
				Count_RunTimes = 0;
			} else{	//时间未达到上限
				if(Serial_RxFlag[3] == 1) {	//收到消息
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1) {	//成功
						ESP8266_SM_State = State_ROUTINE_1;
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1) {
						ESP8266_SM_State = State_MQTTCONN_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTCONN_ERROR\r\n");	//【Debug】报错
						
					} else {
						ESP8266_SM_State = State_MQTTCONN_Send;	//重发
						Serial_SendStringV2(USARTPC, "MQTTCONN_RSPUnKnow\r\n");	//【Debug】报错
					}
				}//本回合无消息，进入下回合
			}
			break;
			
		case State_ROUTINE_1:
			if(IsWiFiConnected == 1) {
				ESP8266_SM_State = State_ROUTINE_2;
				Count_RunTimes = 0;
				
			} else if(IsWiFiConnected == 0) {
				ESP8266_SM_State = WiFiIssue;
				Count_RunTimes = 0;
				
			} else {
				Serial_SendStringV2(USARTPC, "IsWiFiConnected_ParaError\r\n");	//【Debug】报错
			}
			//下一回合
			break;
			
		case State_ROUTINE_2:
			if(3000000 <= Count_RunTimes*Count_RunTimes_Period) {//属性周期性上报
				//【TODO】检查Count_RunTimes的变化
				switch(WitchReportToSend) {
					case 1:
						//【TODO】实现拼接 report data
						
						Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main);//【test】
						
						WitchReportToSend = 2;
						
						//ESP8266_SM_State = State_ROUTINE_2;//不变
						//Count_RunTimes = 0;
						break;
					
					case 2:
						//【TODO】实现拼接 report data
						
						Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main);//【test】
						
						WitchReportToSend = 1;
						
						//ESP8266_SM_State = State_ROUTINE_2;//不变
						//Count_RunTimes = 0;
						break;
					
					default:
						Serial_SendStringV2(USARTPC, "WitchReportToSend_ParaError\r\n");	//【Debug】报错
				}
				Count_RunTimes = 0;
				
			} else {
				if(Serial_RxFlag[3] == 1) {
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_DISCON) == 1) {
						Serial_SendStringV2(USARTPC, "WiFi_Disconnect\r\n");	//【Debug】报错
						IsWiFiConnected = 0;
						ESP8266_SM_State = WiFiIssue;
						Count_RunTimes = 0;//转WiFiIssue必备
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATDownCmd_RECV) == 1){
						//提取request_id
						//判断para_name
						//提取para_value
						//根据para_name和para_value执行命令
						//根据request_id拼接上行响应
						//发送上行响应到串口ESP8266
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_OK) == 1){
						Serial_SendStringV2(USARTPC, "ROUTINE_RxOK\r\n");	//【Debug】记录
						
					} else if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_ERROR) == 1){
						//【TODO】WiFi、MQTT连接检修
						Serial_SendStringV2(USARTPC, "ROUTINE_RxERROR\r\n");	//【Debug】报错
						
					} else {
						//【TODO】WiFi、MQTT连接检修
						Serial_SendStringV2(USARTPC, "ROUTINE_RxUnknown\r\n");	//【Debug】报错
					}
				}//本轮没有到发 report data 的时机，也无消息，下轮重复
			}
			break;
			
		case WiFiIssue:
			if(3000000 <= Count_RunTimes*Count_RunTimes_Period) {//每3s报错1次
				Serial_SendStringV2(USARTPC, "WiFiIssue_WaitingRx\r\n");	//【Debug】报错
				Count_RunTimes = 0;
				
			} else{
				if(Serial_RxFlag[3] == 1){
					if(MyArray_IsContain(Serial_Rx3StringPacket, ATRSP_NECTED) == 1) {//WiFi连接
						IsWiFiConnected = 1;
						Serial_SendStringV2(USARTPC, "WiFiConnected\r\n");	//【Debug】记录
						ESP8266_SM_State = State_ROUTINE_2;//恢复正常ROUTINE
						Count_RunTimes = 0;//转State_ROUTINE_2必备
						
					} else{
						Serial_SendStringV2(USARTPC, "WiFiIssue_RxUnknow\r\n");	//【Debug】报错
					}
					Serial_RxFlag[3] = 0;
				}//本回合无消息则进下回合
			}
			
			break;
	}
}





//6月分配毕设老师
//共18老师

//经济学
//17周考试
//16周周五最快
//笔试
//答题卡


//项目管理
//16周考试
//17周答辩，项目报告，一组一份
//答辩每个考核点做一个录屏（成功展示）


