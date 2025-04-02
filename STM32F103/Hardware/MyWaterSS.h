#ifndef __MYWATERSS_H
#define __MYWATERSS_H
//上：错点：在 C 和 C++ 里，标识符（像宏名、变量名、函数名等）仅能由字母（大写或小写）、
//	数字以及下划线 _ 构成，且不能以数字开头。
//	你代码中的 __MYHC-SR04_H 包含了 - 字符，这就违背了标识符的命名规则，所以编译器会给出警告。

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
