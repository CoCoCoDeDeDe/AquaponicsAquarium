#include "AT.h"

#include "stm32f10x.h"                  // Device header

#include "common_types.h"

#include <string.h>

#include <math.h>

#include <stdio.h>

#include "Array.h"

#include "stdlib.h"

#include "Delay.h"

#include "MyFeeder.h"
#include "MyWaterP.h"

#include "MyWaterSS.h"
#include "AirS.h"
#include "MyWaterTS.h"

#include "WaterLS.h"
#include "WaterQS.h"
#include "LightS.h"

#include "MyADCAndDMA.h"

#include "PlantGL.h"
#include "AirP.h"
#include "MyWaterH.h"

#include "Serial3.h"
#include "Serial2.h"

#include "math.h"


/*定义MQTT配置信息结构体变量*/
mqtt_t mqtt = {
	MQTT_CLIENT_ID,
	MQTT_USERNAME,
	MQTT_PWD,
	MQTT_HOSTNAME,
	MQTT_PORT,
	MQTT_DEVICE_ID,
	DISCONNECT		//MQTT服务默认未连接
};

/*定义 MQTT 配置信息结构体变量*/
wifi_t wifi = {
	WIFI_SSID,
	WIFI_PWD,
	DIDNOTCFG,
	DISCONNECT				//未连接
};

/** 
  * @brief  创建并初始化存储AT_SM()信息的结构体
  */
at_sm_status_t at_sm_status = 
{
	AT_SM_S_Default,
	0,			//计数器清零
	100,		//目前是TIM4中断周期
	DIDNOTSEND		//默认刚进入第一个要发送消息的状态下时还没有发送特定消息
};

onOrOff_t_e AT_Report_Ctrl = OFF;

/*定义 常规 AT 命令*/
char ATCMD_QuitTT[] = "+++\r\n";	//退出透传模式
char ATCMD_ATE0[] = "ATE0\r\n";		//关闭回显
/*定义 WiFi AT 命令字符串*/
char ATCMD_CWMODE1[] = "AT+CWMODE=1\r\n";	//Station 模式
char ATCMD_CWQAP1[] = "AT+CWQAP=1\r\n";		//断开与 AP 的连接
char ATCMD_CIPMUX0[] = "AT+CIPMUX=0\r\n";	//单连接模式
char ATCMD_CWJAP_C[10 + WIFI_SSID_LEN + WIFI_PWD_LEN] = "AT+CWJAP=";	//配置 WiFi 连接
char ATCMD_CWJAP_Q[] = "AT+CWJAP?\r\n";
/*定义 WIFI AT命令字符*/
#define ATCMD_CWJAP_C_BODY "AT+CWJAP=\"%s\",\"%s\"\r\n"
char ATCMD_CWJAP_C_main[ATCMD_CWJAP_C_LEN] = ATCMD_CWJAP_C_BODY;
/*定义 MQTT AT 命令字符串-USERCFG*/
#define ATCMD_MQTTUSERCFG_BODY "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n"
char ATCMD_MQTTUSERCFG_main[ATCMD_MQTTUSERCFG_LEN];
/*定义 MQTT AT 命令字符串-CLIENTID*/
#define ATCMD_MQTTCLIENTID_BODY "AT+MQTTCLIENTID=0,\"%s\"\r\n"
char ATCMD_MQTTCLIENTID_main[ATCMD_MQTTCLIENTID_LEN];
/*定义 MQTT AT 命令字符串-CONN*/
#define ATCMD_MQTTCONN_BODY "AT+MQTTCONN=0,\"%s\",%s,1\r\n"
char ATCMD_MQTTCONN_main[ATCMD_MQTTCONN_LEN];
/*定义 MQTT AT 命令字符串-SUB*/
#define ATCMD_MQTTSUB_BODY "AT+MQTTSUB=0,\"$oc/devices/%s/sys/commands/#\",0\r\n"
char ATCMD_MQTTSUB_main[ATCMD_MQTTSUB_LEN];
/**定义 MQTT AT 命令字符串-上报信息*/
#define ATCMD_MQTTPUB_RPT_BODY "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"All\\\"\\,\\\"properties\\\":%s}]}\",0,1\r\n"
char ATCMD_MQTTPUB_RPT_body[ATCMD_MQTTPUB_RPT_LEN];
char ATCMD_MQTTPUB_RPT_main1[ATCMD_MQTTPUB_RPT_LEN];
char ATCMD_MQTTPUB_RPT_main2[ATCMD_MQTTPUB_RPT_LEN];
char ATCMD_MQTTPUB_RPT_data1[ATCMD_MQTTPUB_RPT_LEN];
char ATCMD_MQTTPUB_RPT_data2[ATCMD_MQTTPUB_RPT_LEN];
#define ATCMD_MQTTPUB_RPT_DATA_BODY1 "{\\\"WSD\\\":%d\\,\\\"WQSVR\\\":%d\\,\\\"SMSVR\\\":%d\\,\\\"WT\\\":%d\\,\\\"WPVR\\\":%d\\,\\\"APRS\\\":%d\\,\\\"WHRS\\\":%d}"
#define ATCMD_MQTTPUB_RPT_DATA_BODY2 "{\\\"ISVR\\\":%d\\,\\\"ALVR\\\":%d\\,\\\"PGLVR\\\":%d\\,\\\"FRS\\\":%d\\,\\\"AT\\\":%d\\,\\\"AH\\\":%d}"
/*定义 MQTT AT 命令字符串-上行响应*/
#define ATCMD_MQTTPUB_UPRSP_BODY "AT+MQTTPUB=0,\"$oc/devices/%s/sys/commands/response/request_id=%s\",\"{}\",0,1\r\n"
/*body存储的字段是在初始化时根据mqtt或者wifi信息配置固定，
会留下%s等等供后续插入每次可能不同的字段。
其之后使用时整个复制给main，main在使用中会重复清零和复制，
每次main要重置都要从body复制来基础的字段*/
char ATCMD_MQTTPUB_UPRSP_body[ATCMD_MQTTPUB_UPRSP_LEN];
char ATCMD_MQTTPUB_UPRSP_main[ATCMD_MQTTPUB_UPRSP_LEN];

