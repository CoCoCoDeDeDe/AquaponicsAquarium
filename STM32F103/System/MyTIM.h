#ifndef __MYTIM_H
#define __MYTIM_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "OLED.h"
#include "Serial.h"

#include "MyHCSR04.h"
#include "MyDHT11.h"
#include "MyDS18B20.h"

#include "MyAquariumHeater.h"

extern uint32_t MyTIM_1UpCount;
extern uint32_t MyTIM_2Count;
extern uint32_t MyTIM_3Count;
extern uint32_t MyTIM_4Count;

void MyTIM1_Init(void);
void MyTIM2_Init(void);
void MyTIM3_Init(void);
void MyTIM4_Init(void);

void MyTIMx_ENABLECmd(TIM_TypeDef* TIMx);
void MyTIMx_DISABLECmd(TIM_TypeDef* TIMx);

void MyTIM3_DIVx(uint16_t x);
void MyTIM3_DIVy(uint16_t z);

#endif
