#include "MyDS18B20.h"

typedef enum {
	T_Write0 = 0,
	T_Write1 = 1,
	T_ReadBit = 2,
	T_Init = 3,
	T_Wait750ms = 4,
	T_Convert = 5,
	T_IDLE = 6
} MyDS18B20_TaskState_TypeDef;

typedef enum {
	I_Down = 0,
	I_Rise = 1,
	I_Check = 2,
	I_Wait = 3
} MyDS18B20_InitState_TypeDef;

typedef enum {
	W0_Down = 0,
	W0_Rise = 1
} MyDS18B20_Write0State_TypeDef;

typedef enum {
	W1_Down = 0,
	W1_Rise = 1,
	W1_Wait = 2
} MyDS18B20_Write1State_TypeDef;

typedef enum {
	R_Down = 0,
	R_Rise = 1,
	R_Check = 2,
	R_Wait = 3
} MyDS18B20_ReadBitState_TypeDef;

static GPIO_InitTypeDef GPIO_InitStruct;

static const MyDS18B20_TaskState_TypeDef MyDS18B20_TaskStateOrder[125] = {//����㡿��������ʱ��һ���ֽ��ڵı���û�е�λ����
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
static uint8_t MyDS18B20_TaskStateOrder_Index = 0;//TaskSM��״̬ת��ָ��
static uint32_t MyDS18B20_Count_TaskSM_RunTimes = 0;//��������״̬������״̬������
static uint8_t MyDS18B20_Flag_Task_Succed = 0;//TaskSM�ĳɹ�״̬��־λ

static const MyDS18B20_InitState_TypeDef MyDS18B20_InitStateOrder[4] = {
	I_Down, I_Rise, I_Check, I_Wait
};//Init��״̬ת��˳��
static uint8_t MyDS18B20_InitStateOrder_Index = 0;//Init��״̬ת��ָ��
static uint8_t MyDS18B20_Target_Init_CkeckDelay = 70;//Init�м��RSP��ʱ��
static uint8_t MyDS18B20_Flag_TaskInit_Succed = 0;

static const MyDS18B20_Write0State_TypeDef MyDS18B20_Write0StateOrder[2] = {
	W0_Down, W0_Rise
};//Write0��״̬ת��˳��
static uint8_t MyDS18B20_Write0StateOrder_Index = 0;//Write0��״̬ת��ָ��

static const MyDS18B20_Write1State_TypeDef MyDS18B20_Write1StateOrder[3] = {
	W1_Down, W1_Rise, W1_Wait
};
static uint8_t MyDS18B20_Write1StateOrder_Index = 0;

static const MyDS18B20_ReadBitState_TypeDef MyDS18B20_ReadBitStateOrder[4] = {
	R_Down, R_Rise, R_Check, R_Wait
};
static uint8_t MyDS18B20_ReadBitStateOrder_Index = 0;

static uint8_t MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 0;//Task�ĳɹ���麯���Ķ�ʱ����

static uint8_t MyDS18B20_Count_BitReaded = 0;//��¼��Task�غ����Ѿ���ȡ��Bit��

static uint8_t MyDS18B20_ReadPacket[72];//����װ��SCRATCHPADȫ��8Byte���ݵ�����

uint16_t MyDS18B20_ReadPacket_16Bit_Temp = 0;//�ݴ���,����ReadPacketConvert()��

uint16_t MyDS18B20_Result_12Bit_H7Bit = 0;//12Bit�ֱ��ʽ������������

uint16_t MyDS18B20_Result_12Bit_L4Bit = 0;//12Bit�ֱ��ʽ����С������

static uint16_t MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;

void MyDS18B20_Test1(void) {
	
	Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder[0]);
	Serial_SendInteger(USART2, MyDS18B20_InitStateOrder[0]);
	Serial_SendInteger(USART2, MyDS18B20_Write1StateOrder[0]);
	Serial_SendInteger(USART2, MyDS18B20_Write0StateOrder[0]);
	Serial_SendInteger(USART2, MyDS18B20_ReadBitStateOrder[0]);
	
	Serial_SendByte(USART2, '\r');
	Serial_SendByte(USART2, '\n');
}

static void MyDS18B20_Task_Reset(void) {//Reset��TurnOn�������ظ�,ֻ��TurnOnִ�м���
	
	MyDS18B20_TaskStateOrder_Index = 0;//IDLE
}

static void MyDS18B20_TaskSuccedCkeck(void) {
	
	if((MyDS18B20_Flag_Task_Succed == 1) && 
		(MyDS18B20_Flag_TaskInit_Succed == 1)) {//�ɹ�
		
		Serial_SendByte(USART2, 'T');//��Debug��
		Serial_SendByte(USART2, 'S');//��Debug��
		
		Serial_SendByte(USART2, '\r');//��Debug��
		Serial_SendByte(USART2, '\n');//��Debug��
	} else {//ʧ��
		
		MyDS18B20_Task_Reset();
		
		Serial_SendByte(USART2, 'T');//��Debug��
		Serial_SendByte(USART2, 'F');//��Debug��
		
		Serial_SendByte(USART2, '\r');//��Debug��
		Serial_SendByte(USART2, '\n');//��Debug��
	}
}

