#ifndef __MYWATERPUMP_H
#define __MYWATERPUMP_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"

#define GPIO_MyWaterPump		GPIOA
#define GPIO_Pin_MyWaterPump	GPIO_Pin_10

void MyWaterPump_Init(uint16_t TIM_Pulse);

void MyWaterPump_SetPulse(uint16_t TIM_Pulse);

#endif
