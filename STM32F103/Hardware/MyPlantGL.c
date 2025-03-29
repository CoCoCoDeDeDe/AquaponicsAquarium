#include "MyPlantGL.h"

static TIM_OCInitTypeDef TIM_OCInitStruct;

void MyPlantGL_Init(uint16_t TIM_Pulse) {
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyPlantGL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyPlantGL, &GPIO_InitStruct);
	
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//设置定时器空闲时该OC的高低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR，注意实际值=寄存器值+1
	TIM_OC4Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
}

void MyPlantGL_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR
	TIM_OC4Init(TIM1, &TIM_OCInitStruct);
}

float PlantGLVR = 0;

void MyPlantGL_SetVoltageRatio(uint8_t r) {
	
	PlantGLVR = r;
	
	MyPlantGL_SetPulse(r*200);//20000/100=200
}


