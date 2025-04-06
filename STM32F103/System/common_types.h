#ifndef __COMMON_TYPES_H
#define __COMMON_TYPES_H

#include "stm32f10x.h"                  // Device header

/*定义Serial3的Tx和Rx的宏定义*/
#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

/*定义Serial2的Tx和Rx的宏定义*/
#define TX2_MSG_LEN_MAX 1024
#define RX2_BUF_MAX_SIZE 512

/*定义 WiFi AT 宏定义*/
#define WIFI_SSID_LEN	20
#define WIFI_SSID		"321"
#define WIFI_PWD_LEN	20
#define WIFI_PWD		"111000111"
#define ATCMD_CWJAP_C_LEN 20+WIFI_SSID_LEN+WIFI_PWD_LEN

typedef enum
{
	CONNECT,
	DISCONNECT
}	connectStatus_t_e;

typedef enum
{
	DIDNOTCFG,
	CFGED
}	configStatus_t_e;

/*定义 WIFI 配置信息结构体
可放入IP【可更新】*/
typedef struct
{
	char		ssid[WIFI_SSID_LEN];
	char		pwd[WIFI_PWD_LEN];
	/*要重新配置WiFi前需重置，按要求配置完WiFi后置1，
	没有更改WiFi需求的情况下一旦连接过WiFi就一直置1*/
	configStatus_t_e			isConfiged;
	connectStatus_t_e		isConnect;
} wifi_t;

/*定义 MQTT AT 宏定义*/
#define MQTT_CLIENT_ID_SIZE	64
#define MQTT_CLIENT_ID	"AQAQ25032901_0_0_2025032907"
#define MQTT_USERNAME_SIZE	64
#define MQTT_USERNAME	"AQAQ25032901"
#define MQTT_PWD_SIZE	124
#define MQTT_PWD		"5eecee18613e8930c19fb93469c89b153fd48a3bb2fb606e1ce6b27fd35af531"
#define MQTT_HOSTNAME_SIZE	124
#define MQTT_HOSTNAME	"ad0ce5c71f.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define MQTT_PORT_SIZE	16
#define MQTT_PORT		"1883"
#define MQTT_DEVICE_ID_SIZE	64
#define MQTT_DEVICE_ID	"AQAQ25032901"

/*定义MQTT配置信息结构体*/
typedef struct
{
    // MQTT客户端ID，用于唯一标识客户端
    char client_id[MQTT_CLIENT_ID_SIZE];
    // MQTT用户名，用于身份验证
    char username[MQTT_USERNAME_SIZE];
    // MQTT密码，用于身份验证
    char pwd[MQTT_PWD_SIZE];
    // MQTT服务器主机名
    char hostname[MQTT_HOSTNAME_SIZE];
    // MQTT服务器端口号
    char port[MQTT_PORT_SIZE];
    // 设备ID，用于标识连接的设备
    char device_id[MQTT_DEVICE_ID_SIZE];
	//MQTT服务连接标志位
	connectStatus_t_e	isMqttConnect;
} mqtt_t;

/*定义命令字符串的define*/
#define ATCMD_MQTTUSERCFG_LEN		127
#define ATCMD_MQTTCLIENTID_LEN		127
#define ATCMD_MQTTCONN_LEN			127
#define ATCMD_MQTTSUB_LEN			127
#define ATCMD_MQTTPUB_RPT_LEN		255
#define ATCMD_MQTTPUB_UPRSP_LEN	127

/*SerialTx消息结构体*/
typedef struct
{
	int8_t		tc;
	char			*msg;
	uint32_t	len;
} tx_msg_t;

/*SerialRx消息结构体*/
typedef struct
{
	char			str[RX2_BUF_MAX_SIZE];		//【WARN】写入前清零
	uint32_t	len;
	int8_t		rc;
} rx_msg_t;

/** 
  * @brief  消息类型枚举
  */
typedef enum
{
	MSG_Default,
	MSG_PowerOn,
	MSG_NONE,
	MSG_BUSY,
	MSG_ATE0_OK,
	MSG_WIFI_CONNECTED,
	MSG_WIFI_GOT_IP,
	MSG_WIFI_DISCONNECTED,
	MSG_ERROR,
	MSG_OK,		//多一些识别标志防止误判
	MSG_MQTTCONN_OK,
	MSG_MQTTDISCONNECTED,
	MSG_MQTTSUBRECV
} Msg_t_e;

/** 
  * @brief  Serial3消息结构体
  */
typedef struct
{
	int8_t			rc_1;		//代表是否有接受完可供使用的字符串
	char			buf[RX3_BUF_MAX_SIZE];
	Msg_t_e		type;
	Msg_t_e		type_2;
}	rx3_msg_t;

/** 
  * @brief  消息识别关键词枚举类型
  */
typedef enum
{
	CMD_UNKNOW,
	CMD_WPVR,
	CMD_APRS,
	CMD_WHRS,
	CMD_ALVR,
	CMD_PGLVR,
	CMD_FT
} Cmd_t_e;

/** 
  * @brief  消息类型类型关键词结构体
  */
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


/** 
  * @brief  AT_SM()的状态枚举类型
  */
typedef enum
{
	/*此状态是为了同步AT_SM与Serial3的开启，防止TIM4开启使得AT_SM也开启，
	但此时Serial3还未开启导致收发不同步*/
	AT_SM_S_Default,
	/*此状态是Serial3开启后，STM32接受ESP8266的上电消息，确认ESP8266的连接状态*/
	AT_SM_S_PowerOn,
	AT_SM_S_ATE0,
	/*此状态是为未来可变wifi配置准备的，手机端更改STM32的WiFi配置信息，
	并让STM32通知ESP8266重新配置和连接WiFi*/
	AT_SM_S_CWJAP_C,
	/*此状态是每次上电后为ESP8266配置MQTT配置信息的第一步，先发送配置信息，
	然后检查接受超时和接受信息，标志记得清零*/
	AT_SM_S_MQTTUSERCFG,
	AT_SM_S_MQTTCLIENTID,
	AT_SM_S_MQTTCONN,
	AT_SM_S_MQTTSUB,		//此后，MQTT配置完成，SM进入日常根据WIFI连接状态控制report状态
	/*此状态的前提是WiFi、MQTT、订阅都已配置且连接成功，
	此状态要report控制运行状态，当WIFI连接则开启report*/
	AT_SM_S_IDLE,
	AT_SM_S_WaitingWiFi
} at_sm_state_t_e;


/** 
  * @brief  表示消息是否已经发送的标志
  */
typedef enum
{
	DIDNOTSEND,
	SENDED
}	isMsgSended_t_e;

/** 
  * @brief  表示是否开启的标志
  */
typedef enum
{
	OFF,
	ON
}	onOrOff_t_e;

/** 
  * @brief  存储AT_SM()信息的结构体
  */
typedef struct
{
	at_sm_state_t_e		state;
	int32_t						runtimes;
	int32_t						runtimes_repriod;
	isMsgSended_t_e	isMsgSended;	//只存储当前状态下应该发送的某个特定的消息，要及时清零。
} at_sm_status_t;


#endif


