#ifndef __COMMON_TYPES_H
#define __COMMON_TYPES_H

#include "stm32f10x.h"                  // Device header

/*定义Serial3的Tx和Rx的宏定义*/
#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

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

/*定义命令字符串的define*/
#define ATCMD_MQTTUSERCFG_part2 "\",\""				//后接MQTT_PWS
#define ATCMD_MQTTUSERCFG_part3	"\",0,0,\"\"\r\n"	//末尾

#define ATCMD_MQTTCLIENTID_part2 "\"\r\n"			//末尾

#define ATCMD_MQTTCONN_part2 "\",1883,1\r\n"		//末尾

#define ATCMD_MQTTSUB_part2 "\",0\r\n"				//末尾

#define ATCMD_MQTTPUB_RPT_part1 "AT+MQTTPUB=0,\"$oc/devices/"//后接device_id
#define ATCMD_MQTTPUB_RPT_part2 "/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"All\\\"\\,\\\"properties\\\":"//后接data1或data2
#define ATCMD_MQTTPUB_RPT_part3 "}]}\",0,1\r\n"		//末尾

#define ATCMD_MQTTPUB_UPRSP_part1 "AT+MQTTPUB=0,\"$oc/devices/"	//后接device_id
#define ATCMD_MQTTPUB_UPRSP_part2 "/sys/commands/response/request_id="	//后接request_id
#define ATCMD_MQTTPUB_UPRSP_part3 "\",\"{}\",0,1\r\n"//末尾

/*消息类型枚举*/
typedef enum
{
	MSG_NONE,
	MSG_POWERON,
	MSG_OK,
	MSG_ERROR,
	MSG_WIFI_CONN,
	MSG_WIFI_GOTIP,
	MSG_WIFI_DISCONN,
	MSG_MQTT_DISCONN,
	MSG_MQTT_CONN_SUCCESS,
	MSG_WIFI_CONN_SUCCESS,
	MSG_DOWNCMD
} Msg_t_e;

typedef enum
{
	CMD_UNKNOWN,
	CMD_WPVR,
	CMD_APRS,
	CMD_WHRS,
	CMD_ALVR,
	CMD_PGLVR,
	CMD_FT
} Cmd_t_e;

/*消息识别关键词结构体*/
typedef struct
{
	//消息类型的匹配关键词
	const char *keyword;
	
	/*消息类型的匹配关键词的字符长度，不包括'\0'，
	因为strstr()自动忽略子串的'\0'，所以暂时用不上*/
	uint16_t len;
	
	//消息类型的类型枚举值
	Msg_t_e type;
	
} Msg_KeyWord_t;

/*命令识别关键词结构体*/
/*命令识别关键词结构体的变量由命令识别关键词结构体数组进行管理，
每一种命令类型都创建一个Cmd_KeyWord_t类型的变量，
并存入命令识别关键词结构体数组。当确定收到消息为下行命令
并提取完request_id后，有多少种命令即命令识别关键词结构体变量
，就要用strstr()识别多少次，中途若匹配成功则将命令识别关键词结构体变量的
Cmd_t_e type赋值给命令结构体的Cmd_t_e type。命令结构体只有一个，
用于存储一次下行命令的request_id、命令类型、参数值（字符串形式）*/
typedef struct
{
	/*通过strstr()依次比较各种命令，每次比较中keyword用于作为
	子串，匹配成功，则将正在指向的Cmd_KeyWord_t类型变量的
	type就是收到的命令的类型。匹配成功后将其返回或者保存至
	Cmd_t类型变量的type数据项中*/
	const char *keyword;
	uint16_t len;
	Cmd_t_e type;
} Cmd_KeyWord_t;

/*命令结构体*/
typedef struct
{
	char request_id[64];
	
	/*note:rstr()识别出命令类型后就不用提取参数名字符串了，
	已经确定了para_value怎么使用。strstr()返回的参数名字符串首地址用于
	与参数名长度和参数名与参数值的间隔运算出参数值的首地址。*/
	Cmd_t_e type;
	
	char para_value[10];
} Cmd_t;








#endif
