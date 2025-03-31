#ifndef __MYSERIAL_H
#define __MYSERIAL_H

#include "stm32f10x.h"                  // Device header

#include <math.h>
#include <stdio.h>

#include "ESP8266.h"

#define USARTESP8266		USART3
#define USARTESP8266_IRQn	USART3_IRQn
#define GPIOESP8266			GPIOB
#define GPIO_Pin_ESP8266Tx	GPIO_Pin_10
#define GPIO_Pin_ESP8266Rx	GPIO_Pin_11

#define USARTPC				USART2
#define USARTPC_IRQn		USART2_IRQn
#define GPIOPC				GPIOA
#define GPIO_Pin_PCTx		GPIO_Pin_2
#define GPIO_Pin_PCRx		GPIO_Pin_3

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
void Serial_SendString(USART_TypeDef* USARTx, char *string);
void Serial_SendStringV2(USART_TypeDef* USARTx, char* str);
void Serial_SendInteger(USART_TypeDef* USARTx, int num);
#endif
