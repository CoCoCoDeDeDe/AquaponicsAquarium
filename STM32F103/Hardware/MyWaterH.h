#ifndef __MYWATERH_H
#define __MYWATERH_H

#include "stm32f10x.h"                  // Device header

#define	GPIO_Heater	GPIOB
#define PIN_Heater	GPIO_Pin_12

void MyWaterH_Init(void);
void MyWaterH_SET(void);
void MyWaterH_RESET(void);
void MyWaterH_Cmd(BitAction BitVal);
void MyWaterH_GetBitVal(void);
void MyWaterH_GetBitVal(void);

extern uint8_t WaterHRS;

void MyWaterH_SetRunStatus(uint8_t rs);

#endif
