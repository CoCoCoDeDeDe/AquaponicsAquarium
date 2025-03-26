#include "MySoilMoistureSensor.h"

void MySoilMoistureSensor_Init(void) {
	
	//GPIO=====
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//ADCRegularChannel=====
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_55Cycles5);

	//Serial_SendStringPacketV2(USART2, "MySoilMoistureSensor_Init_End\r\n");
}
