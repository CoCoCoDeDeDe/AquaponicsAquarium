#include "MyTIM.h"                  // Device header

uint32_t MyTIM_1UpCount = 0;
uint32_t MyTIM_2Count = 0;
uint32_t MyTIM_3Count = 0;
uint32_t MyTIM_4Count = 0;

void MyTIM1_Init(void) {	//fCNT=50Hz,TCNT=0.02s=20ms=20,000us
	
	//Triger==========
	TIM_InternalClockConfig(TIM1);
	
	//TimeBase==========
	TIM_TimeBaseInitTypeDef TIME_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIME_TimeBaseInitStruct);
	TIME_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIME_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//CNT每个周期内从0到TIM_Period递增
	TIME_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;
	TIME_TimeBaseInitStruct.TIM_Period = 20000 - 1;	//fCNT = 50Hz，TCNT = 20ms
	TIME_TimeBaseInitStruct.TIM_RepetitionCounter = (uint8_t)(1 - 1);
	
	TIM_TimeBaseInit(TIM1, &TIME_TimeBaseInitStruct);
	
	//ITConfig==========
	TIM_ClearFlag(TIM1, TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	//NVIC==========
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;	//NVIC指向的中断请求配置变量存储在stm32f10x.h文件中
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStruct);
	
	//Cmd==========
	TIM_Cmd(TIM1, ENABLE);//【Cmd】
}


void MyTIM2_Init(void) {	//fCNT=200,000Hz,TCNT=0.000005s=20ms=5us
	
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2, ENABLE);//【Cmd】
}


void MyTIM3_Init(void) {	//fCNT=100Hz,TCNT=0.01s=10ms=10,000us
	
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, ENABLE);//【Cmd】
}

void MyTIM4_Init(void) {	//fCNT=【】Hz,TCNT=【】s=【】us
	
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;	//0~65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 1-1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	
	TIM_ClearFlag(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM4, DISABLE);//【Cmd】
}

void MyTIM3_DIVx(uint16_t x) {//TTIM3IT = 0.01s = 10,000us
	//x = 100
	static uint16_t DIVxCount;
	DIVxCount ++;
	if(DIVxCount >= x) {//每ITx次(1s)执行该if内1次, TIT=TCKCNT/x
		DIVxCount=0;//使得再经历100个MyTIM3IT才再发送Trig信号
		
		MyHCSR04_TrigCtrlerSwitchOn();//批准发送Trig信号，在下一个TIM2中断开始
		
		MyDHT11_SwitchOn();
		
		//Serial_SendByte(USART2, 'U');
	}
}

void MyTIM3_DIVy(uint16_t y) {//TTIM3IT = 0.01s = 10,000us
	//y = 200
	static uint16_t DIVyCount;
	DIVyCount ++;
	if(DIVyCount >= y) {
		DIVyCount=0;
		
		//MyDS18B20_VariableInit();
		//MyDS18B20_Flag_TaskList1_RunStatus = 1;
	}
}

void MyTIM3_DIVz(uint16_t z) {
	//z = 500
	static uint16_t DIVzCount;//【错点】用uint8_t计数溢出导致无法满足>=256
	DIVzCount ++;
	if(DIVzCount >= z) {//fRUN=1/5Hz, TRUN=5s
		DIVzCount  = 0;
		
		MyDS18B20_TaskSM_TurnOn();
		
		
		//Serial_SendByte(USART2, 'U');
	}
}
