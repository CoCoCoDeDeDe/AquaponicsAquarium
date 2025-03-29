#include "MyWaterTS.h"

typedef enum {
	T_Write0 = 0,
	T_Write1 = 1,
	T_ReadBit = 2,
	T_Init = 3,
	T_Wait750ms = 4,
	T_Convert = 5,
	T_IDLE = 6
} MyWaterTS_TaskState_TypeDef;

typedef enum {
	I_Down = 0,
	I_Rise = 1,
	I_Check = 2,
	I_Wait = 3
} MyWaterTS_InitState_TypeDef;

typedef enum {
	W0_Down = 0,
	W0_Rise = 1
} MyWaterTS_Write0State_TypeDef;

typedef enum {
	W1_Down = 0,
	W1_Rise = 1,
	W1_Wait = 2
} MyWaterTS_Write1State_TypeDef;

typedef enum {
	R_Down = 0,
	R_Rise = 1,
	R_Check = 2,
	R_Wait = 3
} MyWaterTS_ReadBitState_TypeDef;

static GPIO_InitTypeDef GPIO_InitStruct;

static const MyWaterTS_TaskState_TypeDef MyWaterTS_TaskStateOrder[125] = {//����㡿��������ʱ��һ���ֽ��ڵı���û�е�λ����
	T_IDLE,
	T_Init, 
	T_Write0, T_Write0, T_Write1, T_Write1, T_Write0, T_Write0, T_Write1, T_Write1, 
	T_Write0, T_Write0, T_Write1, T_Write0, T_Write0, T_Write0, T_Write1, T_Write0, 
	T_Wait750ms, 
	T_Init, 
	T_Write0, T_Write0, T_Write1, T_Write1, T_Write0, T_Write0, T_Write1, T_Write1, 
	T_Write0, T_Write1, T_Write1, T_Write1, T_Write1, T_Write1, T_Write0, T_Write1, 
	T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, 
	T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, 
	T_Convert
};//TaskSM��״̬ת��˳��
static uint8_t MyWaterTS_TaskStateOrder_Index = 0;//TaskSM��״̬ת��ָ��
static uint32_t MyWaterTS_Count_TaskSM_RunTimes = 0;//��������״̬������״̬������
static uint8_t MyWaterTS_Flag_Task_Succed = 0;//TaskSM�ĳɹ�״̬��־λ

static const MyWaterTS_InitState_TypeDef MyWaterTS_InitStateOrder[4] = {
	I_Down, I_Rise, I_Check, I_Wait
};//Init��״̬ת��˳��
static uint8_t MyWaterTS_InitStateOrder_Index = 0;//Init��״̬ת��ָ��
static uint8_t MyWaterTS_Target_Init_CkeckDelay = 70;//Init�м��RSP��ʱ��
static uint8_t MyWaterTS_Flag_TaskInit_Succed = 0;

static const MyWaterTS_Write0State_TypeDef MyWaterTS_Write0StateOrder[2] = {
	W0_Down, W0_Rise
};//Write0��״̬ת��˳��
static uint8_t MyWaterTS_Write0StateOrder_Index = 0;//Write0��״̬ת��ָ��

static const MyWaterTS_Write1State_TypeDef MyWaterTS_Write1StateOrder[3] = {
	W1_Down, W1_Rise, W1_Wait
};
static uint8_t MyWaterTS_Write1StateOrder_Index = 0;

static const MyWaterTS_ReadBitState_TypeDef MyWaterTS_ReadBitStateOrder[4] = {
	R_Down, R_Rise, R_Check, R_Wait
};
static uint8_t MyWaterTS_ReadBitStateOrder_Index = 0;

static uint8_t MyWaterTS_Flag_TaskSuccedCheckTimer_RunStatus = 0;//Task�ĳɹ���麯���Ķ�ʱ����

static uint8_t MyWaterTS_Count_BitReaded = 0;//��¼��Task�غ����Ѿ���ȡ��Bit��

static uint8_t MyWaterTS_ReadPacket[72];//����װ��SCRATCHPADȫ��8Byte���ݵ�����

