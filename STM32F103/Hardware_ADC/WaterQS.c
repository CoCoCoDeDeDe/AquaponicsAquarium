#include "WaterQS.h"
#include "stm32f10x.h"                  // Device header

#define GPIOWaterQS								GPIOB
#define GPIO_Pin_WaterQS						GPIO_Pin_0
#define ADC_Channel_WaterQS				ADC_Channel_8
#define ADC_RegularRank_WaterQS		3

static GPIO_InitTypeDef GPIO_InitStruct;

void WaterQS_Init(void) {
	
	//GPIO=====
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_WaterQS;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOWaterQS, &GPIO_InitStruct);
	
	//ADCRegularChannel=====
	ADC_RegularChannelConfig(ADC1, ADC_Channel_WaterQS, ADC_RegularRank_WaterQS, ADC_SampleTime_55Cycles5);
	//MyADCAndDMA_Result[1]
}
