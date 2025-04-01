#include "ESP8266.h"

//TODO:USART3DMASendString
//TODO:USART3DMAReceiveString

#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

char tx3_buf[TX3_BUF_MAX_SIZE] = {0};
char rx3_buf[RX3_BUF_MAX_SIZE] = {0};

GPIO_InitTypeDef GPIO_IS_Tx;
GPIO_InitTypeDef GPIO_IS_Rx;
USART_InitTypeDef USART_IS;
DMA_InitTypeDef DMA_IS_Tx;
DMA_InitTypeDef DMA_IS_Rx;
NVIC_InitTypeDef NVIC_IS_Tx;
NVIC_InitTypeDef NVIC_IS_Rx;

void Serial_Init_Com(uint32_t br, FunctionalState ns)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	USART_IS.USART_BaudRate = br;
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_IS.USART_Parity = USART_Parity_No;
	USART_IS.USART_StopBits = USART_StopBits_1;
	USART_IS.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_IS);
	
	USART_Cmd(USART3, ns);
}

void Serial_Init_Tx(uint8_t nvic_pp, uint8_t nvic_sp, FunctionalState ns)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_IS_Tx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_IS_Tx.GPIO_Pin = GPIO_Pin_10;
	GPIO_IS_Tx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_IS_Tx);
	
	NVIC_IS_Tx.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_IS_Tx.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Tx.NVIC_IRQChannelPreemptionPriority = nvic_pp;
	NVIC_IS_Tx.NVIC_IRQChannelSubPriority = nvic_sp;
	NVIC_Init(&NVIC_IS_Tx);
	
	DMA_IS_Tx.DMA_BufferSize			= 0;//0代表关闭DMA转移
	DMA_IS_Tx.DMA_DIR					= DMA_DIR_PeripheralDST;//外设为目的地
	DMA_IS_Tx.DMA_M2M					= DMA_M2M_Disable;//非内存到内存
	DMA_IS_Tx.DMA_MemoryBaseAddr		= (u32)tx3_buf;
	DMA_IS_Tx.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_IS_Tx.DMA_Mode					= DMA_Mode_Normal;
	DMA_IS_Tx.DMA_PeripheralBaseAddr	= (u32)USART3->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//每发1Byte和DMA的内存指向地址自增同步
	DMA_IS_Tx.DMA_Priority				= DMA_Priority_High;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, ns);
	
	DMA_Cmd(DMA1_Channel2, ns);
	
	//DMAITCFG,USARTITCFG,NVICINIT
}

void Serial_Init_Rx()
{
	
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
}


