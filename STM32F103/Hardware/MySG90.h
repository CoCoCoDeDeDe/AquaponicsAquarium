#ifndef __MYSG90_H
#define __MYSG90_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"

#define GPIO_MySG90			GPIOA
#define GPIO_Pin_MySG90		GPIO_Pin_9

void MySG90_Init(uint16_t TIM_Pulse);

void MySG90_SetPulse(uint16_t TIM_Pulse);

#endif
