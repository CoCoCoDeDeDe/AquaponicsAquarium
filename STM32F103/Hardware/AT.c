#include "AT.h"

#include "stm32f10x.h"                  // Device header

#include <string.h>

#include "Array.h"

#include "stdlib.h"

#include "Delay.h"

#include "MySerial.h"

#include "MyFeeder.h"
#include "MyWaterP.h"

#include "MyWaterSS.h"
#include "MyAirS.h"
#include "MyWaterTS.h"

#include "MyWaterQS.h"
#include "MySoilMS.h"
#include "MyIlluminationS.h"

#include "MyADCAndDMA.h"

#include "MyPlantGL.h"
#include "MyAirP.h"
#include "MyWaterH.h"

#include "MyAquariumL.h"

#include "Serial3.h"

#include "math.h"

#include "common_types.h"

/*定义 常规 AT 命令*/
char ATCMD_QuitTT[] = "+++\r\n";	//退出透传模式
char ATCMD_ATE0[] = "ATE0\r\n";		//关闭回显

/*定义 WiFi AT 命令字符串*/
char ATCMD_CWMODE1[] = "AT+CWMODE=1\r\n";	//Station 模式
char ATCMD_CWQAP1[] = "AT+CWQAP=1\r\n";		//断开与 AP 的连接
char ATCMD_CIPMUX0[] = "AT+CIPMUX=0\r\n";	//单连接模式
char ATCMD_CWJAP_C[10 + WIFI_SSID_LEN + WIFI_PWD_LEN] = "AT+CWJAP=";	//配置 WiFi 连接
char ATCMD_CWJAP_Q[] = "AT+CWJAP?\r\n";

/*定义 MQTT AT 命令字符串-USERCFG*/
char ATCMD_MQTTUSERCFG_main[AT_MQTTUSERCFG_LEN]=	//后接MQTT_USERNAME
	"AT+MQTTUSERCFG=0,1,\"NULL\",\"";

/*定义 MQTT AT 命令字符串-CLIENTID*/
char ATCMD_MQTTCLIENTID_main[AT_MQTTCLIENTID_LEN] =	//后接MQTT_CLIENT_ID
	"AT+MQTTCLIENTID=0,\"";

/*定义 MQTT AT 命令字符串-CONN*/
char ATCMD_MQTTCONN_main[AT_MQTTCONN_LEN] = 		//后接MQTT_HOSTNAME
	"AT+MQTTCONN=0,\"";

/*定义 MQTT AT 命令字符串-SUB*/
char ATCMD_MQTTSUB_main[AT_MQTTSUB_LEN] = 			//后接device_id
	"AT+MQTTSUB=0,\"";

/*定义 MQTT AT 命令字符串-RPT*/
char ATCMD_MQTTPUB_RPT_main1[AT_MQTTPUB_RPT_LEN] ;
char ATCMD_MQTTPUB_RPT_main2[AT_MQTTPUB_RPT_LEN] ;
char ATCMD_MQTTPUB_RPT_part1and2[AT_MQTTPUB_RPT_LEN] = ATCMD_MQTTPUB_RPT_part1;
char ATCMD_MQTTPUB_RPT_data1[AT_MQTTPUB_RPT_LEN];			//用于插入数据
char ATCMD_MQTTPUB_RPT_data2[AT_MQTTPUB_RPT_LEN];			//用于插入数据

/*定义 MQTT AT 命令字符串-UpRSP*/
char ATCMD_MQTTPUB_UPRSP_main[AT_MQTTPUB_UpRSP_LEN];
char ATCMD_MQTTPUB_UPRSP_part1and2[AT_MQTTPUB_UpRSP_LEN] = ATCMD_MQTTPUB_UPRSP_part1;

///*定义 AT 常见响应 检测字段*/
//char ATRSP_ERROR[]	= "ERROR";	//ERROR
//char ATRSP_OK[]		= "OK";		//OK
//char ATRSP_NECTED[]	= "NECTED";	//WIFI CONNECTED
//char ATRSP_DISCON[]	= "DISCON";	//WIFI DISCONNECTED

