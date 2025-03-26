#include "MyDS18B20.h"


enum MyDS18B20_TaskState_TypeDef {
	T_Write0 = 0,	//0
	T_Write1,		//1
	T_ReadBit,		//2
	T_Init,			//3
	T_Wait750ms,	//4
	T_Convert,		//5
	T_IDLE,			//6
};

enum MyDS18B20_InitState_TypeDef {
	I_Down = 0,
	I_Rise,
	I_Check,
	I_Wait
};

enum MyDS18B20_Write0State_TypeDef {
	W0_Down = 0,
	W0_Rise
};

enum MyDS18B20_Write1State_TypeDef {
	W1_Down = 0,
	W1_Rise,
	W1_Wait
};

enum MyDS18B20_ReadBitState_TypeDef {
	R_Down = 0,
	R_Rise,
	R_Check,
	R_Wait
};

GPIO_InitTypeDef GPIO_InitStruct;

const uint8_t MyDS18B20_TaskStateOrder[100] = {
	T_IDLE,
	T_Init, 
	T_Write1, T_Write1, T_Write0, T_Write0, T_Write1, T_Write1, T_Write0, T_Write0, 
	T_Write0, T_Write1, T_Write0, T_Write0, T_Write0, T_Write1, T_Write0, T_Write0, 
	T_Wait750ms, 
	T_Init, 
	T_Write1, T_Write1, T_Write0, T_Write0, T_Write1, T_Write1, T_Write0, T_Write0, 
	T_Write1, T_Write0, T_Write1, T_Write1, T_Write1, T_Write1, T_Write1, T_Write0, 
	T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, 
	T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, T_ReadBit, 
	T_Convert
};//TaskSM的状态转换顺序
uint8_t MyDS18B20_TaskStateOrder_Index = 0;//TaskSM的状态转换指引
uint32_t MyDS18B20_Count_TaskSM_RunTimes = 0;//其下所有状态机和子状态机共用
uint8_t MyDS18B20_Flag_Task_Succed = 0;//TaskSM的成功状态标志位

const uint8_t MyDS18B20_InitStateOrder[4] = {
	I_Down, I_Rise, I_Check, I_Wait
};//Init的状态转换顺序
uint8_t MyDS18B20_InitStateOrder_Index = 0;//Init的状态转换指引
uint8_t MyDS18B20_Target_Init_CkeckDelay = 70;//Init中检查RSP的时机

const uint8_t MyDS18B20_Write0StateOrder[2] = {
	W0_Down, W0_Rise
};//Write0的状态转换顺序
uint8_t MyDS18B20_Write0StateOrder_Index = 0;//Write0的状态转换指引

const uint8_t MyDS18B20_Write1StateOrder[3] = {
	W1_Down, W1_Rise, W1_Wait
};
uint8_t MyDS18B20_Write1StateOrder_Index = 0;

const uint8_t MyDS18B20_ReadBitStateOrder[4] = {
	R_Down, R_Rise, R_Check, R_Wait
};
uint8_t MyDS18B20_ReadBitStateOrder_Index = 0;

uint8_t MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 0;//Task的成功检查函数的定时启动

uint8_t MyDS18B20_Count_BitReaded = 0;//记录本Task回合中已经读取的Bit数

uint8_t MyDS18B20_ReadPacket[72];//可以装下SCRATCHPAD全部8Byte数据的数组

uint16_t MyDS18B20_Result_7BitUint_Temp = 0;//暂存结果,用在ReadPacketConvert()中

uint16_t MyDS18B20_Result_7BitUint = 0;//转换结果

uint16_t MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;

void MyDS18B20_Init(void) {//【错点】遗漏Init
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_MyDS18B20DQ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MyDS18B20DQ, &GPIO_InitStruct);
}

void MyDS18B20_TaskSM_TurnOn(void) {//兼Reset
	
	//Serial_SendByte(USART2, 'T');
	Serial_SendByte(USART2, '\r');
	Serial_SendByte(USART2, '\n');
	
	MyDS18B20_TaskStateOrder_Index = 1;//TaskSM进入Init开始运行
	MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 1;//启动TaskSuccedCheck
	MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;
	MyDS18B20_Target_Init_CkeckDelay = 130;//Init的Check时机
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
		
		if(400 <= MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes) {
			
			MyDS18B20_TaskSuccedCkeck();
			
			MyDS18B20_Count_TaskSuccedCheckTimer_RunTimes = 0;
			
			MyDS18B20_Flag_TaskSuccedCheckTimer_RunStatus = 0;
			//Serial_SendByte(USART2, 'C');
		}
	}
}

void MyDS18B20_TaskSuccedCkeck(void) {
	if(MyDS18B20_Flag_Task_Succed == 1) {//成功
		
	} else if(MyDS18B20_Flag_Task_Succed  == 0) {//失败
		//MyDS18B20_Task_Reset();
		//Serial_SendByte(USART2, 'f');
	}
}

void MyDS18B20_Task_Reset(void) {//Reset和TurnOn中若有重复,只让TurnOn执行即可
	MyDS18B20_TaskStateOrder_Index = 0;//IDLE
}

