#include "MyWaterQS.h"

void MyWaterQS_Init(void) {
	
	//GPIO=====
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyWaterQS;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOMyWaterQS, &GPIO_InitStruct);
	
	//ADCRegularChannel=====
	ADC_RegularChannelConfig(ADCMyWaterQS, ADC_Channel_MyWaterQS, 2, ADC_SampleTime_55Cycles5);	
	//MyADCAndDMA_Result[1]
	
	//Serial_SendStringV2(USART2, "MHA11A_Init_End\r\n");
}