///*定义 下行命令 检测字段*/
//char ATDownCmd_RECV[] = "RECV";	//+MQTTSUBRECV:0,"{topic}",81,{"data"}

//【TODO】定义对应enum
//【WARN】关键词在检测时不考虑关键词的'\0'
/*定义用于检测的关键词 消息类型*/
char KEYW_POWERON[] = "ESP";
char KEYW_OK[] = "OK";
char KEYW_ERROR[] = "ERRO";
char KEYW_WIFI_CONN[] = "FI CO";
char KEYW_WIFI_DISCONN[] = "FI DI";
char KEYW_MQTT_DISCONN[] = "TTDI";
char KEYW_MQTT_CONN_SUCCESS[] = MQTT_HOSTNAME;//用于:1.MQTT连上;2.MQTTCONN?检查
char KEYW_WIFI_CONN_SUCCESS[] = WIFI_SSID;//用于:CWJAP?检查
char KEYW_DOWNCMD[] = "SUBRE";
char KEYW_WIFI_GOTIP[] = "FI GO";

/*定义用于检测的关键词 命令类型*/
char KEYW_WPVR[] = "WPVR";
char KEYW_APRS[] = "APRSC";
char KEYW_WHRS[] = "WHRS";
char KEYW_ALVR[] = "ALVR";
char KEYW_PGLVR[] = "PGLVR";
char KEYW_FT[] = "FT";

/*定义数组用于在ParseMessage函数中按顺序识别对应消息类型并返回消息类型枚举值*/
const Msg_KeyWord_t msg_keywords[] =
{
	/*关键词不包括字符串末尾'\0'*/
	/*高优先级*/
	{KEYW_WIFI_DISCONN, sizeof(KEYW_WIFI_DISCONN) - 1, MSG_WIFI_DISCONN},
	{KEYW_MQTT_DISCONN, sizeof(KEYW_MQTT_DISCONN) - 1, MSG_MQTT_DISCONN},
	{KEYW_WIFI_CONN, sizeof(KEYW_WIFI_CONN) - 1, MSG_WIFI_CONN},
	/*中优先级*/
	{KEYW_POWERON, sizeof (KEYW_POWERON) - 1, MSG_POWERON},
	{KEYW_MQTT_CONN_SUCCESS, sizeof(MQTT_HOSTNAME) - 1, MSG_MQTT_CONN_SUCCESS},
	{KEYW_WIFI_CONN_SUCCESS, sizeof(WIFI_SSID) - 1, MSG_WIFI_CONN_SUCCESS},
	{KEYW_DOWNCMD, sizeof(KEYW_DOWNCMD) - 1, MSG_DOWNCMD},
	{KEYW_WIFI_GOTIP, sizeof(KEYW_WIFI_GOTIP) - 1, MSG_WIFI_GOTIP},
	/*低优先级*/
	/*前面集中消息中可能包含OK或ERROR，为防止误识别降低OK和ERROR的识别优先级*/
	{KEYW_OK, sizeof(KEYW_OK) - 1, MSG_OK},
	{KEYW_ERROR, sizeof(KEYW_ERROR) - 1, MSG_ERROR}
};

uint8_t Msg_KeyWord_t_count = (sizeof(msg_keywords) / sizeof(Msg_KeyWord_t));

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

void AT_Init_Str(void)
{
	/*拼接初始化后不会变动的字符串及字符串片段*/
	//拼接AT+MQTTUSERCFG=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_USERNAME, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part2, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, MQTT_PWD, AT_MQTTUSERCFG_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTUSERCFG_main, ATCMD_MQTTUSERCFG_part3, AT_MQTTUSERCFG_LEN);
	//拼接AT+MQTTCLIENTID=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, MQTT_CLIENT_ID, AT_MQTTCLIENTID_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCLIENTID_main, ATCMD_MQTTCLIENTID_part2, AT_MQTTCLIENTID_LEN);
	//拼接AT+MQTTCONN=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, MQTT_HOSTNAME, AT_MQTTCONN_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTCONN_main, ATCMD_MQTTCONN_part2, AT_MQTTCONN_LEN);
	//拼接AT+MQTTSUB=...
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, MQTT_DEVICE_ID, AT_MQTTSUB_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTSUB_main, ATCMD_MQTTSUB_part2, AT_MQTTSUB_LEN);
	//拼接ATCMD_MQTTPUB_RPT_part1and2，之后作为main的头，后接data1或data2
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_part1and2, MQTT_DEVICE_ID, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_part1and2, ATCMD_MQTTPUB_RPT_part2, AT_MQTTPUB_RPT_LEN);
	//拼接ATCMD_MQTTPUB_UPRSP_part1and2，之后作为main的头，后接request_id
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UPRSP_part1and2, MQTT_DEVICE_ID, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UPRSP_part1and2, ATCMD_MQTTPUB_UPRSP_part2, AT_MQTTPUB_RPT_LEN);
	
}