void MyDS18B20_TaskSM(void) {
	switch(MyDS18B20_TaskStateOrder[MyDS18B20_TaskStateOrder_Index]) {
		case T_IDLE:
			
		//Serial_SendByte(USART2, 'I');
			
			break;
		case T_Init://期内case顺序固定,尽量不要再手动编排
			MyDS18B20_Count_TaskSM_RunTimes ++;
			switch(MyDS18B20_InitStateOrder[MyDS18B20_InitStateOrder_Index]) {
				case I_Down:
					
					Serial_SendByte(USART2, 'd');
					Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);
					Serial_SendInteger(USART2, MyDS18B20_TaskStateOrder_Index);
				
					MyDS18B20DQ_RESET;
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					MyDS18B20_InitStateOrder_Index ++;
					
					break;
				case I_Rise:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(480 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
						Serial_SendByte(USART2, 'r');
						Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);
						
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_InitStateOrder_Index ++;
					}
					break;
				case I_Check:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(MyDS18B20_Target_Init_CkeckDelay <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						if(!(MyDS18B20DQ_Read)) {
							
							Serial_SendByte(USART2, 'p');
							Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);
							
							MyDS18B20_Flag_Task_Succed = 0;
							
							MyDS18B20_Count_TaskSM_RunTimes = 0;
							MyDS18B20_InitStateOrder_Index ++;
						}
					}
					break;
				case I_Wait:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(480 - MyDS18B20_Target_Init_CkeckDelay <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
						Serial_SendByte(USART2, 'a');
						Serial_SendInteger(USART2, MyDS18B20_InitStateOrder_Index);
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_InitStateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write0:
			//Serial_SendByte(USART2, '0');
			MyDS18B20_Count_TaskSM_RunTimes ++;
			switch (MyDS18B20_Write0StateOrder[MyDS18B20_Write0StateOrder_Index]) {
				case W0_Down:
					MyDS18B20DQ_RESET;
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					MyDS18B20_Write0StateOrder_Index ++;
					break;
				case W0_Rise:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(60 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write0StateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
		case T_Write1:
			//Serial_SendByte(USART2, '1');
			MyDS18B20_Count_TaskSM_RunTimes ++;
			switch (MyDS18B20_Write1StateOrder[MyDS18B20_Write1StateOrder_Index]) {
				case W1_Down:
					MyDS18B20DQ_RESET;
					MyDS18B20_Count_TaskSM_RunTimes = 0;
					MyDS18B20_Write0StateOrder_Index ++;
					
					//Serial_SendByte(USART2, 'd');
					break;
				case W1_Rise:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(15 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						MyDS18B20DQ_SET;
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write0StateOrder_Index ++;
					
						//Serial_SendByte(USART2, 'r');
					}
					break;
				case W1_Wait:
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					if(45 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						MyDS18B20_Write0StateOrder_Index = 0;
						MyDS18B20_TaskStateOrder_Index ++;
						
						
						//Serial_SendByte(USART2, 'a');
					}
					break;
			}
			break;
			
		case T_ReadBit:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
			
			switch (MyDS18B20_ReadBitStateOrder[MyDS18B20_ReadBitStateOrder_Index]) {
				
				case R_Down:
					
					MyDS18B20DQ_RESET;
					
					//MyDS18B20_Count_TaskSM_RunTimes = 0;
					
					MyDS18B20_ReadBitStateOrder_Index ++;
					
					break;
					
				case R_Rise:
					
					MyDS18B20DQ_SET;
					
					//MyDS18B20_Count_TaskSM_RunTimes = 0;
					
					MyDS18B20_ReadBitStateOrder_Index ++;
					
					break;
				
				case R_Check://【WARN:小心Bug】
					
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					
					if(10 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
						Serial_SendByte(USART2, 'r');
						
						if(MyDS18B20DQ_Read) {
							
							Serial_SendByte(USART2, '1');
							
							MyDS18B20_ReadPacket[MyDS18B20_Count_BitReaded ++] = 1;
							
						} else{
							
							Serial_SendByte(USART2, '0');
							
							MyDS18B20_ReadPacket[MyDS18B20_Count_BitReaded ++] = 0;
						}
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						
						MyDS18B20_ReadBitStateOrder_Index ++;
					}
					break;
					
				case R_Wait:
					
					//MyDS18B20_Count_TaskSM_RunTimes ++;
					
					if(45 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
						
						MyDS18B20_Count_TaskSM_RunTimes = 0;
						
						MyDS18B20_ReadBitStateOrder_Index = 0;
						
						MyDS18B20_TaskStateOrder_Index ++;
					}
					break;
			}
			break;
			
		case T_Wait750ms:
			
			MyDS18B20_Count_TaskSM_RunTimes ++;
			
			if(1000000 <= MyDS18B20_Count_TaskSM_RunTimes * 5) {
				
				Serial_SendByte(USART2, 'A');
				
				MyDS18B20_Count_TaskSM_RunTimes = 0;
				
				MyDS18B20_TaskStateOrder_Index ++;
			}
			break;
		case T_Convert:
			
			Serial_SendByte(USART2, 'V');
			
			MyDS18B20_ReadPacketConvert();
			
			MyDS18B20_Flag_Task_Succed = 1;
			
			MyDS18B20_TaskStateOrder_Index = 0;
			
			break;
	}
}

void MyDS18B20_ReadPacketConvert(void) {
	
	MyDS18B20_Result_7BitUint_Temp = 0;
	
	for(uint8_t i = 0; i < 7; i ++) {
		
		MyDS18B20_Result_7BitUint_Temp |= (MyDS18B20_ReadPacket[i + 5] << i);
	}
	if(MyDS18B20_ReadPacket[0] == 1) {
		
		MyDS18B20_Result_7BitUint = ~MyDS18B20_Result_7BitUint_Temp + 1;
	
	} else{
		
		MyDS18B20_Result_7BitUint = MyDS18B20_Result_7BitUint_Temp;
	}
}

