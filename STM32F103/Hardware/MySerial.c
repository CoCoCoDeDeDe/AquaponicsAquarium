#include "MySerial.h"

static USART_InitTypeDef USART_InitStruct;

uint8_t Serial_RxFlag[4] = {0, 0, 0, 0};	//Serial_RxFlag[ser]
//用于标记状态机是否把一整个String接收完，否则不输出新接收的String等等

char Serial_Rx3StringPacket[244];
char Serial_Rx2StringPacket[244];

uint8_t StateMachine_s3 = 0;
uint8_t StateMachine_count3 = 0;
uint8_t StateMachine_s2 = 0;
uint8_t StateMachine_count2 = 0;

void Serial_Init(
	USART_TypeDef*	USARTx, 
	uint32_t		USART_BaudRate, 
	uint8_t			NVIC_IRQChannelPreemptionPriority, 
	uint8_t			NVIC_IRQChannelSubPriority
	) {
	
	GPIO_TypeDef*	GPIOTx;
	uint16_t		GPIO_Pin_Tx;
	GPIO_TypeDef*	GPIORx;
	uint16_t		GPIO_Pin_Rx;
		
			
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//NVIC
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	uint8_t NVIC_IRQChannel;

	if(USARTx == USARTESP8266) {
		GPIOTx = GPIORx = GPIOESP8266;
		GPIO_Pin_Tx = GPIO_Pin_ESP8266Tx;
		GPIO_Pin_Rx = GPIO_Pin_ESP8266Rx;
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		NVIC_IRQChannel = USARTESP8266_IRQn;
		
	} else if(USARTx == USARTPC) {
		GPIOTx = GPIORx =	GPIOPC;
		GPIO_Pin_Tx =		GPIO_Pin_PCTx;
		GPIO_Pin_Rx =		GPIO_Pin_PCRx;
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
		NVIC_IRQChannel = USARTPC_IRQn;
		
	}//USARTx应为USART3（对应STM32-ESP8266）或USART2（对应STM32-PC）
	
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_Tx;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOTx, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_Rx;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIORx, &GPIO_InitStruct);
	
	USART_StructInit(&USART_InitStruct);
	
	USART_InitStruct.USART_BaudRate = USART_BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USARTx, &USART_InitStruct);
	
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
	
	
	NVIC_InitStruct.NVIC_IRQChannel = NVIC_IRQChannel;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	
	NVIC_Init(&NVIC_InitStruct);
	
	
	USART_Cmd(USARTx, ENABLE);
	
	//Serial_SendByte(USART2, 'I');//【Debug】
}

void Serial_BaudRateConfig(USART_TypeDef* USARTx, uint32_t USART_BaudRate) {
	//对应的Serial在Init后才可使用该函数
	USART_InitStruct.USART_BaudRate = USART_BaudRate;
	
	USART_Init(USARTx, &USART_InitStruct);
}

