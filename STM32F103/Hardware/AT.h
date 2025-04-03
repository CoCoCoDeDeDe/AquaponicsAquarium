#ifndef __AT_H
#define __AT_H

#include "common_types.h"

extern char ATCMD_MQTTPUB_UPRSP_main[];
extern char ATCMD_MQTTPUB_UPRSP_part1and2[] ;

extern const Msg_KeyWord_t msg_keywords[];
extern const Cmd_KeyWord_t cmd_keywords[];
extern Cmd_t cmd;

void AT_Init_Str(void);
int8_t AT_Test(void);
int8_t AT_Report(void);
Msg_t_e AT_ParseMessage(
	const char							*msg,
	uint16_t								msg_len,
	const Msg_KeyWord_t		*_msg_keywords);
Cmd_t_e AT_ParseCmdMsg(
	const char							*_msg,
	uint16_t								msg_len,
	const Cmd_KeyWord_t		*_cmd_keywords,
	Cmd_t									*_cmd);


#endif
