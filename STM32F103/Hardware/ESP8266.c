#include "ESP8266.h"

/*定义WiFi宏定义*/
#define WIFI_SSID_LEN	10
#define WIFI_SSID		"321"
#define WIFI_PWD_LEN	10
#define WIFI_PWD		"111000111"

/*定义MQTT宏定义*/
#define MQTT_CLIENTID	"AQAQ25032901_0_0_2025032907"
#define MQTT_USERNAME	"AQAQ25032901"
#define MQTT_PWD		"5eecee18613e8930c19fb93469c89b153fd48a3bb2fb606e1ce6b27fd35af531"
#define MQTT_HOST		"ad0ce5c71f.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define MQTT_PORT		"1883"

/*定义 常规 AT 命令*/
char ATCMD_QuitTransparentTransmission[] = "+++\r\n";	//退出透传模式
char ATCMD_ATE0[] = "ATE0\r\n";				//关闭回显

/*定义 WiFi AT 命令字符串*/
char ATCMD_CWMODE1[] = "AT+CWMODE=1\r\n";	//Station 模式
char ATCMD_CWQAP1[] = "AT+CWQAP=1\r\n";		//断开与 AP 的连接
char ATCMD_CIPMUX0[] = "AT+CIPMUX=0\r\n";	//单连接模式
char ATCMD_CWJAP_C[10 + WIFI_SSID_LEN + WIFI_PWD_LEN] = "AT+CWJAP=";	//配置 WiFi 连接
char ATCMD_CWJAP_Q[] = "AT+CWJAP?\r\n";

/*定义 MQTT AT 命令字符串*/
char ATCMD_MQTTUSERCFG_main[127] = "AT+MQTTUSERCFG=0,1,\"NULL\",\"";//
char ATCMD_MQTTUSERCFG_part1[] = "\",\"";							//
char ATCMD_MQTTUSERCFG_part2[] = "\",0,0,\"\"";						//
char ATCMD_MQTTCLIENTID_main[127] = "AT+MQTTCLIENTID=0,\"";			//
char ATCMD_MQTTCLIENTID_part1[] = "\"";								//
char ATCMD_MQTTCONN_main[127] = "AT+MQTTCONN=0,\"";					//
char ATCMD_MQTTCONN_part1[] = "\",1883,1";							//
char ATCMD_MQTTSUB_main[] = "AT+MQTTSUB=0,\"";						//
char ATCMD_MQTTSUB_part1[] = "\",0";								//
char ATCMD_MQTTreport_main[255] = "AT+MQTTPUB=0,\"$oc/devices/";	//
char ATCMD_MQTTreport_part1[] = "/sys/properties/report\",\"{\"services\":[{\"service_id\":\"All\"\\,\"properties\":";
char ATCMD_MQTTreport_part2[] = "}]}\",0,1";
char ATCMD_MQTTreport_data1[] = "{\"WSD\":4000\\,\"WQSV\":5.00\\,\"SMSV\":5.00\\,\"WT\":125\\,\"WPVR\":100\\,\"APRS\":10\\,\"WHRS\":10}";
char ATCMD_MQTTreport_data2[] = "{\"ISV\":5.00\\,\"ALVR\":100\\,\"PGLVR\":100\\,\"FRS\":10\\,\"AT\":50\\,\"AH\":100}";

/*定义常用字段*/
char ATCMD_PART_CRLF[] = "\r\n";

void ESP8266_Init(void) {
	
	
}








//Serial_Init(USART3, 115200, 0, 0);	//Serial1——ESP8266
//Serial_SendStringV2(USART3, "Serial3_On\r\n");//【Debug】

//6月分配毕设老师
//共18老师
