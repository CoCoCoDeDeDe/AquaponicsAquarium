#ifndef __MYWATERQS_H
#define __MYWATERQS_H

#include "stm32f10x.h"                  // Device header

#include "MySerial.h"

#define GPIOMyWaterQS			GPIOA
#define GPIO_Pin_MyWaterQS		GPIO_Pin_1
#define ADCMyWaterQS			ADC1
#define ADC_Channel_MyWaterQS	ADC_Channel_1

void MyWaterQS_Init(void);

#endif
