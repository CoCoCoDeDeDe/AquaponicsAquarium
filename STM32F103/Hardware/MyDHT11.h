#ifndef __MYDHT11_H
#define __MYDHT11_H

#include "stm32f10x.h"                  // Device header

#include "MyTest.h"
#include "Serial.h"

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
} MyDHT11_States_ReaderSM;

extern uint8_t MyDHT11_Flag_TIM3ARCounter_On;
extern uint32_t MyDHT11_Count_TIM3AR;

extern uint8_t MyDHT11_Flag_ReadCheckTimer_On ;

extern uint8_t MyDHT11_Flag_WriterSM_On;
extern uint8_t MyDHT11_State_WriterSM;
extern uint32_t MyDHT11_Count_WriterSM;

extern uint8_t MyDHT11_Count_ReadBit;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

extern uint8_t MyDHT11_BitsArr[];//ÿ1��Ԫ�ض�ӦҪ��ȡ�����ݵ�ÿһ��bit
extern uint8_t MyDHT11_DataArr[];//ÿ1��Ԫ�ض���ת�����һ���ֽ�,������У��λ
extern MyDHT11_States_ReaderSM MyDHT11_State_ReaderSM;//Ĭ��δ��ʼ����
extern uint32_t MyDHT11_Count_ReadInterval;//�洢FALLING��RISING֮��TIM3CNT��������
extern uint32_t MyDHT11_Count_ReadIntervalStart;//�洢��ʼ��FALLING��RISING֮��TIM3CNT��������ʱ��TIM3CNTֵ
extern uint8_t MyDHT11_Flag_ReadSucced;
extern uint8_t MyDHT11_Count_ReadBit;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

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
