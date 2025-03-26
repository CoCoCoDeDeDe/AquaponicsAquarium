#ifndef __MYHC_SR04_H
#define __MYHC_SR04_H
//上：错点：在 C 和 C++ 里，标识符（像宏名、变量名、函数名等）仅能由字母（大写或小写）、
//	数字以及下划线 _ 构成，且不能以数字开头。
//	你代码中的 __MYHC-SR04_H 包含了 - 字符，这就违背了标识符的命名规则，所以编译器会给出警告。

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
