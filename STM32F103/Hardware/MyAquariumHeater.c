#include "MyAquariumHeater.h"

void MyAquariumHeater_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_Heater;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Heater, &GPIO_InitStruct);
	
	MyAquariumHeater_SET();//默认关闭
}
void MyAquariumHeater_SET(void) {
	GPIO_SetBits(GPIO_Heater, PIN_Heater);	//高电平
}
void MyAquariumHeater_RESET(void) {
	GPIO_ResetBits(GPIO_Heater, PIN_Heater);
}
void MyAquariumHeater_Cmd(BitAction BitVal) {
	GPIO_WriteBit(GPIO_Heater, PIN_Heater, BitVal);
}

void MyAquariumHeater_GetBitVal(void) {
	GPIO_ReadOutputDataBit(GPIO_Heater, PIN_Heater);
}



