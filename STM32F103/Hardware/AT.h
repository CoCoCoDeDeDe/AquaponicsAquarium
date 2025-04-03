#ifndef __AT_H
#define __AT_H

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


#include "math.h"

#include "common_types.h"

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


#include "Serial3.h"

extern Msg_t_e rx3_msg_type;

void AT_Init_Str(void);
int8_t AT_Test(void);
int8_t AT_Report(void);
Msg_t_e ParseMessage(const char *msg, uint16_t msg_len);



#endif
