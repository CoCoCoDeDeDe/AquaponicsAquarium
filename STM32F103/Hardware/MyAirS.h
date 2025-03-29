#ifndef __MYAIRS_H
#define __MYAIRS_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "MySerial.h"

#define PA4SET			GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

#define PA4RESET		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);

typedef enum {
	IDLE,//空闲状态(未启动接收)
	RSP_START,
	RESPONSE_RISING,
	RSP_END,
	BIT_END,
	END_END,
	FAILED//接收错误(超时或校验失败)
} MyAirS_States_ReaderSM;

extern uint8_t MyAirS_Flag_TIM3ARCounter_On;
extern uint32_t MyAirS_Count_TIM3AR;

extern uint8_t MyAirS_Flag_ReadCheckTimer_On ;

extern uint8_t MyAirS_Flag_WriterSM_On;
extern uint8_t MyAirS_State_WriterSM;
extern uint32_t MyAirS_Count_WriterSM;

extern uint8_t MyAirS_Count_ReadBit;//在成功接收1Bit时++,在接收完40BIT或者开始接收时=0

extern uint8_t MyAirS_BitsArr[];//每1个元素对应要读取的数据的每一个bit
extern uint8_t MyAirS_DataArr[];//每1个元素对于转换完的一个字节,不包括校验位
extern MyAirS_States_ReaderSM MyAirS_State_ReaderSM;//默认未开始接收
extern uint32_t MyAirS_Count_ReadInterval;//存储FALLING和RISING之间TIM3CNT计数次数
extern uint32_t MyAirS_Count_ReadIntervalStart;//存储开始记FALLING和RISING之间TIM3CNT计数次数时的TIM3CNT值
extern uint8_t MyAirS_Flag_ReadSucced;
extern uint8_t MyAirS_Count_ReadBit;//在成功接收1Bit时++,在接收完40BIT或者开始接收时=0

void MyAirS_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority);
void MyAirS_EXTICmd(FunctionalState EXTI_LineCmd);
void MyAirS_PA4GPIOModeConfig(GPIOMode_TypeDef GPIO_Mode);
void MyAirS_SetPA4GPIOMode_Out_PP(void);
void MyAirS_SetPA4GPIOMode_IPU(void);
void MyAirS_SetMode_Write(void);
void MyAirS_SetMode_Read(void);
void MyAirS_SetEXTITrigRising(void);
void MyAirS_SetEXTITrigFalling(void);
void MyAirS_Count_TIM3ARer(void);
void MyAirS_ReadCheckTimer(void);
void MyAirS_SwitchOn(void);
void MyAirS_ReaderSM_PrePrepare(void);
void MyAirSReadCheck(void);
void MyAirS_ReadSucced(void);
void MyAirS_ReadFailed(void);
void MyAirS_Reset(void);
void MyAirS_WriterSM(void);
void MyAirS_ReaderSM(void);
void MyAirS_Count_ReadInterval_Start(void);
void MyAirS_Count_ReadInterval_Save(void);
void MyAirS_DataTranslate(void);

extern uint8_t AirT;

extern uint8_t AirH;

#endif
