#ifndef __MYADCANDDMA_H
#define __MYADCANDDMA_H

#include "stm32f10x.h"                  // Device header

#include "MySerial.h"

extern uint16_t MyADCAndDMA_Result[];

void MyADCAndDMA_Init(uint8_t ADC_Ch_Num);

float adc_to_voltage(uint16_t adc_value);

#endif