int8_t AT_Test(void)
{
	char str_tset1[252] = "";
	
	int8_t len;
	
	len = snprintf(str_tset1, 
		sizeof(str_tset1), 
		"{\\\"WSD\\\":%d\\,\\\"WQSV\\\":%d\\,\\\"SMSV\\\":%d\\,\"WT\\\":%d\\,\\\"WPVR\\\":%d\\,\\\"APRS\\\":%d\\,\\\"WHRS\\\":%d}\r\n", 
		WaterSD,			//水面距离，mm
		MyADCAndDMA_Result[1],			//水质传感器电压ADC转换值
		MyADCAndDMA_Result[2],			//土壤湿度传感器电压ADC转换值
		MyWaterTS_Result_12Bit_H7Bit,				//水温，摄氏度
		WaterPVR,			//水泵电压相比满载比值
		AirPRS,			//气泵运行状态
		WaterHRS);			//水温加热器运行状态
	
	if(len < 0 || len > sizeof(str_tset1))
	{
		Serial_SendStringV2(USARTPC, "error");
		return -1;	//格式化字符串后长度错误
	}
	
	Serial_SendStringV2(USARTPC, str_tset1);
	
	return 1;//Succeed
}

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
	memset(ATCMD_MQTTPUB_RPT_main1, 0, AT_MQTTPUB_RPT_LEN);
	
	/*拼接AT+MQTTPUB=...report...(无data和末尾)。
	之后data1\2嵌入数据后将data1/2接上*/
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main1, ATCMD_MQTTPUB_RPT_part1and2, AT_MQTTPUB_RPT_LEN);
	
	len = snprintf(ATCMD_MQTTPUB_RPT_data1, 
		sizeof(ATCMD_MQTTPUB_RPT_data1), 
		"{\\\"WSD\\\":%d\\,\\\"WQSVR\\\":%d\\,\\\"SMSVR\\\":%d\\,\\\"WT\\\":%d\\,\\\"WPVR\\\":%d\\,\\\"APRS\\\":%d\\,\\\"WHRS\\\":%d}", 
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
	
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main1, ATCMD_MQTTPUB_RPT_data1, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main1, ATCMD_MQTTPUB_RPT_part3, AT_MQTTPUB_RPT_LEN);
	
	Serial3_SendString(ATCMD_MQTTPUB_RPT_main1, strlen(ATCMD_MQTTPUB_RPT_main1));
	
	Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main1);
	
	return 1;	//成功
}

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
	memset(ATCMD_MQTTPUB_RPT_main2, 0, AT_MQTTPUB_RPT_LEN);
	
	/*拼接AT+MQTTPUB=...report...(无data和末尾)。
	之后data1\2嵌入数据后将data1/2接上part2*/
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main2, ATCMD_MQTTPUB_RPT_part1and2, AT_MQTTPUB_RPT_LEN);

	len = snprintf(ATCMD_MQTTPUB_RPT_data2, 
		sizeof(ATCMD_MQTTPUB_RPT_data2),
		"{\\\"ISVR\\\":%d\\,\\\"ALVR\\\":%d\\,\\\"PGLVR\\\":%d\\,\\\"FRS\\\":%d\\,\\\"AT\\\":%d\\,\\\"AH\\\":%d}",
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

	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main2, ATCMD_MQTTPUB_RPT_data2, AT_MQTTPUB_RPT_LEN);
	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_RPT_main2, ATCMD_MQTTPUB_RPT_part3, AT_MQTTPUB_RPT_LEN);
	
	Serial3_SendString(ATCMD_MQTTPUB_RPT_main2, strlen(ATCMD_MQTTPUB_RPT_main2));
	
	Serial_SendStringV2(USARTPC, ATCMD_MQTTPUB_RPT_main2);
	
	return 1;	//成功
}

