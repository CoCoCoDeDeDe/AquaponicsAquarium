#ifndef __MYADCANDDMA_H
#define __MYADCANDDMA_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"

extern uint16_t MyADCAndDMA_Result[];

void MyADCAndDMA_Init(uint8_t ADC_Ch_Num);

#endif
