#include "MyWaterH.h"

void MyWaterH_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_Heater;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Heater, &GPIO_InitStruct);
	
	MyWaterH_SET();//默认关闭
}
void MyWaterH_SET(void) {
	GPIO_SetBits(GPIO_Heater, PIN_Heater);	//高电平
}
void MyWaterH_RESET(void) {
	GPIO_ResetBits(GPIO_Heater, PIN_Heater);
}
void MyWaterH_Cmd(BitAction BitVal) {
	GPIO_WriteBit(GPIO_Heater, PIN_Heater, BitVal);
}

void MyWaterH_GetBitVal(void) {
	GPIO_ReadOutputDataBit(GPIO_Heater, PIN_Heater);
}

uint8_t WaterHRS = 0;

void MyWaterH_SetRunStatus(uint8_t rs) {
	
	WaterHRS = rs;
	
	if(rs == 1) {
		
		GPIO_SetBits(GPIO_Heater, PIN_Heater);	//高电平开
		
	} else if (rs == 0) {
		
		GPIO_ResetBits(GPIO_Heater, PIN_Heater);
	} else {
		
		//ERROR
	}
}



