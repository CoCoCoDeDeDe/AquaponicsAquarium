#include "MyTest.h"
#include "math.h"

void MyTest_PB5_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	MyTest_PB5_WriteSET();
}

void MyTest_PB5_WriteSET(void) {
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

void MyTest_PB5_WriteRESET(void) {
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
}


