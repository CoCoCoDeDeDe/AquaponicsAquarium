//=====������=====
//3V3����BT6612AIN1
//GND����BT6612AIN2
//TIM1����OC3PWM����PA10����BT6612PWMA
//================
//��㣺Ӳ����TB6612���ǽ�STBY��3.3V


#include "stm32f10x.h"                  // Device header

void MyTB6612_Init(void) {
	

}



//void MyTB6612_Init(void) {
//	//RCC===
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	
//	//�£���㣺û�н�PA15�Ĺ�����ӳ��ΪGPIO����+��SWJ_Disable�����޷���STLINK��¼
//	//Remap====
//	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
//	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
//	
//	//Init===
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOA, &GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOA, &GPIO_InitStruct);
//	
//	//PA12����AIN2
//	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
//	//PA15����AIN1
//	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);
//}



