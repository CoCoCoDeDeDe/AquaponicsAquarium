#ifndef __MYHC_SR04_H
#define __MYHC_SR04_H
//�ϣ���㣺�� C �� C++ ���ʶ��������������������������ȣ���������ĸ����д��Сд����
//	�����Լ��»��� _ ���ɣ��Ҳ��������ֿ�ͷ��
//	������е� __MYHC-SR04_H ������ - �ַ������Υ���˱�ʶ���������������Ա�������������档

#include "stm32f10x.h"                  // Device header

#include "Array.h"

#include "Delay.h"

#include "MyTIM.h"

#include "Serial.h"

extern EXTI_InitTypeDef EXTI_InitStruct;

extern uint8_t MyHCSR04_TrigCtrler_Ctrl;
extern uint8_t MyHCSR04_TrigSETer_Ctrl;

extern uint8_t MyHCSR04_EchoCtrlerSM_Ctrl;
extern uint32_t MyHCSR04_CountEchoSpan;

extern uint8_t MyHCSR04_EchoCtrlerSM_State;

extern uint16_t MyHCSR04_CountEchoSpanFiltered;

void MyHCSR04_Trig_Init(void);
void MyHCSR04_Echo_Init(void);
uint16_t MyHCSR04_ResultFilter(uint32_t t);
void MyHCSR04_TrigCtrlerSwitchOn(void);
void MyHCSR04_TrigCtrler(void);
void MyHCSR04_TrigSETer(void);
void MyHCSR04_TrigRESETer(void);
void MyHCSR04_SetEXITTrig(EXTITrigger_TypeDef mode);
void MyHCSR04_EchoCtrlerSM(void);
void MyHCSR04_CollectEchoSpan(void);
uint16_t MyHCSR04_GetResult_mm(void);

#endif
