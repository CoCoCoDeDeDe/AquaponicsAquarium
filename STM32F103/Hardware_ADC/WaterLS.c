#include "WaterLS.h"
#include "stm32f10x.h"                  // Device header

#define GPIOWaterLS_1								GPIOA
#define GPIOWaterLS_2								GPIOA
#define GPIO_Pin_WaterLS_1						GPIO_Pin_0
#define GPIO_Pin_WaterLS_2						GPIO_Pin_1
#define ADC_Channel_WaterLS_1				ADC_Channel_0
#define ADC_Channel_WaterLS_2				ADC_Channel_1
#define ADC_RegularRank_WaterLS_1		1
#define ADC_RegularRank_WaterLS_2		2

static GPIO_InitTypeDef GPIO_IS;

void WaterLS_Init(void)
{
	GPIO_StructInit(&GPIO_IS);
	GPIO_IS.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_IS.GPIO_Pin  = GPIO_Pin_WaterLS_1;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOWaterLS_1, &GPIO_IS);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_WaterLS_1, ADC_RegularRank_WaterLS_1, ADC_SampleTime_55Cycles5);
	
	GPIO_StructInit(&GPIO_IS);
	GPIO_IS.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_IS.GPIO_Pin  = GPIO_Pin_WaterLS_2;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOWaterLS_2, &GPIO_IS);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_WaterLS_2, ADC_RegularRank_WaterLS_2, ADC_SampleTime_55Cycles5);
}


