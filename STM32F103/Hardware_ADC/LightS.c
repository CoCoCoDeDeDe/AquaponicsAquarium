#include "LightS.h"                  // Device header
#include "stm32f10x.h"                  // Device header

#define GPIOLightS									GPIOB
#define GPIO_Pin_LightS							GPIO_Pin_1
#define ADC_Channel_LightS					ADC_Channel_9
#define ADC_RegularRank_LightS			4

void LightS_Init(void) {
	
	//GPIO=====
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_LightS;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//ADCRegularChannel=====
	ADC_RegularChannelConfig(ADC1, ADC_Channel_LightS, ADC_RegularRank_LightS, ADC_SampleTime_55Cycles5);
	//MyADCAndDMA_Result[3]
	
	//Serial_SendStringV2(USART2, "LightS_Init_End\r\n");
}
