#ifndef __MYWATERQSENDOR_H
#define __MYWATERQSENDOR_H

#include "stm32f10x.h"                  // Device header

#include "Serial.h"

#define GPIOMyWaterQSensor			GPIOA
#define GPIO_Pin_MyWaterQSensor		GPIO_Pin_1
#define ADCMyWaterQSensor			ADC1
#define ADC_Channel_MyWaterQSensor	ADC_Channel_1

void MyWaterQSensor_Init(void);

#endif
