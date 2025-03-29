#ifndef __MYAQUARIUML_H
#define __MYAQUARIUML_H

#include "stm32f10x.h"                  // Device header

#define GPIO_MyAquariumL		GPIOA
#define GPIO_Pin_MyAquariumL	GPIO_Pin_10

void MyAquariumL_Init(uint16_t TIM_Pulse);

void MyAquariumL_SetPulse(uint16_t TIM_Pulse);

extern float AquariumLVR;

void MyAquariumL_SetVoltageRatio(uint8_t r);

#endif
