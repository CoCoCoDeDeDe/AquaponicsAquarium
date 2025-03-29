#include "MyFeeder.h"                  // Device header

static TIM_OCInitTypeDef TIM_OCInitStruct;

//下：SG90->Signal——PA9——OC1——TIM1
void MyFeeder_Init(uint16_t TIM_Pulse) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyFeeder;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyFeeder, &GPIO_InitStruct);
	
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;	//设置定时器空闲时该OC的高低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;	//Cmd
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//Cmd
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR，注意实际值=寄存器值+1
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//Serial_SendStringPacketV2(USART2, "MyFeeder_Init_End\r\n");
}

void MyFeeder_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//设置多少次CNT自增/自减后将CNT次数存入CCR
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
}

uint8_t FeederRS = 0;//0：待复位，无储粮；1：还可喂食1次；2：还可喂食2次...10：还可喂食10次。

void MyFeeder_SetRunStatus(uint8_t rs) {
	
	FeederRS = rs;
	
	//【TODO】完善喂食控制???
}

uint16_t FeederAngle = 0;//【TODO】改进：上传电机角度或喂食次数信息或该FRS为喂食次数，
	//应用端在最后一次喂食完后提醒用户没有粮食了，
	//此时不复位，用户确认已经补充粮仓后并触发一次才复位

void MyFeeder_Triger(void) {	//【TODO】喂食状态机，喂食1次、2次...复位。

	if(FeederRS == 1) {
		//RUNNING
	} else if (FeederRS == 0) {
		
		MyFeeder_SetPulse(FeederAngle);
	}
}

void MyFeeder_ServoTest(void) {//放入循环
	
	static uint8_t j = 0;
		
	for(uint8_t i = 0; i < 8; i++) {
		MyFeeder_SetPulse(500 + j*500);
		if(i < 4) {
			j++;
		} else {
			j--;
		}
			Delay_ms(1000);
		}
}
//	0	1	2	3	4	5	6	7	
//	500	1000150020002500200015001000	
//	++	++	++	++	--	--	--	--
//	+0*	+1*	+2*	+3*	+4*	+3*	+2*	+1*