//【WARN】关键词在检测时不考虑关键词的'\0'
/*定义用于检测的关键词 消息类型*/
char KEYW_PowerOn[] = "arch:ESP";
char KEYW_WIFI_CONNECTED[] = "FI CO";
char KEYW_WIFI_GOT_IP[] = "FI GO";
char KEYW_WIFI_DISCONNECTED[] = "FI DIS";
char KEYW_ERROR[] = "ERRO";
char KEYW_OK[] = "OK";
char KEYW_MQTTCONN_OK[] = MQTT_HOSTNAME;
char KEYW_MQTTDISCONNECTED[] = "TTDI";
char KEYW_MQTTSUBRECV[] = "SUBRE";
char KEYW_BUSY[] = "busy";
char KEYW_ATE0_OK[] = "ATE0";

/*定义用于检测的关键词 命令类型*/
char KEYW_WPVR[] = "WPVR";
char KEYW_APRS[] = "APRS";
char KEYW_WHRS[] = "WHRS";
char KEYW_ALVR[] = "ALVR";
char KEYW_PGLVR[] = "PGLVR";
char KEYW_FT[] = "FT";

/*定义数组用于在ParseMessage函数中按顺序识别对应消息类型并返回消息类型枚举值*/
const Msg_KeyWord_t msg_keywords[] =
{
	{KEYW_BUSY,									sizeof(KEYW_BUSY) - 1,										MSG_BUSY},
	{KEYW_OK,										sizeof(KEYW_OK) - 1,											MSG_OK},
	{KEYW_ERROR,								sizeof(KEYW_ERROR) - 1,									MSG_ERROR},
	{KEYW_WIFI_CONNECTED,			sizeof(KEYW_WIFI_CONNECTED) - 1,				MSG_WIFI_CONNECTED},
	{KEYW_WIFI_GOT_IP,						sizeof(KEYW_WIFI_GOT_IP) - 1,							MSG_WIFI_GOT_IP},
	{KEYW_WIFI_DISCONNECTED,		sizeof(KEYW_WIFI_DISCONNECTED) - 1,			MSG_WIFI_DISCONNECTED},
	{KEYW_MQTTCONN_OK,				sizeof(KEYW_MQTTCONN_OK) - 1,					MSG_MQTTCONN_OK},
	{KEYW_MQTTDISCONNECTED,		sizeof(KEYW_MQTTDISCONNECTED) - 1,			MSG_MQTTDISCONNECTED},
	{KEYW_MQTTSUBRECV,					sizeof(KEYW_MQTTSUBRECV) - 1,					MSG_MQTTSUBRECV},
	{KEYW_ATE0_OK,							sizeof(KEYW_ATE0_OK) - 1,								MSG_ATE0_OK},
//	{KEYW_PowerOn,								sizeof(KEYW_PowerOn) - 1,									MSG_PowerOn},
};
uint8_t msg_keywords_count = (sizeof(msg_keywords) / sizeof(Msg_KeyWord_t));

