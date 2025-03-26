#include "MyPlantGrowLamp.h"

void MyPlantGrowLamp_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_PGLamp;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PGLamp, &GPIO_InitStruct);
	
	MyPlantGrowLamp_SET();//默认关闭
}
void MyPlantGrowLamp_SET(void) {
	GPIO_SetBits(GPIO_PGLamp, PIN_PGLamp);	//高电平
}
void MyPlantGrowLamp_RESET(void) {
	GPIO_ResetBits(GPIO_PGLamp, PIN_PGLamp);
}
void MyPlantGrowLamp_Cmd(BitAction BitVal) {
	GPIO_WriteBit(GPIO_PGLamp, PIN_PGLamp, BitVal);
}

void PlantGrowLamp_GetBitVal(void) {
	GPIO_ReadOutputDataBit(GPIO_PGLamp, PIN_PGLamp);
}

