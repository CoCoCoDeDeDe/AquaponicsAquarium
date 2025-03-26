#ifndef __MYAQUARIUMLIGHT_H
#define __MYAQUARIUMLIGHT_H

#include "stm32f10x.h"                  // Device header


#define GPIO_MyAquariumLight		GPIOA
#define GPIO_Pin_MyAquariumLight	GPIO_Pin_8

void MyAquariumLight_Init(uint16_t TIM_Pulse);

void MyAquariumLight_SetPulse(uint16_t TIM_Pulse);

#endif