/*定义数组用于按顺序识别对应命令类型并返回命令类型枚举值即识别出要设置的参数，
同时按顺序提取命令参数，同时提取出request_id*/
const Cmd_KeyWord_t cmd_keywords[] = 
{
	{KEYW_WPVR, sizeof(KEYW_WPVR) - 1, CMD_WPVR},
	{KEYW_APRS, sizeof(KEYW_APRS) - 1, CMD_APRS},
	{KEYW_WHRS, sizeof(KEYW_WHRS) - 1, CMD_WHRS},
	{KEYW_ALVR, sizeof(KEYW_ALVR) - 1, CMD_ALVR},
	{KEYW_PGLVR, sizeof(KEYW_PGLVR) - 1, CMD_PGLVR},
	{KEYW_FT, sizeof(KEYW_FT) - 1, CMD_FT}
};
uint8_t Cmd_KeyWord_t_count = (sizeof(cmd_keywords) / sizeof(Cmd_KeyWord_t));

/*定义命令结构体，用于存储一次下行命令的request_id、命令类型、命令参数值*/
Cmd_t cmd;

/*定义常用字段*/
char ATCMD_PART_CRLF[] = "\r\n";

/**
* @name	将AT命令要用到的固定字符串或者字符串的固定字段拼接起来。
* @brief	由于AT命令字符串的部分参数可能在未来进行修改，
*  所以采用将可能要修改的字段与一定不会修改的字段进行分离，
*  并在初始化时按情况进行拼接。
*  整个字符串运行后都不会变动的拼接后存到main字符串中，
*  运行后会需要更改数据或字段的字符的固定部分拼接后存到body字符串中，
*  在要使用时复制给main字符串并嵌入数据或字段
* @param	没有参数，直接调用全局变量并将数据存储到全局变量中。
* @retval	无返回，非复杂程序，主要是简单步骤的多次重复。
*/
void AT_Init_Str(void)
{
	/*拼接AT+CWJAP="WIFISSID","WIFIWPD"=完整命令。初始化后固定长*/
	snprintf(
		ATCMD_CWJAP_C_main,
		ATCMD_CWJAP_C_LEN,
		ATCMD_CWJAP_C_BODY,
		wifi.ssid, 
		wifi.pwd);
//	Serial2_SendString(ATCMD_CWJAP_C_main, strlen(ATCMD_CWJAP_C_main));	//【Debug】
	/*拼接AT+MQTTUSERCFG=完整命令。初始化后固定*/
	snprintf(
		ATCMD_MQTTUSERCFG_main,
		ATCMD_MQTTUSERCFG_LEN,
		ATCMD_MQTTUSERCFG_BODY,
		MQTT_USERNAME,
		MQTT_PWD);
//	Serial2_SendString(ATCMD_MQTTUSERCFG_main, strlen(ATCMD_MQTTUSERCFG_main));	//【Debug】
	/*拼接AT+MQTTCLIENTID=完整命令。初始化后固定*/
	snprintf(
		ATCMD_MQTTCLIENTID_main,
		ATCMD_MQTTCLIENTID_LEN,
		ATCMD_MQTTCLIENTID_BODY,
		mqtt.client_id);
//	Serial2_SendString(ATCMD_MQTTCLIENTID_main, strlen(ATCMD_MQTTCLIENTID_main));	//【Debug】
	/*拼接AT+MQTTCONN=完整命令。初始化后固定*/
	snprintf(
		ATCMD_MQTTCONN_main,
		ATCMD_MQTTCONN_LEN,
		ATCMD_MQTTCONN_BODY,
		mqtt.hostname,
		mqtt.port);
//	Serial2_SendString(ATCMD_MQTTCONN_main, strlen(ATCMD_MQTTCONN_main));	//【Debug】
	/*拼接AT+MQTTSUB=订阅下行命令完整命令。初始化后固定*/
	snprintf(
		ATCMD_MQTTSUB_main,
		ATCMD_MQTTSUB_LEN,
		ATCMD_MQTTSUB_BODY,
		mqtt.device_id);
//	Serial2_SendString(ATCMD_MQTTSUB_main, strlen(ATCMD_MQTTSUB_main));	//【Debug】
	/*拼接AT+MQTTPUB=上报属性不完整命令的头部。初始化后固定*/
	snprintf(
		ATCMD_MQTTPUB_RPT_body,
		ATCMD_MQTTPUB_RPT_LEN,
		ATCMD_MQTTPUB_RPT_BODY,
		mqtt.device_id,
		"%s");
		/*拼接AT+MQTTPUB=上行响应不完整命令的头部。初始化后固定*/
		snprintf(
		ATCMD_MQTTPUB_UPRSP_body,
		ATCMD_MQTTPUB_UPRSP_LEN,
		ATCMD_MQTTPUB_UPRSP_BODY,
		mqtt.device_id,
		"%s");
}	// END OF AT_Init_Str()

