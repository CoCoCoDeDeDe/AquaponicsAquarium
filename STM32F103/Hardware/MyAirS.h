#ifndef __MYAIRS_H
#define __MYAIRS_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "MySerial.h"

#define PA4SET			GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

#define PA4RESET		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);

typedef enum {
	IDLE,//����״̬(δ��������)
	RSP_START,
	RESPONSE_RISING,
	RSP_END,
	BIT_END,
	END_END,
	FAILED//���մ���(��ʱ��У��ʧ��)
} MyAirS_States_ReaderSM;

extern uint8_t MyAirS_Flag_TIM3ARCounter_On;
extern uint32_t MyAirS_Count_TIM3AR;

extern uint8_t MyAirS_Flag_ReadCheckTimer_On ;

extern uint8_t MyAirS_Flag_WriterSM_On;
extern uint8_t MyAirS_State_WriterSM;
extern uint32_t MyAirS_Count_WriterSM;

extern uint8_t MyAirS_Count_ReadBit;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

extern uint8_t MyAirS_BitsArr[];//ÿ1��Ԫ�ض�ӦҪ��ȡ�����ݵ�ÿһ��bit
extern uint8_t MyAirS_DataArr[];//ÿ1��Ԫ�ض���ת�����һ���ֽ�,������У��λ
extern MyAirS_States_ReaderSM MyAirS_State_ReaderSM;//Ĭ��δ��ʼ����
extern uint32_t MyAirS_Count_ReadInterval;//�洢FALLING��RISING֮��TIM3CNT��������
extern uint32_t MyAirS_Count_ReadIntervalStart;//�洢��ʼ��FALLING��RISING֮��TIM3CNT��������ʱ��TIM3CNTֵ
extern uint8_t MyAirS_Flag_ReadSucced;
extern uint8_t MyAirS_Count_ReadBit;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

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
