#ifndef __PLANTGL_H
#define __PLANTGL_H

#include "stm32f10x.h"                  // Device header

#define GPIO_MyPlantGL		GPIOA
#define GPIO_Pin_MyPlantGL	GPIO_Pin_11

void MyPlantGL_Init(uint16_t TIM_Pulse);

void MyPlantGL_SetPulse(uint16_t TIM_Pulse);

extern float PlantGLVR;

void MyPlantGL_SetVoltageRatio(uint8_t r);

#endif