/**
* @name	从Serial3发送第1号属性上报消息。
* @brief	传入第1号属性上报消息所需的各种属性数据，先判断传入参数的合法性，
*  然后将属性上报的用于嵌入数据的字段的固定字符串复制到用于嵌入数据的字符串中，
*  并通过snprintf()函数嵌入数据，最后从Serial3发送。【可优化，将2个snprintf合并并优化掉data字符串】
* @param	HC-SR04测量的水面距离传感器的距离，单位是mm。
* @param	水质传感器模拟输出口的电压经过ADC转换后的数值。
* @param	土壤湿度传感器模拟输出口的电压经过ADC转换后的数值。
* @param	DS18B20温度传感器测量的水温数据的整数部分，范围以及单位是-55~125摄氏度。
*  【待测试：零下温度的测量能力】
* @param	水泵的PWM供电的功率百分比的分子。
* @param	气泵的运行状态。
* @param	水加热器的运行状态。
* @retval	运行结果所对应的整数数字。
*/
int8_t AT_Report_1(
	int16_t wsd, 
	int16_t wqsvr, 
	int16_t smsvr, 
	int16_t wt, 
	int16_t wpvr, 
	int16_t aprs, 
	int16_t whrs)
{
	if(wsd < 0) wsd = 0;
	if(4000 < wsd) wsd = 4000;
	
	if(wqsvr < 0) wqsvr = 0;
	if(4095< wqsvr) wqsvr = 4095;
	
	if(smsvr < 0) smsvr = 0;
	if(4095 < smsvr) smsvr = 4095;
	
	if(wt < -55) wt = -55;
	if(125 < wt) wt = 125;
	
	if(wpvr < 0) wpvr = 0;
	if(100 < wpvr) wpvr = 100;
	
	if(aprs < 0) aprs = 0;
	if(10 < aprs) aprs = 10;
	
	if(whrs < 0) whrs = 0;
	if(10 < whrs) whrs = 10;
	
	int16_t len = 0;
	
	/*每次发送新report前重置字符串。防止旧字符串的残留信息未被覆盖导致将残留信息发出。*/
	memset(ATCMD_MQTTPUB_RPT_main1, 0, ATCMD_MQTTPUB_RPT_LEN);
	
	len = snprintf(ATCMD_MQTTPUB_RPT_data1, 
		sizeof(ATCMD_MQTTPUB_RPT_data1), 
		ATCMD_MQTTPUB_RPT_DATA_BODY1, 
		wsd,			//水面距离，mm
		wqsvr,			//水质传感器电压ADC转换值
		smsvr,			//土壤湿度传感器电压ADC转换值
		wt,				//水温，摄氏度
		wpvr,			//水泵电压相比满载比值
		aprs,			//气泵运行状态
		whrs);			//水温加热器运行状态
	
	if(len < 0 || len >= sizeof(ATCMD_MQTTPUB_RPT_data1))
	{
		return -1;	//snprintf转换后长度有问题
	}
	
	snprintf(
	ATCMD_MQTTPUB_RPT_main1,
	ATCMD_MQTTPUB_RPT_LEN,
	ATCMD_MQTTPUB_RPT_body,
	ATCMD_MQTTPUB_RPT_data1);
	
	Serial2_SendString(ATCMD_MQTTPUB_RPT_main1, strlen(ATCMD_MQTTPUB_RPT_main1));//【Debug】
	Serial3_SendString(ATCMD_MQTTPUB_RPT_main1, strlen(ATCMD_MQTTPUB_RPT_main1));
	
	return 1;	//成功
}	// END OF AT_Report_1()

