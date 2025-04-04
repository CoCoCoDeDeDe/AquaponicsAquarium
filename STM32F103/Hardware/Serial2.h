#ifndef __SERIAL2_H
#define __SERIAL2_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>
#include "common_types.h"

/*RX2接受消息结构体变量*/
extern rx_msg_t rx2_msg;


void Serial2_Init_All(void);
void Serial2_SendByte(char byte);
int8_t Serial2_SendString(char *msg, uint32_t len);





#endif
