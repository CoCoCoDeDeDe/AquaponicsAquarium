#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"                  // Device header
#include "main.h"

extern int8_t tx3_tc_flag;
extern uint16_t tc_count;
extern uint16_t tst_num1;

void Serial3_Init_All(void);
int8_t Serial3_SendString(char *str, uint16_t str_len);

#endif


