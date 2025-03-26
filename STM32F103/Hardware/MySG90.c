#include "MySG90.h"                  // Device header

static TIM_OCInitTypeDef TIM_OCInitStruct;

//下：SG90->Signal——PA9——OC2——TIM1
void MySG90_Init(uint16_t TIM_Pulse) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MySG90;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MySG90, &GPIO_InitStruct);
	
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//设置定时器空闲时该OC的高低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR，注意实际值=寄存器值+1
	TIM_OC2Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//Serial_SendStringPacketV2(USART2, "MySG90_Init_End\r\n");
}

void MySG90_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR
	TIM_OC2Init(TIM1, &TIM_OCInitStruct);
}