/**
* @name	从Serial3发送第2号属性上报消息。
* @brief	传入第2号属性上报消息所需的各种属性数据，先判断传入参数的合法性，
*  然后将属性上报的用于嵌入数据的字段的固定字符串复制到用于嵌入数据的字符串中，
*  并通过snprintf()函数嵌入数据，最后从Serial3发送。【可优化，将2个snprintf合并并优化掉data字符串】
* @param	光照强度传感器模拟输出口的电压经过ADC转换后的数值。
* @param	鱼缸灯光的PWM供电的功率百分比的分子。
* @param	植物生长等的PWM供电的功率百分比的分子。
* @param	喂食器运行状态对应的整数数字。
* @param	DHT11温湿度传感器测量的空气温度数值，范围0~55摄氏度。
* @param	DHT11温湿度传感器测量的空气温度数值，范围0~100相对湿度。
* @retval	运行结果所对应的整数数字。
*/
int8_t AT_Report_2(
	int16_t isvr, 
	int16_t alvr, 
	int16_t pglvr, 
	int16_t frs,
	int16_t at,
	int16_t ah)
{	
	if(isvr < 0) isvr = 0;
	if(4095 < isvr) isvr = 4095;
	
	if(alvr < 0) alvr = 0;
	if(100 < alvr) alvr = 100;
	
	if(pglvr < 0) pglvr = 0;
	if(100 < pglvr) pglvr = 100;
	
	if(frs < 0) frs = 0;
	if(10 < frs) frs = 10;
	
	if(at < 0) at = 0;
	if(50 < at) at = 50;
	
	if(ah < 0) ah = 0;
	if(100 < ah) ah = 100;
	
	int16_t len = 0;
	
	/*每次发送新report前重置字符串。防止旧字符串的残留信息未被覆盖导致将残留信息发出。*/
	memset(ATCMD_MQTTPUB_RPT_main2, 0, ATCMD_MQTTPUB_RPT_LEN);
	
	/*拼接AT+MQTTPUB=...report...(无data和末尾)。
	之后data1\2嵌入数据后将data1/2接上part2*/
//	memcpy(ATCMD_MQTTPUB_RPT_main2, ATCMD_MQTTPUB_RPT_body, strlen(ATCMD_MQTTPUB_RPT_body) + 1);		//下面snprint()会复制

	len = snprintf(ATCMD_MQTTPUB_RPT_data2, 
		sizeof(ATCMD_MQTTPUB_RPT_data2),
		ATCMD_MQTTPUB_RPT_DATA_BODY2,	//【TODO宏定义化】
		isvr,			//光照传感器电压ADC转换值
		alvr,			//鱼缸灯光电压相比满载比值
		pglvr,		//植物生长等电压相比满载比值
		frs,			//喂食器运行状态
		at,			//空气温度，摄氏度
		ah);			//空气湿度
	
	if(len < 0 || len >= sizeof(ATCMD_MQTTPUB_RPT_data2))
	{
		return -1;	//snprintf转换后长度出问题
	}
	
	snprintf(
		ATCMD_MQTTPUB_RPT_main2,
		ATCMD_MQTTPUB_RPT_LEN,
		ATCMD_MQTTPUB_RPT_body,
		ATCMD_MQTTPUB_RPT_data2);
	
	Serial2_SendString(ATCMD_MQTTPUB_RPT_main2, strlen(ATCMD_MQTTPUB_RPT_main2));	//【Debug】
	Serial3_SendString(ATCMD_MQTTPUB_RPT_main2, strlen(ATCMD_MQTTPUB_RPT_main2));
	
	return 1;	//成功
}	// END OF AT_Report_2()

/**
  * @name  循环调用AT_Report_1和2函数
  * @brief  循环调用AT_Report_1()和AT_Report_2()，将大量参数传入两个函数，
  * 由于ESP8266接受AT消息限制长度，所以要将属性上报消息分两个上传，
  * 为了频繁接受还要让两个消息发送有一定时间间隔，
  * 因此本项目采用每隔固定时间轮流发送两个消息的方式。
  * 会根据运行结果返回响应整数数字。
  * @retval  表示运行结果的整数。
  */
int8_t AT_Report(void)
{
	if(AT_Report_Ctrl == 1)
	{
	static int8_t selecter = 0;
	
	switch(selecter)
	{
		case 0:
			AT_Report_1(
				1,
//				WaterSD, 
				MyADCAndDMA_Result[1], 
				MyADCAndDMA_Result[2], 
				MyWaterTS_Result_12Bit_H7Bit, 
				WaterPVR, 
				AirPRS, 
				WaterHRS);
			selecter = 1;
			break;
		case 1:
			AT_Report_2(
				MyADCAndDMA_Result[3], 
				66, 												//【TODO】
				PlantGLVR, 
				FeederRS, 
				AirT, 
				AirH);
			selecter = 0;
			break;
	}
	return 1;//成功
	}
	return -1;//未允许发送
}	// END OF AT_Report()

/**
  * @name  判断AT消息的类型并返回类型。
  * @brief  将传入的AT消息字符串首地址和传入的消息类型对比关键词结构体数组中的关键词进行一一对比，判断出
  * 传入的AT消息的类型并返回AT消息类型枚举值。
  * @param  要判断类型的AT消息字符串的首地址的指针。
  * @param  AT消息字符的长度，不包括'\0'。
  * @param  存储用于对比和匹配的消息类型结构体变量的数组的首地址指针。
  * @retval  匹配到的AT消息类型，或者表示识别失败的类型。
  */
