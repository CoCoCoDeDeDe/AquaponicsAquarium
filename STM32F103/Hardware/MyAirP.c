#include "MyAirP.h"

void MyAirP_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_APump;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_APump, &GPIO_InitStruct);
	
	MyAirP_SET();//默认关闭
}
void MyAirP_SET(void) {
	GPIO_SetBits(GPIO_APump, PIN_APump);	//高电平
}
void MyAirP_RESET(void) {
	GPIO_ResetBits(GPIO_APump, PIN_APump);
}
void MyAirP_Cmd(BitAction BitVal) {
	GPIO_WriteBit(GPIO_APump, PIN_APump, BitVal);
}

uint8_t MyAirP_GetBitVal(void) {
	return GPIO_ReadOutputDataBit(GPIO_APump, PIN_APump);
}

uint8_t AirPRS = 0;

void MyAirP_SetRunStatus(uint8_t rs) {
	AirPRS = rs;
	if(rs == 0) {
		GPIO_ResetBits(GPIO_APump, PIN_APump);//低电平关
	} else if (rs == 1) {
		GPIO_SetBits(GPIO_APump, PIN_APump);//高电平开
	} else {
		//ERROR
	}
}
