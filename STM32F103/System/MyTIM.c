#include "MyTIM.h"                  // Device header

uint32_t MyTIM_1UpCount = 0;
uint32_t MyTIM_2Count = 0;
uint32_t MyTIM_3Count = 0;
uint32_t MyTIM_4Count = 0;

void MyTIM1_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {	//fCNT=50Hz,TCNT=0.02s=20ms=20,000us
	
	//Triger==========
	TIM_InternalClockConfig(TIM1);
	
	//TimeBase==========
	TIM_TimeBaseInitTypeDef TIME_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIME_TimeBaseInitStruct);
	TIME_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIME_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//CNTÿ�������ڴ�0��TIM_Period����
	TIME_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;
	TIME_TimeBaseInitStruct.TIM_Period = 20000 - 1;	//fCNT = 50Hz��TCNT = 20ms
	TIME_TimeBaseInitStruct.TIM_RepetitionCounter = (uint8_t)(1 - 1);
	
	TIM_TimeBaseInit(TIM1, &TIME_TimeBaseInitStruct);
	
	//ITConfig==========
	TIM_ClearFlag(TIM1, TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	//NVIC==========
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;	//NVICָ����ж��������ñ����洢��stm32f10x.h�ļ���
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	
	NVIC_Init(&NVIC_InitStruct);
	
	//Cmd==========
	TIM_Cmd(TIM1, DISABLE);//��Cmd��
}


void MyTIM2_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {	//fCNT=200,000Hz,TCNT=0.000005s=20ms=5us
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStruct.TIM_Period = 5 - 1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1 - 1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_ClearFlag(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;//0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2, DISABLE);//��Cmd��
}


void MyTIM3_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {	//fCNT=100Hz,TCNT=0.01s=10ms=10,000us
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	
	TIM_ClearFlag(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, DISABLE);//��Cmd��
}

void MyTIM4_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {	//fCNT=��1000��Hz,TCNT=��0.001��s=��1000��us
	
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_Period = 10 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1-1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	
	TIM_ClearFlag(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM4, DISABLE);//��Cmd��
}

void MyTIMx_ENABLECmd(TIM_TypeDef* TIMx) {
	TIM_Cmd(TIMx, ENABLE);
}

void MyTIMx_DISABLECmd(TIM_TypeDef* TIMx) {
	TIM_Cmd(TIMx, DISABLE);
}

//����Ϊ�жϺ���=============================================================

void TIM1_UP_IRQHandler(void) {		//IT per 20ms=	0.02s
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		//MyTIM_1UpCount ++;
		
	}
}

void TIM2_IRQHandler(void) {//TCKCNT=1us,TCNT=5us
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		//MyTIM_2Count++;
		
		MyWaterSS_TrigCtrler();
		
		MyAirS_WriterSM();
		
		MyWaterTS_TaskSM();
	}
}


void TIM3_IRQHandler(void) {//TCKCNT=1us,TCNT=0.01s
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		//MyTIM_3Count++;
		
		MyTIM3_DIVx(1000000/10000);
		
		MyAirS_Count_TIM3ARer();
		
		MyAirS_ReadCheckTimer();
		
		MyTIM3_DIVy(3000000/10000);
		
		MyWaterTS_TaskSuccedCheckTimer();
		
	}
}

void MyTIM3_DIVx(uint16_t x) {//TTIM3IT = 0.01s = 10,000us
	//x = 100
	static uint16_t DIVxCount;
	DIVxCount ++;
	if(DIVxCount >= x) {//ÿITx��(1s)ִ�и�if��1��, TIT=TCKCNT/x
		DIVxCount=0;//ʹ���پ���100��MyTIM3IT���ٷ���Trig�ź�
		
		MyWaterSS_TrigCtrlerSwitchOn();//��׼����Trig�źţ�����һ��TIM2�жϿ�ʼ
		
		MyAirS_SwitchOn();
	}
}

void MyTIM3_DIVy(uint16_t y) {
	//z = 300
	static uint16_t DIVyCount;//����㡿��uint8_t������������޷�����>=256
	DIVyCount ++;
	if(DIVyCount >= y) {//fRUN=1/5Hz, TRUN=5s
		DIVyCount  = 0;
		
		MyWaterTS_TaskSM_TurnOn();
	}
}

void TIM4_IRQHandler(void) {//TCKCNT=����us,TCNT=����s
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		//MyTIM_4Count++;
		
		//EPS8266_SM();//RunTime_Period = 100us
	}
}