Msg_t_e AT_ParseMessage(const char *msg, uint16_t msg_len, const Msg_KeyWord_t *_msg_keywords)
{
	for(uint8_t i = 0; i < msg_keywords_count; i++)//按优先级顺序分析
	{
		const Msg_KeyWord_t *kw = &_msg_keywords[i];	//指针指向全局变量关键字结构体数组内的关键词结构体类型元素
		
		if(strstr((char*)msg, kw->keyword) != NULL)
		{
			return kw->type;		//当前指针指向的关键词结构体变量的存储关键词类型的成员/项
		}
	}
	return MSG_NONE;		//如果匹配到msg的类型则在上面for中就已经return，运行到这里说明没有匹配到msg的类型
}	// END OF AT_ParseMessage()

/**
  * @name  提取下行命令AT消息中的关键信息并存储和返回。
  * @brief  将转入的消息与传入的命令关键词数组中的关键词进行逐一寻找，当在传入的消息中找到了对应的关键次则返回
  * 存储在关键词数组中的关键词结构体中的命令类型枚举值。提取并存储消息中的request_id、type、para_value。
  * @param  指向消息字符首地址的指针。
  * @param  消息的字符长度，不包括'\0'。
  * @param  关键词结构体数组的首地址指针
  * @param  命令结构体指针，用于存储从消息中提取出来的request_id字符串、type枚举值、para_value字符串
  * @retval  命令类型枚举值
  */
/*note:通过AT_ParseMessage确定消息是下行命令后，需要从消息中获取request_id并保存
直到命令执行完并拼接到上行响应中发送回去。
然后从消息的request_id后面开始识别命令的类型即命令参数名，
利用strstr()识别命令参数名并返回类型名称字符串首地址，与字符串长度运算后得出参数值字符串首地址。
从参数值字符串首地址开始识别参数值末尾后的第一个字符并得出其地址再运算出参数值字符串长度并
复制到Cmd结构体变量*/
Cmd_t_e AT_ParseCmdMsg(
	const char *_msg,
	uint16_t msg_len,
	const Cmd_KeyWord_t *_cmd_keywords,
	Cmd_t *_cmd)	//传入Cmd_t *_cmd参数时要传入地址
{
	/*先将_cmd指向的cmd中的数据特别是字符串清零*/
	memset(_cmd, 0, sizeof(Cmd_t));
	
	//【TODO】提取request_id
	/*这里获取的是"t_id="的首地址*/
	char *request_id_addr_head = strstr((char*)_msg, "t_id=");	
	if(request_id_addr_head == NULL)
	{
		return CMD_UNKNOW;
	}
	/*处理后才指向request_id的首地址*/
	request_id_addr_head += 5;
	/*这里获取的是从request_id首开始的第一个','的地址*/
	char *request_id_addr_tail = strstr((char*)request_id_addr_head, ",");
	if(request_id_addr_tail == NULL)
	{
		return CMD_UNKNOW;
	}
	/*处理后才指向request_id的尾地址，无'\0'*/
	request_id_addr_tail -= 2;
	/*计算request_id的长度*/
	size_t request_id_len = request_id_addr_tail - request_id_addr_head + 1;
	/*利用子串request_id的首地址和长度通过memcpy()将其复制给_cmd->request_id*/
	memcpy(_cmd->request_id, request_id_addr_head, request_id_len);
	/*给_cmd读取到的request_id的末尾加上'\0'。
	索引request_id_len对应第request_id_len+1位，此处原本不是有效数据，
	是最后一个有效数据的下一个字符*/
	_cmd->request_id[request_id_len] = '\0';
	/*到此，_cmd的request_id已经获得*/
	
	char *para_name_addr_head;
	uint16_t para_name_len = 0;
	
	for(uint8_t i = 0; i < Cmd_KeyWord_t_count; i++)
	{
		/*note:strstr()的主串搜索范围是传入的地址1直到遇到'\0'*/
		para_name_addr_head = strstr((char*)_msg, _cmd_keywords[i].keyword);
		
		if(para_name_addr_head != NULL)
		{//匹配到命令
			/*note:访问结构体指针的数据项用"->"，
			访问结构体变量本体的数据项用'.'*/
			_cmd->type = _cmd_keywords[i].type;
			/*到此，_cmd的request_id、type已经获得*/
			para_name_len = _cmd_keywords[i].len;
			
//			OLED_ShowNum(4, 1, _cmd_keywords[i].len, 8);// 【Debug】
			
			/*【重要】否则for会遍历完，导致已经匹配上的para_name_addr_head
			变NULL，后续代码全失败*/
			break;
		}
	}
	if(para_name_addr_head == NULL)
	{
		return CMD_UNKNOW;	//在_msg中找到参数名
	}
	
	/*note:
	 (char*)(para_name_addr_head + para_name_len)指向的是para_name后紧随的双引号"\""的地址。
	+2是用于跳过冒号":"*/
	char *para_value_addr_head = (char*)(para_name_addr_head + para_name_len + 2);
//	Serial3_SendString((char*)(para_name_addr_head + para_name_len + 2), strlen((char*)(para_name_addr_head + para_name_len + 2)));// 【Debug】
	/*strstr()从para_value_addr_head开始寻找，界限是主串_msg的'\0'，*/
	char *para_value_addr_tail = strstr((char*)para_name_addr_head, "},");
	if(para_value_addr_tail == NULL)
	{
		return CMD_UNKNOW;	//上1行strstr()寻找失败
	}
	/*将地址从para_value紧随的"}"向前移动到para_value的尾地址*/
	para_value_addr_tail -= 1;
	/*计算para_value的字符串长度，不包括'\0'*/
	uint16_t para_value_len = para_value_addr_tail - para_value_addr_head + 1;
	/*利用para_value的首地址和para_value的长度（无'\0'）通过memcpy()复制给_cmd->para_value*/
	memcpy(_cmd->para_value, para_value_addr_head, para_value_len);
	/*给_cmd->para_Value的第para_value_len + 1位写入'\0'*/
	_cmd->para_value[para_value_len] = '\0';
	/*到此，_cmd的request_id、type、para_value已经获得*/
	
	return CMD_UNKNOW;	//命令消息中未找到匹配的参数名
}	//	END OF AT_ParseCmdMsg()