void Serial_Auto_StateMachine(
	USART_TypeDef *USARTx, 
	uint8_t ser, 
	uint8_t *s, 
	uint8_t *count, 
	char Serial_RxStringPacket[], 
	uint8_t size) {
		
		//Serial_SendByte(USART2, 'S');//【Debug】
		
		uint16_t RxData = USART_ReceiveData(USARTx);
		//RXNE在USART_DR被读取后自动RESET（即0，数据没有收到）
		//下一轮USART_ReceiveData()运行对USART_DR进行写操作会将TXE自动置0即RESET，即数据未被完全转移至移位寄存器
		//USART_DR是TDR和RDR在软件概念上的合并
		//while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET);
		//等USART_DR被RxData读取完再继续
		
		switch (*s) {
			case 0:
//				if(RxData == '@' && Serial_RxFlag[ser] == 0) {	
//					//错点：遗漏判断Serial_RxFlag[ser]，即判断上一个String是否接收完毕。
//					
//					*s = 1; 
//					*count = 0;
//				} else {
//					//NOTHING
//				}
				if(Serial_RxFlag[ser] == 0) {
					*s = 1; 
					*count = 0;
					Serial_RxStringPacket[(*count)++] = RxData;
				}
				//抛弃'@'包头方案
				
				break;
			case 1:
				if(RxData == '\r') {
					Serial_RxStringPacket[(*count)++] = RxData;
					*s = 2;
				} else {
					if (*count < size - 1) {
						//错点：将数组传递到函数是以原始的指针的形式在函数内存在的，sizeof(Serial_RxStringPacket)会出warning。
						//解决方法：在数组传入函数时就将数组size传入函数
						//当 已经装了的位数(*count+1)<可以装的位数(sizeof(...)) 时才继续装填
						Serial_RxStringPacket[(*count)++] = RxData;
						//错点：*count++错误，++优先级高于*，需写为(*count)++
						}
					}
				break;
			case 2:
				if(RxData == '\n') {
					
					//Serial_SendByte(USART3, 'H');//【Debug】
					Serial_RxStringPacket[(*count)++] = RxData;
					*s = 0;
					Serial_RxStringPacket[*count] = '\0';
					*count = 0;
					Serial_RxFlag[ser] = 1;	
					//此处表示一整个String已经接受完，可进行读取变量等操作。
					
				} else {
					//NOTHING
				}
				break;
			default:
				//ERROR
				break;	//dafault:内若没有要执行的代码则需要加break;
		}
}

void USART2_IRQHandler() {
	
	//Serial_SendByte(USART2, 'h');//【Debug】
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET) {	//错点：将GetITStatus错写为GetFlagStatus
		
		//Serial_SendByte(USART2, 'H');//【Debug】
		
		//USART_ClearITPendingBit(USART2, USART_IT_RXNE);//自动清标志位
		
		uint16_t size = sizeof(Serial_Rx2StringPacket)/sizeof(Serial_Rx2StringPacket[0]);
		
		//进入状态机程序
		Serial_Auto_StateMachine(USART2, 2, &StateMachine_s2, &StateMachine_count2, Serial_Rx2StringPacket, size);
	}
}

void USART3_IRQHandler() {
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET) {	//错点：将GetITStatus错写为GetFlagStatus
		//当RDR移位寄存器中的数据被转移到USART_DR寄存器中（等待读取），该位被硬件置位（1即SET）。
		//如果USART_CR1寄存器中的RXNEIE为1，则产生中断。
		//对USART_DR的读操作（RxData = USART_ReceiveData(USARTx)）可以将该位清零（0即RESET）。
		
		//USART_ClearITPendingBit(USART3, USART_IT_RXNE);//自动清标志位
		
		uint16_t size = sizeof(Serial_Rx3StringPacket)/sizeof(Serial_Rx3StringPacket[0]);
		
		
		//进入状态机程序
		Serial_Auto_StateMachine(USART3, 3, &StateMachine_s3, &StateMachine_count3, Serial_Rx3StringPacket, size);
		
	}
}

void Serial_SendByte(USART_TypeDef* USARTx, uint8_t Byte) {
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);	
	//TXE置0：数据还未被完全转移至移位寄存器；TXE置1：数据已被完全转移至移位寄存器。
	//下一轮对USART_SendData()运行对USART_DR进行写操作会将TXE自动置0即RESET，即数据未被完全转移至移位寄存器
	//当TDR寄存器中的数据被硬件完全转移到移位寄存器的时候，该位被硬件置位即1即SET，即上while(...)可跳出
	//上行代码作用：等待上一轮Tx从USART_DR全部转移到移位寄存器
	
	USART_SendData(USARTx, Byte);
}

void Serial_SendString(USART_TypeDef* USARTx, char *string) {
	for(uint8_t i = 0; string[i] != '\0'; i++) {	//错点：将string[i]错写为i != '\0'
		Serial_SendByte(USARTx, string[i]);
	}
}

void Serial_SendStringV2(USART_TypeDef* USARTx, char* str) {
    while (*str) {
        Serial_SendByte(USARTx, *str++);
    }
}

void Serial_SendInteger(USART_TypeDef* USARTx, int num) {
    char buffer[20];
    sprintf(buffer, "%d", num);	//#include <stdio.h>
    Serial_SendStringV2(USART2, buffer);
}