int8_t AT_Report(void)
{
	static int8_t selecter = 0;
	
	switch(selecter)
	{
		case 0:
			AT_Report_1(
				WaterSD, 
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
				AquariumLVR, 
				PlantGLVR, 
				FeederRS, 
				AirT, 
				AirH);
			selecter = 0;
			break;
	}
	return 1;//成功
}


Msg_t_e AT_ParseMessage(const char *msg, uint16_t msg_len, const Msg_KeyWord_t *_msg_keywords)
{
	for(uint8_t i = 0; i < Msg_KeyWord_t_count; i++)//按优先级顺序分析
	{
		const Msg_KeyWord_t *kw = &_msg_keywords[i];	//指针指向全局变量关键字结构体数组内的关键词结构体类型元素
		
		if(strstr((char*)msg, kw->keyword) != NULL)
		{
			return kw->type;		//当前指针指向的关键词结构体变量的存储关键词类型的成员/项
		}
	}
	return MSG_NONE;		//如果匹配到msg的类型则在上面for中就已经return，运行到这里说明没有匹配到msg的类型
}

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
		return CMD_UNKNOWN;
	}
	/*处理后才指向request_id的首地址*/
	request_id_addr_head += 4;
	/*这里获取的是从request_id首开始的第一个','的地址*/
	char *request_id_addr_tail = strstr((char*)request_id_addr_head, ",");
	if(request_id_addr_tail == NULL)
	{
		return CMD_UNKNOWN;
	}
	/*处理后才指向request_id的尾地址，无'\0'*/
	request_id_addr_tail -= 1;
	/*计算request_id的长度*/
	size_t request_id_len = request_id_addr_tail - request_id_addr_head + 1;
	/*利用子串request_id的首地址和长度通过memcpy()将其复制给_cmd->request_id*/
	memcpy(_cmd->request_id, request_id_addr_head, request_id_len);
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
		}
	}
	
	/*note:
	 (char*)(para_name_addr_head + para_name_len)指向的是para_name后紧随的双引号"\""的地址。
	+2是用于跳过冒号":"*/
	char *para_value_addr_head = (char*)(para_name_addr_head + para_name_len + 2);
	/*strstr()从para_value_addr_head开始寻找，界限是主串_msg的'\0'，*/
	char *para_value_addr_tail = strstr((char*)para_value_addr_head, "}");
	if(para_value_addr_tail == NULL) return CMD_UNKNOWN;	//找para_value紧随的"}"失败
	/*将地址从para_value紧随的"}"向前移动到para_value的尾地址*/
	para_value_addr_tail -= 1;
	uint16_t para_value_len = para_value_addr_tail - para_value_addr_head + 1;
	/*利用para_value的首地址和para_value的长度（无'\0'）通过memcpy()复制给_cmd->para_value*/
	memcpy(_cmd->para_value, para_value_addr_head, para_value_len);
	/*到此，_cmd的request_id、type、para_value已经获得*/
	
	
	return CMD_UNKNOWN;	//命令消息中未找到匹配的参数名
}

//int8_t AT_UpResponse(char *request_id)
//{
//	/*拼接AT+MQTTPUB=...response...(无request_id和末尾)。
//	之后从AT串口接受的消息中提取request_id后将其接上再接上part1*/
//	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UPRSP_main, 
//		MQTT_DEVICE_ID, 
//		AT_MQTTPUB_UpRSP_LEN);
//	MyArray_Char_CopyBToATail(ATCMD_MQTTPUB_UPRSP_main, 
//		ATCMD_MQTTPUB_UPRSP_part1, 
//		AT_MQTTPUB_UpRSP_LEN);
//	
//	return 1;
//}