uint16_t MyWaterTS_ReadPacket_16Bit_Temp = 0;//�ݴ���,����ReadPacketConvert()��

uint16_t MyWaterTS_Result_12Bit_H7Bit = 0;//12Bit�ֱ��ʽ������������

uint16_t MyWaterTS_Result_12Bit_L4Bit = 0;//12Bit�ֱ��ʽ����С������

static uint16_t MyWaterTS_Count_TaskSuccedCheckTimer_RunTimes = 0;

static void MyWaterTS_Task_Reset(void) {//Reset��TurnOn�������ظ�,ֻ��TurnOnִ�м���
	
	MyWaterTS_TaskStateOrder_Index = 0;//IDLE
}

static void MyWaterTS_TaskSuccedCkeck(void) {
	
	if((MyWaterTS_Flag_Task_Succed == 1) && 
		(MyWaterTS_Flag_TaskInit_Succed == 1)) {//�ɹ�
		
//		Serial_SendByte(USART2, 'T');//��Debug��
//		Serial_SendByte(USART2, 'S');//��Debug��
//		
//		Serial_SendByte(USART2, '\r');//��Debug��
//		Serial_SendByte(USART2, '\n');//��Debug��
	} else {//ʧ��
		
		MyWaterTS_Task_Reset();
		
//		Serial_SendByte(USART2, 'T');//��Debug��
//		Serial_SendByte(USART2, 'F');//��Debug��
//		
//		Serial_SendByte(USART2, '\r');//��Debug��
//		Serial_SendByte(USART2, '\n');//��Debug��
	}
}

static void MyWaterTS_ReadPacketConvert(void) {
	
	MyWaterTS_ReadPacket_16Bit_Temp = 0;//'|'����ǰ�Ƚ�ÿ��bit����
	
	for(uint8_t i = 0; i <16 ; i ++) {//��16Bit_Temp���ߵ�λ˳������
		MyWaterTS_ReadPacket_16Bit_Temp |= MyWaterTS_ReadPacket[i] << i;//16Bit_Tempԭ��ȫΪ0��
		//Racket[]��������ʲô����ʲô
	}
	
	if(MyWaterTS_ReadPacket_16Bit_Temp>>15 == 1) {//���Ϊ����ȡ������+1
		MyWaterTS_ReadPacket_16Bit_Temp = ~MyWaterTS_ReadPacket_16Bit_Temp + 1;
	}
	
	MyWaterTS_Result_12Bit_H7Bit = MyWaterTS_ReadPacket_16Bit_Temp>>4;
	
	uint8_t mask = (1 << 4) - 1;//8Bit����:0000 1111b
	
	MyWaterTS_Result_12Bit_L4Bit = MyWaterTS_ReadPacket_16Bit_Temp & mask;
	//���ص㡿��λ��'&'������ԭ������Bit����'1'�ŵ�'1'�������'0'��'&'������Զ��'0'����'1'��'&'������Զ����
	//���ص㡿������'|'������ԭ����'0'��'|'������Զ���䣬��'1'��'|'������Զ��'1'
	
}

void MyWaterTS_Init(void) {//����㡿��©Init
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyWaterTSDQ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyWaterTSDQ, &GPIO_InitStruct);
}

void MyWaterTS_TaskSM_TurnOn(void) {//��Reset
	
//	Serial_SendByte(USART2, 'T');//��Debug��
//	Serial_SendByte(USART2, '\r');//��Debug��
//	Serial_SendByte(USART2, '\n');//��Debug��
	
	MyWaterTS_TaskStateOrder_Index = 1;//TaskSM����Init��ʼ����
	MyWaterTS_Flag_TaskSuccedCheckTimer_RunStatus = 1;//����TaskSuccedCheck
	MyWaterTS_Count_TaskSuccedCheckTimer_RunTimes = 0;
	MyWaterTS_Target_Init_CkeckDelay = 130;//Init��Checkʱ��
	MyWaterTS_Flag_Task_Succed = 0;
	MyWaterTS_InitStateOrder_Index = 0;
	MyWaterTS_Write0StateOrder_Index = 0;
	MyWaterTS_Write1StateOrder_Index = 0;
	MyWaterTS_ReadBitStateOrder_Index = 0;
	MyWaterTS_Count_BitReaded = 0;
}

