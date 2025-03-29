#include "MyFeeder.h"                  // Device header

static TIM_OCInitTypeDef TIM_OCInitStruct;

//�£�SG90->Signal����PA9����OC1����TIM1
void MyFeeder_Init(uint16_t TIM_Pulse) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyFeeder;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyFeeder, &GPIO_InitStruct);
	
	
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
	
	//Serial_SendStringPacketV2(USART2, "MyFeeder_Init_End\r\n");
}

void MyFeeder_SetPulse(uint16_t TIM_Pulse) {
	TIM_OCInitStruct.TIM_Pulse = TIM_Pulse - 1;	//���ö��ٴ�CNT����/�Լ���CNT��������CCR
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
}

uint8_t FeederRS = 0;//0������λ���޴�����1������ιʳ1�Σ�2������ιʳ2��...10������ιʳ10�Ρ�

void MyFeeder_SetRunStatus(uint8_t rs) {
	
	FeederRS = rs;
	
	//��TODO������ιʳ����???
}

uint16_t FeederAngle = 0;//��TODO���Ľ����ϴ�����ǶȻ�ιʳ������Ϣ���FRSΪιʳ������
	//Ӧ�ö������һ��ιʳ��������û�û����ʳ�ˣ�
	//��ʱ����λ���û�ȷ���Ѿ��������ֺ󲢴���һ�βŸ�λ

void MyFeeder_Triger(void) {	//��TODO��ιʳ״̬����ιʳ1�Ρ�2��...��λ��

	if(FeederRS == 1) {
		//RUNNING
	} else if (FeederRS == 0) {
		
		MyFeeder_SetPulse(FeederAngle);
	}
}

void MyFeeder_ServoTest(void) {//����ѭ��
	
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


