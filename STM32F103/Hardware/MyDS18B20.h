#ifndef __MYDS18B20_H
#define __MYDS18B20_H

#include "stm32f10x.h"                  // Device header

#include "Array.h"

#include "Serial.h"

#define GPIO_MyDS18B20DQ			GPIOA
#define GPIO_Pin_MyDS18B20DQ		GPIO_Pin_5

#define MyDS18B20DQ_RESET		GPIO_WriteBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ, Bit_RESET)
#define MyDS18B20DQ_SET			GPIO_WriteBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ, Bit_SET)
#define MyDS18B20DQ_Read		GPIO_ReadInputDataBit(GPIO_MyDS18B20DQ, GPIO_Pin_MyDS18B20DQ)

extern const uint8_t MyDS18B20_TaskStateOrder[];
extern uint8_t MyDS18B20_TaskStateOrder_Index;
extern uint32_t MyDS18B20_Count_TaskSM_RunTimes;
extern uint8_t MyDS18B20_Flag_Task_Succed;

extern const uint8_t MyDS18B20_InitStateOrder[];
extern uint8_t MyDS18B20_InitStateOrder_Index;
extern uint8_t MyDS18B20_Target_Init_CkeckDelay;

extern const uint8_t MyDS18B20_Write0StateOrder[];
extern uint8_t MyDS18B20_Write0StateOrder_Index;

extern const uint8_t MyDS18B20_Write1StateOrder[];
extern uint8_t MyDS18B20_Write1StateOrder_Index;

extern const uint8_t MyDS18B20_ReadBitStateOrder[];
extern uint8_t MyDS18B20_ReadBitStateOrder_Index;

extern uint8_t MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus;

extern uint8_t MyDS18B20_Count_BitReaded;

extern uint8_t MyDS18B20_ReadPacket[];//8ByteSCRATCHPAD

extern uint16_t MyDS18B20_Result_7BitUint_Temp;

extern uint16_t MyDS18B20_Result_7BitUint;

extern uint16_t MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes;

void MyDS18B20_Init(void);

void MyDS18B20_TaskSM_TurnOn(void);

void MyDS18B20_TaskSuccedCheckTimer(void);

void MyDS18B20_TaskSuccedCkeck(void);

void MyDS18B20_Task_Reset(void);

void MyDS18B20_TaskSM(void);

void MyDS18B20_ReadPacketConvert(void);

#endif
