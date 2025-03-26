#ifndef __MYDHT11_H
#define __MYDHT11_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "Serial.h"

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
} MyDHT11_States_ReaderSM;

extern uint8_t MyDHT11_Flag_TIM3ARCounter_On;
extern uint32_t MyDHT11_Count_TIM3AR;

extern uint8_t MyDHT11_Flag_ReadCheckTimer_On ;

extern uint8_t MyDHT11_Flag_WriterSM_On;
extern uint8_t MyDHT11_State_WriterSM;
extern uint32_t MyDHT11_Count_WriterSM;

extern uint8_t MyDHT11_Count_ReadBit;//在成功接收1Bit时++,在接收完40BIT或者开始接收时=0

extern uint8_t MyDHT11_BitsArr[];//每1个元素对应要读取的数据的每一个bit
extern uint8_t MyDHT11_DataArr[];//每1个元素对于转换完的一个字节,不包括校验位
extern MyDHT11_States_ReaderSM MyDHT11_State_ReaderSM;//默认未开始接收
extern uint32_t MyDHT11_Count_ReadInterval;//存储FALLING和RISING之间TIM3CNT计数次数
extern uint32_t MyDHT11_Count_ReadIntervalStart;//存储开始记FALLING和RISING之间TIM3CNT计数次数时的TIM3CNT值
extern uint8_t MyDHT11_Flag_ReadSucced;
extern uint8_t MyDHT11_Count_ReadBit;//在成功接收1Bit时++,在接收完40BIT或者开始接收时=0

void MyDHT11_Init(void);
void MyDHT11_EXTICmd(FunctionalState EXTI_LineCmd);
void MyDHT11_PA4GPIOModeConfig(GPIOMode_TypeDef GPIO_Mode);
void MyDHT11_SetPA4GPIOMode_Out_PP(void);
void MyDHT11_SetPA4GPIOMode_IPU(void);
void MyDHT11_SetMode_Write(void);
void MyDHT11_SetMode_Read(void);
void MyDHT11_SetEXTITrigRising(void);
void MyDHT11_SetEXTITrigFalling(void);
void MyDHT11_Count_TIM3ARer(void);
void MyDHT11_ReadCheckTimer(void);
void MyDHT11_SwitchOn(void);
void MyDHT11_ReaderSM_PrePrepare(void);
void MyDHT11ReadCheck(void);
void MyDHT11_ReadSucced(void);
void MyDHT11_ReadFailed(void);
void MyDHT11_Reset(void);
void MyDHT11_WriterSM(void);
void MyDHT11_ReaderSM(void);
void MyDHT11_Count_ReadInterval_Start(void);
void MyDHT11_Count_ReadInterval_Save(void);
void MyDHT11_DataTranslate(void);
#endif
