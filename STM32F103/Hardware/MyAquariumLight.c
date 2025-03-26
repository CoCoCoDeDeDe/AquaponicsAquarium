#include "MyAquariumLight.h"

static TIM_OCInitTypeDef TIM_OCInitStruct;

//�£�LED����PA8����OC1����TIM1
void MyAquariumLight_Init(uint16_t TIM_Pulse) {
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyAquariumLight;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyAquariumLight, &GPIO_InitStruct);
	
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//���ö�ʱ������ʱ��OC�ĸߵ͵�ƽ
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//���ö��ٴ�CNT����/�Լ���CNT��������CCR��ע��ʵ��ֵ=�Ĵ���ֵ+1
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
}

void MyAquariumLight_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//���ö��ٴ�CNT����/�Լ���CNT��������CCR
	TIM_OC2Init(TIM1, &TIM_OCInitStruct);
}




