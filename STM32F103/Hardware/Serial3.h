#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "common_types.h"

//extern rx3_msg_t rx3_msg;

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


