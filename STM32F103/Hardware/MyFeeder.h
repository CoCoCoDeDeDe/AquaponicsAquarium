#ifndef __MYDEEDER_H
#define __MYDEEDER_H

#include "stm32f10x.h"                  // Device header

#include "MySerial.h"

#include "Delay.h"

#define GPIO_MyFeeder			GPIOA
#define GPIO_Pin_MyFeeder		GPIO_Pin_8

void MyFeeder_Init(uint16_t TIM_Pulse);

void MyFeeder_SetPulse(uint16_t TIM_Pulse);

extern uint8_t FeederRS;

void MyFeeder_SetRunStatus(uint8_t rs);

extern uint16_t FeederAngle;

void MyFeeder_Triger(int8_t ft);

void MyFeeder_ServoTest(void);

#endif
