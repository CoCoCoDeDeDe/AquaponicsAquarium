#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "stm32f10x.h"                  // Device header

#include "AT.h"

#include <string.h>

#include "Array.h"

#include "MySerial.h"

#include "math.h"

#include "MyTIM.h"

#include "common_types.h"

/*向前声明*/
typedef enum Msg_t_e Msg_t_e;

#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

/*消息类型枚举*/
typedef enum {
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


/*关键词结构体*/
typedef struct
{
	const char *keyword;
	uint16_t len;
	Msg_t_e type;
} KeyWord_t;

/*rx3接受消息结构体*/
/*note:柔性数组成员（如 char buf[]）必须是结构体的最后一个成员，
且结构体至少有一个其他成员。*/
typedef struct
{
	int8_t idle_flag;
	Msg_t_e msg_type;
	char rx3_buf[RX3_BUF_MAX_SIZE];
} rx3_msg_t;

extern rx3_msg_t rx3_msg;

//extern int8_t rx3_buf_idx;
extern int8_t		rx3_idle_flag;
extern char			rx3_buf[];
//extern char *rx3_buf_arr[];

extern int8_t		tx3_tc_flag;
extern uint16_t	tc_count;
extern uint16_t	tst_num1;

void Serial3_Init_All(void);
int8_t Serial3_SendByte(char b);
int8_t Serial3_SendString(char *str, uint16_t str_len);

#endif


