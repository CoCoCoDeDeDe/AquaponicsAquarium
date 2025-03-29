#ifndef __MYWATERSS_H
#define __MYWATERSS_H
//�ϣ���㣺�� C �� C++ ���ʶ��������������������������ȣ���������ĸ����д��Сд����
//	�����Լ��»��� _ ���ɣ��Ҳ��������ֿ�ͷ��
//	������е� __MYHC-SR04_H ������ - �ַ������Υ���˱�ʶ���������������Ա�������������档

#include "stm32f10x.h"                  // Device header

#include "Array.h"

#include "Delay.h"

#include "MyTIM.h"

#include "MySerial.h"

extern EXTI_InitTypeDef EXTI_InitStruct;

extern uint8_t MyWaterSS_TrigCtrler_Ctrl;
extern uint8_t MyWaterSS_TrigSETer_Ctrl;

extern uint8_t MyWaterSS_EchoCtrlerSM_Ctrl;
extern uint32_t MyWaterSS_CountEchoSpan;

extern uint8_t MyWaterSS_EchoCtrlerSM_State;

extern uint16_t MyWaterSS_CountEchoSpanFiltered;

extern uint16_t WaterSD;

void MyWaterSS_Trig_Init(void);
void MyWaterSS_Echo_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);
uint16_t MyWaterSS_ResultFilter(uint32_t t);
void MyWaterSS_TrigCtrlerSwitchOn(void);
void MyWaterSS_TrigCtrler(void);
void MyWaterSS_TrigSETer(void);
void MyWaterSS_TrigRESETer(void);
void MyWaterSS_SetEXITTrig(EXTITrigger_TypeDef mode);
void MyWaterSS_EchoCtrlerSM(void);
void MyWaterSS_CollectEchoSpan(void);
uint16_t MyWaterSS_GetResult_mm(void);


#endif
