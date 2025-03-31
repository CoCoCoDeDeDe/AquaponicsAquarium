#include "MySerial.h"

static USART_InitTypeDef USART_InitStruct;

uint8_t Serial_RxFlag[4] = {0, 0, 0, 0};	//Serial_RxFlag[ser]
//���ڱ��״̬���Ƿ��һ����String�����꣬��������½��յ�String�ȵ�

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
		
	}//USARTxӦΪUSART3����ӦSTM32-ESP8266����USART2����ӦSTM32-PC��
	
	
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
	
	//Serial_SendByte(USART2, 'I');//��Debug��
}

void Serial_BaudRateConfig(USART_TypeDef* USARTx, uint32_t USART_BaudRate) {
	//��Ӧ��Serial��Init��ſ�ʹ�øú���
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
		
		//Serial_SendByte(USART2, 'S');//��Debug��
		
		uint16_t RxData = USART_ReceiveData(USARTx);
		//RXNE��USART_DR����ȡ���Զ�RESET����0������û���յ���
		//��һ��USART_ReceiveData()���ж�USART_DR����д�����ὫTXE�Զ���0��RESET��������δ����ȫת������λ�Ĵ���
		//USART_DR��TDR��RDR����������ϵĺϲ�
		//while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET);
		//��USART_DR��RxData��ȡ���ټ���
		
		switch (*s) {
			case 0:
//				if(RxData == '@' && Serial_RxFlag[ser] == 0) {	
//					//��㣺��©�ж�Serial_RxFlag[ser]�����ж���һ��String�Ƿ������ϡ�
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
				//����'@'��ͷ����
				
				break;
			case 1:
				if(RxData == '\r') {
					Serial_RxStringPacket[(*count)++] = RxData;
					*s = 2;
				} else {
					if (*count < size - 1) {
						//��㣺�����鴫�ݵ���������ԭʼ��ָ�����ʽ�ں����ڴ��ڵģ�sizeof(Serial_RxStringPacket)���warning��
						//��������������鴫�뺯��ʱ�ͽ�����size���뺯��
						//�� �Ѿ�װ�˵�λ��(*count+1)<����װ��λ��(sizeof(...)) ʱ�ż���װ��
						Serial_RxStringPacket[(*count)++] = RxData;
						//��㣺*count++����++���ȼ�����*����дΪ(*count)++
						}
					}
				break;
			case 2:
				if(RxData == '\n') {
					
					//Serial_SendByte(USART3, 'H');//��Debug��
					Serial_RxStringPacket[(*count)++] = RxData;
					*s = 0;
					Serial_RxStringPacket[*count] = '\0';
					*count = 0;
					Serial_RxFlag[ser] = 1;	
					//�˴���ʾһ����String�Ѿ������꣬�ɽ��ж�ȡ�����Ȳ�����
					
				} else {
					//NOTHING
				}
				break;
			default:
				//ERROR
				break;	//dafault:����û��Ҫִ�еĴ�������Ҫ��break;
		}
}

void USART2_IRQHandler() {
	
	//Serial_SendByte(USART2, 'h');//��Debug��
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET) {	//��㣺��GetITStatus��дΪGetFlagStatus
		
		//Serial_SendByte(USART2, 'H');//��Debug��
		
		//USART_ClearITPendingBit(USART2, USART_IT_RXNE);//�Զ����־λ
		
		uint16_t size = sizeof(Serial_Rx2StringPacket)/sizeof(Serial_Rx2StringPacket[0]);
		
		//����״̬������
		Serial_Auto_StateMachine(USART2, 2, &StateMachine_s2, &StateMachine_count2, Serial_Rx2StringPacket, size);
	}
}

void USART3_IRQHandler() {
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET) {	//��㣺��GetITStatus��дΪGetFlagStatus
		//��RDR��λ�Ĵ����е����ݱ�ת�Ƶ�USART_DR�Ĵ����У��ȴ���ȡ������λ��Ӳ����λ��1��SET����
		//���USART_CR1�Ĵ����е�RXNEIEΪ1��������жϡ�
		//��USART_DR�Ķ�������RxData = USART_ReceiveData(USARTx)�����Խ���λ���㣨0��RESET����
		
		//USART_ClearITPendingBit(USART3, USART_IT_RXNE);//�Զ����־λ
		
		uint16_t size = sizeof(Serial_Rx3StringPacket)/sizeof(Serial_Rx3StringPacket[0]);
		
		
		//����״̬������
		Serial_Auto_StateMachine(USART3, 3, &StateMachine_s3, &StateMachine_count3, Serial_Rx3StringPacket, size);
		
	}
}

void Serial_SendByte(USART_TypeDef* USARTx, uint8_t Byte) {
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);	
	//TXE��0�����ݻ�δ����ȫת������λ�Ĵ�����TXE��1�������ѱ���ȫת������λ�Ĵ�����
	//��һ�ֶ�USART_SendData()���ж�USART_DR����д�����ὫTXE�Զ���0��RESET��������δ����ȫת������λ�Ĵ���
	//��TDR�Ĵ����е����ݱ�Ӳ����ȫת�Ƶ���λ�Ĵ�����ʱ�򣬸�λ��Ӳ����λ��1��SET������while(...)������
	//���д������ã��ȴ���һ��Tx��USART_DRȫ��ת�Ƶ���λ�Ĵ���
	
	USART_SendData(USARTx, Byte);
}

void Serial_SendString(USART_TypeDef* USARTx, char *string) {
	for(uint8_t i = 0; string[i] != '\0'; i++) {	//��㣺��string[i]��дΪi != '\0'
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

