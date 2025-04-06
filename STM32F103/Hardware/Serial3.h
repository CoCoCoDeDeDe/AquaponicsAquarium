#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "common_types.h"

extern rx3_msg_t rx3_msg;

extern int8_t		tx3_tc_flag;
extern uint16_t	tc_count;

void Serial3_Init_All(int32_t br, uint32_t DMAp, int8_t DMA_pp, int8_t DMA_sp, int8_t USART_pp, int8_t USART_sp);
int8_t Serial3_SendByte(char b);

int8_t Serial3_SendString(char *str, uint16_t str_len);

#endif


