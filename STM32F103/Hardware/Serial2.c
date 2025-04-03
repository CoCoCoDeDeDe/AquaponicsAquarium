#include "Serial2.h"
#include "common_types.h"

/*USART先实现SendByte[OK]
USARTDMA先实现SendString
DMA优先级要高于USART优先级*/

static GPIO_InitTypeDef GPIO_IS_Tx;
static GPIO_InitTypeDef GPIO_IS_Rx;
static USART_InitTypeDef USART_IS;
static DMA_InitTypeDef DMA_IS_Tx;
static DMA_InitTypeDef DMA_IS_Rx;
static NVIC_InitTypeDef NVIC_IS_Tx;
static NVIC_InitTypeDef NVIC_IS_Rx;

/*tx_msg初始化
正在发送的string发送完成标志位:							1,
正在发送的string的首地址指针:								0,
正在发送的string的字符串长度（不包含'\0'）:		0*/
tx_msg_t tx2_msg = {1, 0, 0};

void Serial2_Init_Com(uint32_t br, FunctionalState fs)
{
	/*开启Serial2_DMA所有相关的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/*Tx和Rx共同要开启的*/
	USART_DeInit(USART2);
	USART_StructInit(&USART_IS);
	USART_IS.USART_BaudRate = br;
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_IS.USART_Parity = USART_Parity_No;
	USART_IS.USART_StopBits = USART_StopBits_1;
	USART_IS.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_IS);
	
	USART_Cmd(USART2, fs);
}

void Serial2_Init_Tx_USART(void)
{
	GPIO_StructInit(&GPIO_IS_Tx);
	GPIO_IS_Tx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_IS_Tx.GPIO_Pin  = GPIO_Pin_2;
	GPIO_IS_Tx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_IS_Tx);
}

void Serial2_Init_Tx_DMA(uint32_t dma_p,  uint8_t pp, uint8_t sp, FunctionalState fs)
{
	DMA_StructInit(&DMA_IS_Tx);
	DMA_IS_Tx.DMA_BufferSize = 0;			//初始化时关闭，SendString时设置为发送的String的字符串长度，不包括'\0'
	DMA_IS_Tx.DMA_DIR = DMA_DIR_PeripheralDST;		//串口是数据目的地
	DMA_IS_Tx.DMA_M2M = DMA_M2M_Disable;				//存储器到外设
	DMA_IS_Tx.DMA_MemoryBaseAddr = 0;		//初始化时关闭，SendString时设置为发送的String的字符串首地址
	DMA_IS_Tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc = DMA_MemoryInc_Enable;		//转移字符串数据到USART每转移1Char就进下一个Char
	DMA_IS_Tx.DMA_Mode = DMA_Mode_Normal;		//每转移1字符串后关闭，再发送时手动配置并开启
	DMA_IS_Tx.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//目的地只指向DR，转移完一个后置TXN触发下一个Char的DMA转运
	DMA_IS_Tx.DMA_Priority = dma_p;
	DMA_Init(DMA1_Channel7, &DMA_IS_Tx);
	
	NVIC_IS_Tx.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_IS_Tx.NVIC_IRQChannelCmd = fs;
	NVIC_IS_Tx.NVIC_IRQChannelPreemptionPriority = pp;
	NVIC_IS_Tx.NVIC_IRQChannelSubPriority = sp;
	NVIC_Init(&NVIC_IS_Tx);
	
	/*DMA关时，USART触发DMA的开启也无妨（未测试，推测）*/
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	
	/*DMA关时，DMA中断的开启也无妨（未测试，推测）*/
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	
	/*Tx的DMA_Cmd默认关闭，在要发送新string时再打开*/
	DMA_Cmd(DMA1_Channel7, fs);
	
	/*第一个string发送前不会有正在发送的字符串*/
	tx2_msg.tc = 1;
}

void Serial2_Init_All(void)
{
	Serial2_Init_Com(115200, ENABLE);
	Serial2_Init_Tx_USART();
	/*DMA_Cmd在上一个String发送完并且配置完要发送的String后要发送String时再开启*/
	Serial2_Init_Tx_DMA(DMA_Priority_Medium, 0, 0, DISABLE);
}

void Serial2_SendByte(char byte)
{
	/*当发送数据寄存器为空不成立时，即发送数据寄存器还有数据时，阻塞*/
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, byte);
}

int8_t Serial2_SendString(char *msg, uint32_t len)
{
	if(msg == NULL || len <=0 || len >= 1000)
	{
		return -1;	//传入参数错误
	}
	
	/*等待上一个字符串发完。
	当传输未完成时阻塞*/
	while(tx2_msg.tc == 0);
	
	/*确保关闭DMA_Cmd以配置DMA*/
	DMA_Cmd(DMA1_Channel7, DISABLE);
	
	DMA_IS_Tx.DMA_MemoryBaseAddr = (u32)msg;//【WARNING】
	DMA_IS_Tx.DMA_BufferSize = len;
	
	tx2_msg.tc = 0;
	
	/*配置完DMA后重启DMA*/
	DMA_Init(DMA1_Channel7, &DMA_IS_Tx);
	DMA_Cmd(DMA1_Channel7, ENABLE);
	
	return 1;	//成功
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7) == SET)
	{
		/*运行到此说明一个字符串已经发送完成*/
		
		/*关闭DMA_Cmd*/
		DMA_Cmd(DMA1_Channel7, DISABLE);
		
		/*置rx2传输完成标志位*/
		tx2_msg.tc = 1;
		
		DMA_ClearITPendingBit(DMA1_IT_TC7);
	}
}