void MyWaterTS_TaskSuccedCheckTimer(void) {
	
	if(MyWaterTS_Flag_TaskSuccedCheckTimer_RunStatus == 1) {
		
		MyWaterTS_Count_TaskSuccedCheckTimer_RunTimes ++;
		
		if(2000000 <= MyWaterTS_Count_TaskSuccedCheckTimer_RunTimes * 10000) {//Trun=0.01s
			
			MyWaterTS_TaskSuccedCkeck();
			
			MyWaterTS_Count_TaskSuccedCheckTimer_RunTimes = 0;
			
			MyWaterTS_Flag_TaskSuccedCheckTimer_RunStatus = 0;
			//Serial_SendByte(USART2, 'C');//��Debug��
		}
	}
}

void MyWaterTS_TaskSM(void) {
	switch(MyWaterTS_TaskStateOrder[MyWaterTS_TaskStateOrder_Index]) {
		case T_IDLE:
			
//			Serial_SendByte(USART2, 'I');//��Debug��
//			Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//			
//			Serial_SendByte(USART2, '\r');//��Debug��
//			Serial_SendByte(USART2, '\n');//��Debug��
			
			break;
		case T_Init://����case˳��̶�,������Ҫ�ٿɱ���
			
			MyWaterTS_Count_TaskSM_RunTimes ++;
		
			switch(MyWaterTS_InitStateOrder[MyWaterTS_InitStateOrder_Index]) {
				
				case I_Down:
					
//					Serial_SendByte(USART2, 'I');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_InitStateOrder_Index);//��Debug��
				
					MyWaterTSDQ_SET;
				
					MyWaterTSDQ_RESET;
				
					MyWaterTS_Count_TaskSM_RunTimes = 0;
				
					MyWaterTS_InitStateOrder_Index ++;
					
					break;
				case I_Rise:
					
					if(500 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, 'I');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_InitStateOrder_Index);//��Debug��
						
						MyWaterTSDQ_SET;
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_InitStateOrder_Index ++;
					}
					break;
				case I_Check:
					
					if(MyWaterTS_Target_Init_CkeckDelay <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						if(!(MyWaterTSDQ_Read)) {
							
//							Serial_SendByte(USART2, 'I');//��Debug��
//							Serial_SendByte(USART2, 'c');//��Debug��
//							Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//							Serial_SendInteger(USART2, MyWaterTS_InitStateOrder_Index);//��Debug��
							
							MyWaterTS_Flag_TaskInit_Succed = 1;
							
							MyWaterTS_Count_TaskSM_RunTimes = 0;
							
							MyWaterTS_InitStateOrder_Index ++;
						} else {
							MyWaterTS_Flag_TaskInit_Succed = 0;
						}
					}
					break;
				case I_Wait:
					
					if(570 - MyWaterTS_Target_Init_CkeckDelay <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, 'I');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_InitStateOrder_Index);//��Debug��
//						
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_InitStateOrder_Index = 0;
						
						MyWaterTS_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write0:
			
			MyWaterTS_Count_TaskSM_RunTimes ++;
		
			switch (MyWaterTS_Write0StateOrder[MyWaterTS_Write0StateOrder_Index]) {
				
				case W0_Down:
					
//					Serial_SendByte(USART2, '0');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_Write0StateOrder_Index);//��Debug��
					
					MyWaterTSDQ_RESET;
					
					MyWaterTS_Count_TaskSM_RunTimes = 0;
					
					MyWaterTS_Write0StateOrder_Index ++;
					break;
				case W0_Rise:
					
					if(60 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '0');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_Write0StateOrder_Index);//��Debug��
