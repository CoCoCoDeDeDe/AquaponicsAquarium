#ifndef __AT_H
#define __AT_H

#include "common_types.h"

extern wifi_t wifi;

extern char ATCMD_MQTTPUB_UPRSP_main[];
extern char ATCMD_MQTTPUB_UPRSP_body[];

extern const	Msg_KeyWord_t		msg_keywords[];
extern const	Cmd_KeyWord_t	cmd_keywords[];
extern				Cmd_t						cmd;

extern at_sm_status_t at_sm_status;

void AT_Init_Str(void);
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
void AT_SM(at_sm_status_t *status, wifi_t *_wifi, rx3_msg_t *_rx3_msg);

#endif