static void MyDS18B20_ReadPacketConvert(void) {
	
//	MyDS18B20_Result_7BitUint_Temp = 0;
//	
//	for(uint8_t i = 0; i < 7; i ++) {
//		
//		MyDS18B20_Result_7BitUint_Temp |= (MyDS18B20_ReadPacket[i + 5] << i);
//	}
//	if(MyDS18B20_ReadPacket[0] == 1) {
//		
//		MyDS18B20_Result_7BitUint = ~MyDS18B20_Result_7BitUint_Temp + 1;
//	
//	} else{
//		
//		MyDS18B20_Result_7BitUint = MyDS18B20_Result_7BitUint_Temp;
//	}
	
	MyDS18B20_ReadPacket_16Bit_Temp = 0;//'|'����ǰ�Ƚ�ÿ��bit����
	
	for(uint8_t i = 0; i <16 ; i ++) {
		MyDS18B20_ReadPacket_16Bit_Temp |= MyDS18B20_ReadPacket[i] << i;//16Bit_Tempԭ��ȫΪ0��
		//Racket[]��������ʲô����ʲô
	}
	
	if(MyDS18B20_ReadPacket_16Bit_Temp>>15 == 1) {//���Ϊ����ȡ������+1
		MyDS18B20_ReadPacket_16Bit_Temp = ~MyDS18B20_ReadPacket_16Bit_Temp + 1;
	}
	
	MyDS18B20_Result_12Bit_H7Bit = MyDS18B20_ReadPacket_16Bit_Temp>>4;
	
	uint8_t mask = (1 << 4) - 1;//8Bit����:0000 1111b
	
	MyDS18B20_Result_12Bit_L4Bit = MyDS18B20_ReadPacket_16Bit_Temp & mask;
	//���ص㡿��λ��'&'������ԭ������Bit����'1'�ŵ�'1'�������'0'��'&'������Զ��'0'����'1'��'&'������Զ����
	//���ص㡿������'|'������ԭ����'0'��'|'������Զ���䣬��'1'��'|'������Զ��'1'
	
}

void MyDS18B20_Init(void) {//����㡿��©Init
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyDS18B20DQ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyDS18B20DQ, &GPIO_InitStruct);
}

void MyDS18B20_TaskSM_TurnOn(void) {//��Reset
	
//	Serial_SendByte(USART2, 'T');//��Debug��
//	Serial_SendByte(USART2, '\r');//��Debug��
//	Serial_SendByte(USART2, '\n');//��Debug��
	
	MyDS18B20_TaskStateOrder_Index = 1;//TaskSM����Init��ʼ����
	MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 1;//����TaskSuccedCheck
	MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;
	MyDS18B20_Target_Init_CkeckDelay = 130;//Init��Checkʱ��
	MyDS18B20_Flag_Task_Succed = 0;
	MyDS18B20_InitStateOrder_Index = 0;
	MyDS18B20_Write0StateOrder_Index = 0;
	MyDS18B20_Write1StateOrder_Index = 0;
	MyDS18B20_ReadBitStateOrder_Index = 0;
	MyDS18B20_Count_BitReaded = 0;
}

void MyDS18B20_TaskSuccedCheckTimer(void) {
	
	if(MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus == 1) {
		
		MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes ++;
		
		if(2000000 <= MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes * 10000) {//Trun=0.01s
			
			MyDS18B20_TaskSuccedCkeck();
			
			MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;
			
			MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 0;
			//Serial_SendByte(USART2, 'C');//��Debug��
		}
	}
}

void MyDS18B20_TaskSM(void) {
	switch(MyDS18B20_TaskStateOrder[MyDS18B20_TaskStateOrder_Index]) {
		case T_IDLE:
			
//			Serial_SendByte(USART2, 'I');//��Debug��
//			Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//			
//			Serial_SendByte(USART2, '\r');//��Debug��
//			Serial_SendByte(USART2, '\n');//��Debug��
			
			break;
		case T_Init://����case˳��̶�,������Ҫ�ٿɱ���
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
		
			switch(MyDS18B20_InitStateOrder[MyDS18B20_InitStateOrder_Index]) {
				
				case I_Down:
					
//					Serial_SendByte(USART2, 'I');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);//��Debug��
				
					MyDS18B20DQ_SET;
				
					MyDS18B20DQ_RESET;
				
					MyDS18B20_Count_TaskSM_RunTimes = 0;
				
					MyDS18B20_InitStateOrder_Index ++;
					
					break;
				case I_Rise:
					
					if(500 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, 'I');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);//��Debug��
						
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_InitStateOrder_Index ++;
					}
					break;
				case I_Check:
					
					if(MyDS18B20_Target_Init_CkeckDelay <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						if(!(MyDS18B20DQ_Read)) {
							
//							Serial_SendByte(USART2, 'I');//��Debug��
//							Serial_SendByte(USART2, 'c');//��Debug��
//							Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//							Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);//��Debug��
							
							MyDS18B20_Flag_TaskInit_Succed = 1;
							
							MyDS18B20_Count_TaskSM_RunTimes = 0;
							MyDS18B20_InitStateOrder_Index ++;
						} else {
							MyDS18B20_Flag_TaskInit_Succed = 0;
						}
					}
					break;
				case I_Wait:
					
					if(570 - MyDS18B20_Target_Init_CkeckDelay <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, 'I');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);//��Debug��
