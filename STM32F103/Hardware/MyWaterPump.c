#include "MyWaterPump.h"

static TIM_OCInitTypeDef TIM_OCInitStruct;
	
//下：错点：把PA11搞成OC3。答案：PA11——OC4, PA10——OC3
//下：WaterPump->TB6612->PWMA——PA10——OC3——TIM1
void MyWaterPump_Init(uint16_t TIM_Pulse) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyWaterPump;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyWaterPump, &GPIO_InitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//设置定时器空闲时该OC的高低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR，注意实际值=寄存器值
	TIM_OC3Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//Serial_SendStringPacketV2(USART2, "MyWaterPump_Init_End\r\n");
}

void MyWaterPump_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR，注意实际值=寄存器值
	TIM_OC3Init(TIM1, &TIM_OCInitStruct);
}
	

