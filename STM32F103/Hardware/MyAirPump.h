#ifndef __MYAIRPUMP_H
#define __MYAIRPUMP_H

#include "stm32f10x.h"                  // Device header

#define	GPIO_APump	GPIOB
#define PIN_APump	GPIO_Pin_13

void MyAirPump_Init(void);
void MyAirPump_SET(void);
void MyAirPump_RESET(void);
void MyAirPump_Cmd(BitAction BitVal);
void MyAirPump_GetBitVal(void);

#endif
