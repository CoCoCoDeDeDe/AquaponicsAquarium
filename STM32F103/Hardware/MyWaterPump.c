#include "MyWaterPump.h"

static TIM_OCInitTypeDef TIM_OCInitStruct;
	
//�£���㣺��PA11���OC3���𰸣�PA11����OC4, PA10����OC3
//�£�WaterPump->TB6612->PWMA����PA10����OC3����TIM1
void MyWaterPump_Init(uint16_t TIM_Pulse) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyWaterPump;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyWaterPump, &GPIO_InitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//���ö�ʱ������ʱ��OC�ĸߵ͵�ƽ
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//���ö��ٴ�CNT����/�Լ���CNT��������CCR��ע��ʵ��ֵ=�Ĵ���ֵ
	TIM_OC3Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//Serial_SendStringPacketV2(USART2, "MyWaterPump_Init_End\r\n");
}

void MyWaterPump_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//���ö��ٴ�CNT����/�Լ���CNT��������CCR��ע��ʵ��ֵ=�Ĵ���ֵ
	TIM_OC3Init(TIM1, &TIM_OCInitStruct);
}
	

