#ifndef __MYAIRP_H
#define __MYAIRP_H

#include "stm32f10x.h"                  // Device header

#define	GPIO_APump	GPIOB
#define PIN_APump	GPIO_Pin_13

extern uint8_t AirPRS;

void MyAirP_Init(void);
void MyAirP_SET(void);
void MyAirP_RESET(void);
void MyAirP_Cmd(BitAction BitVal);
uint8_t MyAirP_GetBitVal(void);

void MyAirP_SetRunStatus(uint8_t rs);

#endif
