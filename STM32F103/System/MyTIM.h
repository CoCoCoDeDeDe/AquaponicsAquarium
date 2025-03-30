#ifndef __MYTIM_H
#define __MYTIM_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "OLED.h"
#include "MySerial.h"

#include "MyWaterSS.h"
#include "MyAirS.h"
#include "MyWaterTS.h"

#include "MyESP8266.h"

extern uint32_t MyTIM_1UpCount;
extern uint32_t MyTIM_2Count;
extern uint32_t MyTIM_3Count;
extern uint32_t MyTIM_4Count;

void MyTIM1_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) ;
void MyTIM2_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);
void MyTIM3_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);
void MyTIM4_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);

void MyTIMx_ENABLECmd(TIM_TypeDef* TIMx);
void MyTIMx_DISABLECmd(TIM_TypeDef* TIMx);

void MyTIM3_DIVx(uint16_t x);
void MyTIM3_DIVy(uint16_t z);

#endif
