#ifndef __MYWATERP_H
#define __MYWATERP_H

#include "stm32f10x.h"                  // Device header

#include "MySerial.h"

#define GPIO_MyWaterP		GPIOA
#define GPIO_Pin_MyWaterP	GPIO_Pin_9

extern float WaterPVR;

void MyWaterP_Init(uint16_t TIM_Pulse);

void MyWaterP_SetPulse(uint16_t TIM_Pulse);

void MyWaterP_SetVoltageRatio(uint8_t r);

#endif
