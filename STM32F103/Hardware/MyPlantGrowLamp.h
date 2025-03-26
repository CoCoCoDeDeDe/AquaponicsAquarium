#ifndef __MYPLANTGROWLAMP_H
#define __MYPLANTGROWLAMP_H

#include "stm32f10x.h"                  // Device header

#define	GPIO_PGLamp	GPIOB
#define PIN_PGLamp	GPIO_Pin_14

void MyPlantGrowLamp_Init(void);
void MyPlantGrowLamp_SET(void);
void MyPlantGrowLamp_RESET(void);
void MyPlantGrowLamp_Cmd(BitAction BitVal);
void MyPlantGrowLamp_GetBitVal(void);

#endif