//												
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyWaterTSDQ_SET;
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_Write0StateOrder_Index = 0;
						
						MyWaterTS_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write1:
			
			MyWaterTS_Count_TaskSM_RunTimes ++;
		
			switch (MyWaterTS_Write1StateOrder[MyWaterTS_Write1StateOrder_Index]) {
				
				case W1_Down:
					
//					Serial_SendByte(USART2, '1');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_Write1StateOrder_Index);//��Debug��
					
					MyWaterTSDQ_RESET;
					
					MyWaterTS_Count_TaskSM_RunTimes = 0;
					
					MyWaterTS_Write1StateOrder_Index ++;
					
					break;
				
				case W1_Rise:
					
					if(10 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '1');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_Write1StateOrder_Index);//��Debug��
						
						MyWaterTSDQ_SET;
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_Write1StateOrder_Index ++;
					}
					break;
					
				case W1_Wait:
					
					if(50 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '1');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_Write1StateOrder_Index);//��Debug��
//												
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_Write1StateOrder_Index = 0;
						
						MyWaterTS_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
			
		case T_ReadBit:
			
			MyWaterTS_Count_TaskSM_RunTimes ++;
			
			switch (MyWaterTS_ReadBitStateOrder[MyWaterTS_ReadBitStateOrder_Index]) {
				
				case R_Down:
					
//					Serial_SendByte(USART2, 'R');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyWaterTS_ReadBitStateOrder_Index);//��Debug��
					
					MyWaterTSDQ_RESET;
					
					MyWaterTS_Count_TaskSM_RunTimes = 0;
					
					MyWaterTS_ReadBitStateOrder_Index ++;
					
					break;
					
				case R_Rise:
					
					if(5 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, 'R');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_ReadBitStateOrder_Index);//��Debug��
						
						MyWaterTSDQ_SET;
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_ReadBitStateOrder_Index ++;
					}
					break;//�������Ѿ�Down/Reset��5us
				
				case R_Check://��WARN:С��Bug��
					
					if(5 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {//��1����������Down��ʼ���10us,2����15us
						
//						Serial_SendByte(USART2, 'R');//��Debug��
//						Serial_SendByte(USART2, 'c');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_ReadBitStateOrder_Index);//��Debug��
						
						if(MyWaterTSDQ_Read) {
							
							//Serial_SendByte(USART2, '1');//��Debug��
							
							MyWaterTS_ReadPacket[MyWaterTS_Count_BitReaded ++] = 1;
							
						} else{
							
							//Serial_SendByte(USART2, '0');//��Debug��
							
							MyWaterTS_ReadPacket[MyWaterTS_Count_BitReaded ++] = 0;
						}
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_ReadBitStateOrder_Index ++;
					}
					break;
					
				case R_Wait:

					if(45 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {//����45��
						
//						Serial_SendByte(USART2, 'R');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyWaterTS_ReadBitStateOrder_Index);//��Debug��
//						
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
					
						
						MyWaterTS_Count_TaskSM_RunTimes = 0;
						
						MyWaterTS_ReadBitStateOrder_Index = 0;
						
						MyWaterTS_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
			
		case T_Wait750ms:
			
			MyWaterTS_Count_TaskSM_RunTimes ++;
			
			if(750000 <= MyWaterTS_Count_TaskSM_RunTimes * 5) {//�����Ե㡿

//				Serial_SendByte(USART2, 'W');//��Debug��
//				Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//								
//				Serial_SendByte(USART2, '\r');//��Debug��
//				Serial_SendByte(USART2, '\n');//��Debug��
				
				MyWaterTS_Count_TaskSM_RunTimes = 0;
				
				MyWaterTS_TaskStateOrder_Index ++;
			}
			break;
			
		case T_Convert:
			
//			for(int8_t i = 15; i >= 0; i--) {
//				Serial_SendInteger(USART2, MyWaterTS_ReadPacket[i]);//��Debug��
//			}
				
//			Serial_SendByte(USART2, 'C');//��Debug��
//			Serial_SendInteger(USART2, MyWaterTS_TaskStateOrder_Index);//��Debug��
//			
//			Serial_SendByte(USART2, '\r');//��Debug��
//			Serial_SendByte(USART2, '\n');//��Debug��
			
			MyWaterTS_ReadPacketConvert();
			
			MyWaterTS_Flag_Task_Succed = 1;
			
			MyWaterTS_TaskStateOrder_Index = 0;
		
			break;
	}
}
