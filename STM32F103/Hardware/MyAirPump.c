#include "MyAirPump.h"

void MyAirPump_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_APump;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_APump, &GPIO_InitStruct);
	
	MyAirPump_SET();//默认关闭
}
void MyAirPump_SET(void) {
	GPIO_SetBits(GPIO_APump, PIN_APump);	//高电平
}
void MyAirPump_RESET(void) {
	GPIO_ResetBits(GPIO_APump, PIN_APump);
}
void MyAirPump_Cmd(BitAction BitVal) {
	GPIO_WriteBit(GPIO_APump, PIN_APump, BitVal);
}

void MyAirPump_GetBitVal(void) {
	GPIO_ReadOutputDataBit(GPIO_APump, PIN_APump);
}
