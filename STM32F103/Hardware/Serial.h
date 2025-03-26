#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header

#include <math.h>
#include <stdio.h>

extern USART_InitTypeDef USART_InitStruct;
extern uint8_t Serial_RxFlag[];	//错点：Serial.c中Serial_RxFlag[4]赋值后此处不能再次赋值。

extern char Serial_Rx3StringPacket[];
extern char Serial_Rx2StringPacket[];

extern uint8_t StateMachine_s1;	//错点：Serial.c中赋值后此处不能再次赋值。
extern uint8_t StateMachine_count1;	//错点：Serial.c中赋值后此处不能再次赋值。
extern uint8_t StateMachine_s2;	//错点：Serial.c中赋值后此处不能再次赋值。
extern uint8_t StateMachine_count2;	//错点：Serial.c中赋值后此处不能再次赋值。

void Serial_Init(
	USART_TypeDef* USARTx, uint32_t USART_BaudRate, 
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);
void Serial_BaudRateConfig(USART_TypeDef* USARTx, uint32_t USART_BaudRate);
void Serial_Auto_StateMachine(
	USART_TypeDef *USARTx, 
	uint8_t ser, 
	uint8_t *s, 
	uint8_t *count, 
	char Serial_RxStringPacket[], 
	uint8_t size);
void Serial_SendByte(USART_TypeDef* USARTx, uint8_t Byte);
void Serial_SendStringPacket(USART_TypeDef* USARTx, char *string);
void Serial_SendStringPacketV2(USART_TypeDef* USARTx, char* str);
void Serial_SendInteger(USART_TypeDef* USARTx, int num);
#endif