//						
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_InitStateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write0:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
		
			switch (MyDS18B20_Write0StateOrder[MyDS18B20_Write0StateOrder_Index]) {
				
				case W0_Down:
					
//					Serial_SendByte(USART2, '0');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_Write0StateOrder_Index);//��Debug��
					
					MyDS18B20DQ_RESET;
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					MyDS18B20_Write0StateOrder_Index ++;
					break;
				case W0_Rise:
					
					if(60 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '0');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_Write0StateOrder_Index);//��Debug��
//												
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write0StateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write1:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
		
			switch (MyDS18B20_Write1StateOrder[MyDS18B20_Write1StateOrder_Index]) {
				
				case W1_Down:
					
//					Serial_SendByte(USART2, '1');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_Write1StateOrder_Index);//��Debug��
					
					MyDS18B20DQ_RESET;
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					MyDS18B20_Write1StateOrder_Index ++;
					
					break;
				
				case W1_Rise:
					
					if(10 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '1');//��Debug��
//						Serial_SendByte(USART2, 'r');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_Write1StateOrder_Index);//��Debug��
						
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write1StateOrder_Index ++;
					}
					break;
					
				case W1_Wait:
					
					if(50 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
//						Serial_SendByte(USART2, '1');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_Write1StateOrder_Index);//��Debug��
//												
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write1StateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
			
		case T_ReadBit:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
			
			switch (MyDS18B20_ReadBitStateOrder[MyDS18B20_ReadBitStateOrder_Index]) {
				
				case R_Down:
					
//					Serial_SendByte(USART2, 'R');//��Debug��
//					Serial_SendByte(USART2, 'd');//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_ReadBitStateOrder_Index);//��Debug��
					
					MyDS18B20DQ_RESET;
					
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					
					MyDS18B20_ReadBitStateOrder_Index ++;
					
					break;
					
				case R_Rise:
					
					if(5 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
//					Serial_SendByte(USART2, 'R');//��Debug��
//					Serial_SendByte(USART2, 'r');//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//					Serial_SendInteger(USART2, MyDS18B20_ReadBitStateOrder_Index);//��Debug��
					
					MyDS18B20DQ_SET;
					
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					
					MyDS18B20_ReadBitStateOrder_Index ++;
					
					}
					break;//�������Ѿ�Down/Reset��5us
				
				case R_Check://��WARN:С��Bug��
					
					if(5 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {//��1����������Down��ʼ���10us,2����15us
						
//						Serial_SendByte(USART2, 'R');//��Debug��
//						Serial_SendByte(USART2, 'c');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_ReadBitStateOrder_Index);//��Debug��
						
						if(MyDS18B20DQ_Read) {
							
							//Serial_SendByte(USART2, '1');//��Debug��
							
							MyDS18B20_ReadPacket[MyDS18B20_Count_BitReaded ++] = 1;
							
						} else{
							
							//Serial_SendByte(USART2, '0');//��Debug��
							
							MyDS18B20_ReadPacket[MyDS18B20_Count_BitReaded ++] = 0;
						}
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						
						MyDS18B20_ReadBitStateOrder_Index ++;
					}
					break;
					
				case R_Wait:

					if(45 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {//����45��
						
//						Serial_SendByte(USART2, 'R');//��Debug��
//						Serial_SendByte(USART2, 'w');//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//						Serial_SendInteger(USART2, MyDS18B20_ReadBitStateOrder_Index);//��Debug��
//						
//						Serial_SendByte(USART2, '\r');//��Debug��
//						Serial_SendByte(USART2, '\n');//��Debug��
					
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						
						MyDS18B20_ReadBitStateOrder_Index = 0;
						
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
			
		case T_Wait750ms:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
			
			if(750000 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {//�����Ե㡿

//				Serial_SendByte(USART2, 'W');//��Debug��
//				Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//								
//				Serial_SendByte(USART2, '\r');//��Debug��
//				Serial_SendByte(USART2, '\n');//��Debug��
				
				MyDS18B20_Count_TaskSM_RunTimes = 0;
				
				MyDS18B20_TaskStateOrder_Index ++;
			}
			break;
			
		case T_Convert:
			
			for(int8_t i = 15; i >= 0; i--) {
				Serial_SendInteger(USART2, MyDS18B20_ReadPacket[i]);//��Debug��
			}
				
//			Serial_SendByte(USART2, 'C');//��Debug��
//			Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);//��Debug��
//			
			Serial_SendByte(USART2, '\r');//��Debug��
			Serial_SendByte(USART2, '\n');//��Debug��
			
			MyDS18B20_ReadPacketConvert();
			
			MyDS18B20_Flag_Task_Succed = 1;
			
			MyDS18B20_TaskStateOrder_Index = 0;
		
			break;
	}
}
