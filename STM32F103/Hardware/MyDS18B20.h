#ifndef __MYDS18B20_H
#define __MYDS18B20_H

#include "stm32f10x.h"                  // Device header

#include "Array.h"

#include "Serial.h"

#include "Delay.h"

#define GPIO_MyDS18B20DQ			GPIOA
#define GPIO_Pin_MyDS18B20DQ		GPIO_Pin_5

#define MyDS18B20DQ_RESET		GPIO_WriteBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ, Bit_RESET)
#define MyDS18B20DQ_SET			GPIO_WriteBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ, Bit_SET)
#define MyDS18B20DQ_Read		GPIO_ReadInputDataBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ)

extern uint16_t MyDS18B20_ReadPacket_16Bit_Temp;

extern uint16_t MyDS18B20_Result_12Bit_H7Bit;//12Bit分辨率结果的整数部分

extern uint16_t MyDS18B20_Result_12Bit_L4Bit;//12Bit分辨率结果的小数部分

void MyDS18B20_Init(void);

void MyDS18B20_TaskSM_TurnOn(void);

void MyDS18B20_TaskSuccedCheckTimer(void);

void MyDS18B20_TaskSM(void);

#endif
