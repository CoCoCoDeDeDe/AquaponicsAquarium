#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header

#include <math.h>
#include <stdio.h>

#define USARTMySerialESP8266		USART3
#define USARTMySerialESP8266_IRQn	USART3_IRQn
#define GPIOMySerialESP8266			GPIOB
#define GPIO_Pin_MySerialESP8266Tx	GPIO_Pin_10
#define GPIO_Pin_MySerialESP8266Rx	GPIO_Pin_11

#define USARTMySerialPC				USART2
#define USARTMySerialPC_IRQn		USART2_IRQn
#define GPIOMySerialPC				GPIOA
#define GPIO_Pin_MySerialPCTx		GPIO_Pin_2
#define GPIO_Pin_MySerialPCRx		GPIO_Pin_3

extern USART_InitTypeDef USART_InitStruct;
extern uint8_t Serial_RxFlag[];	//错点：Serial.c中Serial_RxFlag[4]赋值后此处不能再次赋值。

extern char Serial_Rx3StringPacket[];
extern char Serial_Rx2StringPacket[];

extern uint8_t StateMachine_s3;	//错点：Serial.c中赋值后此处不能再次赋值。
extern uint8_t StateMachine_count3;	//错点：Serial.c中赋值后此处不能再次赋值。
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
