#ifndef __MYWATERTS_H
#define __MYWATERTS_H

#include "stm32f10x.h"                  // Device header

#include "Array.h"

#include "MySerial.h"

#include "Delay.h"

#define GPIO_MyWaterTSDQ			GPIOA
#define GPIO_Pin_MyWaterTSDQ		GPIO_Pin_5

#define MyWaterTSDQ_RESET		GPIO_WriteBit(GPIO_MyWaterTSDQ, GPIO_Pin_MyWaterTSDQ, Bit_RESET)
#define MyWaterTSDQ_SET			GPIO_WriteBit(GPIO_MyWaterTSDQ, GPIO_Pin_MyWaterTSDQ, Bit_SET)
#define MyWaterTSDQ_Read		GPIO_ReadInputDataBit(GPIO_MyWaterTSDQ, GPIO_Pin_MyWaterTSDQ)

extern uint16_t MyWaterTS_ReadPacket_16Bit_Temp;

extern uint16_t MyWaterTS_Result_12Bit_H7Bit;//12Bit分辨率结果的整数部分

extern uint16_t MyWaterTS_Result_12Bit_L4Bit;//12Bit分辨率结果的小数部分

void MyWaterTS_Init(void);

void MyWaterTS_TaskSM_TurnOn(void);

void MyWaterTS_TaskSuccedCheckTimer(void);

void MyWaterTS_TaskSM(void);

#endif