at_sm_status_t at_sm_status;

//【TODO】属性上报默认关闭，MQTT连接且wifi连接的情况下再开启
//【TODO】MQTT重连机制，MQTT连接标志，WIFI连接标志
//【TODO】WIFI重连机制，MQTT连接标志，WIFI连接标志
/*note:自动恢复配置机制：在任何情况下，
如果WIFI连接标志为0，就去连接WIFI，
如果MQTT连接标志为0，就去连接MQTT*/
/**
* @name	负责处理STM32与ESP8266之间AT命令沟通的状态机函数
* @brief		
*  
*  
* @param	
* @param	
* @param	
* @param	
* @retval		
*/
void AT_SM(at_sm_status_t *_status, rx3_msg_t *_rx3_msg, wifi_t *_wifi, mqtt_t *_mqtt)
{
	_status->runtimes++;
	switch(_status->state)
	{
		case AT_SM_S_Default:
//			Serial2_SendString("Debug:\r\nAT_SM_S_Default\r\n", strlen("Debug:\r\nAT_SM_S_Default\r\n"));//【Debug】
			break;
		
		case AT_SM_S_PowerOn:
			/*到了需要判断时间的状态要提前将计时器清零*/
			if(_status->runtimes * _status->runtimes_repriod >= 10000000)
			{/*距离上次几次清零后10,000,000us没有收到消息或收到WiFi连接的消息转去配置MQTT。
				于是转去配置WiFi*/
				_status->state = AT_SM_S_ATE0;
//				Serial3_SendString("Debug:\r\nS_PowerOn TimeOut\r\n", strlen("Debug:\r\nS_PowerOn TimeOut\r\n"));//【Debug】
				_status->runtimes = 0;
				return;
			}
			if(_wifi->isConnect == CONNECT)
			{
				_status->state = AT_SM_S_ATE0;
				_status->isMsgSended = DIDNOTSEND;
				_wifi->isConnect = CONNECT;
//				Serial3_SendString("Debug:\r\nGOTOS_MQTTUSERCFG\r\n", strlen("Debug:\r\nGOTOS_MQTTUSERCFG\r\n"));//【Debug】
				return;
			}
			return;
			
		case AT_SM_S_ATE0:
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_ATE0, strlen(ATCMD_ATE0));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
				_status->runtimes = 0;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 3000000)
			{//超时重发
//				Serial3_SendString("Debug:\r\nS_CWJAP_C TimeOut\r\n", strlen("Debug:\r\nS_CWJAP_C TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_rx3_msg->type_2 == MSG_OK || _rx3_msg->type_2 == MSG_ATE0_OK)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				if(_wifi->isConnect == CONNECT) _status->state = AT_SM_S_MQTTUSERCFG;
				_status->state = AT_SM_S_CWJAP_C;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
			
		case AT_SM_S_CWJAP_C:
			if(_wifi->isConnect == CONNECT)
			{
				_status->state = AT_SM_S_MQTTUSERCFG;
				return;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 20000000)
			{//超时重发
				Serial3_SendString("Debug:\r\nS_CWJAP_C TimeOut\r\n", strlen("Debug:\r\nS_CWJAP_C TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_CWJAP_C_main, strlen(ATCMD_CWJAP_C_main));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
				_status->runtimes = 0;
			}
			if(_rx3_msg->type_2 == MSG_WIFI_CONNECTED ||
				_rx3_msg->type_2 == MSG_WIFI_GOT_IP ||
				_wifi->isConnect == CONNECT)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				_wifi->isConfiged = CFGED;
				_wifi->isConnect = CONNECT;
				_status->state = AT_SM_S_MQTTUSERCFG;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
		
		case AT_SM_S_MQTTUSERCFG:
			if(_wifi->isConnect == DISCONNECT)
			{
				return;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 5000000)
			{//超时重发
//				Serial3_SendString("Debug:\r\nS_MQTTUSERCFG TimeOut\r\n", strlen("Debug:\r\nS_MQTTUSERCFG TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_MQTTUSERCFG_main, strlen(ATCMD_MQTTUSERCFG_main));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
				_status->runtimes = 0;
			}
			if(_rx3_msg->type_2 == MSG_OK)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				_status->state = AT_SM_S_MQTTCLIENTID;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
		
		case AT_SM_S_MQTTCLIENTID:
			if(_wifi->isConnect == DISCONNECT)
			{
				return;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 5000000)
			{//超时重发
//				Serial3_SendString("Debug:\r\nS_MQTTCLIENTID TimeOut\r\n", strlen("Debug:\r\nS_MQTTCLIENTID TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_MQTTCLIENTID_main, strlen(ATCMD_MQTTCLIENTID_main));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
				_status->runtimes = 0;
				/*不retuen，如果退出SM前就接到并解析消息可以直接在下面检查消息类型*/
			}
			if(_rx3_msg->type_2 == MSG_OK)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				_status->state = AT_SM_S_MQTTCONN;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
		
		case AT_SM_S_MQTTCONN:
			if(_wifi->isConnect == DISCONNECT)
			{
				return;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 5000000)
			{//超时重发
//				Serial3_SendString("Debug:\r\nS_MQTTCONN TimeOut\r\n", strlen("Debug:\r\nS_MQTTCONN TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_MQTTCONN_main, strlen(ATCMD_MQTTCONN_main));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
				_status->runtimes = 0;
				/*不retuen，如果退出SM前就接到并解析消息可以直接在下面检查消息类型*/
			}
			if(_rx3_msg->type_2 == MSG_OK || _rx3_msg->type_2 == MSG_MQTTCONN_OK)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				_status->state = AT_SM_S_MQTTSUB;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
			
		case AT_SM_S_MQTTSUB:
			if(_wifi->isConnect == DISCONNECT)
			{
				return;
			}
			if(_status->runtimes * _status->runtimes_repriod >= 5000000)
			{//超时重发
//				Serial3_SendString("Debug:\r\nS_MQTTSUB TimeOut\r\n", strlen("Debug:\r\nS_MQTTSUB TimeOut\r\n"));//【Debug】
				_status->isMsgSended = DIDNOTSEND;
				_status->runtimes = 0;
				return;
			}
			if(_status->isMsgSended == DIDNOTSEND)
			{//没发就发
				Serial3_SendString(ATCMD_MQTTSUB_main, strlen(ATCMD_MQTTSUB_main));
				_rx3_msg->type_2 = MSG_Default;
				_status->isMsgSended = SENDED;
//				_status->runtimes = 0;
				/*不retuen，如果退出SM前就接到并解析消息可以直接在下面检查消息类型*/
				return;
			}
			if(_rx3_msg->type_2 == MSG_OK)
			{//成功则下一步
				_rx3_msg->type_2 = MSG_Default;
				_status->state = AT_SM_S_IDLE;
				AT_Report_Ctrl = ON;
				_status->isMsgSended = DIDNOTSEND;
			return;
			}
			return;
		
		case AT_SM_S_IDLE:
			if(_wifi->isConnect == DISCONNECT)
			{//没网就等
				AT_Report_Ctrl = OFF;
				return;
			}
			if(_wifi->isConnect == CONNECT && _mqtt->isMqttConnect ==CONNECT)
			{
				AT_Report_Ctrl = ON;
				return;
			}
		
		default:
			break;
	}
	
	
}	//	END OF AT_SM()




