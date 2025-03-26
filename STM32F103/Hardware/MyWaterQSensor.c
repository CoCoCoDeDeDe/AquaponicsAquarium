#include "MyWaterQSensor.h"

void MyWaterQSensor_Init(void) {
	
	//GPIO=====
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyWaterQSensor;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOMyWaterQSensor, &GPIO_InitStruct);
	
	//ADCRegularChannel=====
	ADC_RegularChannelConfig(ADCMyWaterQSensor, ADC_Channel_MyWaterQSensor, 1, ADC_SampleTime_55Cycles5);	
	
	//Serial_SendStringPacketV2(USART2, "MHA11A_Init_End\r\n");
}
