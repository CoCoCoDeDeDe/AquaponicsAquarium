#include "Serial2.h"

/*TEST*/
uint32_t	tstnum1		= 0;
uint32_t	tstnum2		= 0;
uint32_t	tstnum3		= 0;
uint32_t	tstnum4		= 0;
uint32_t	tstnum5		= 0;
uint32_t	tstnum6		= 0;

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

/*	tx_msg初始化
正在发送的string发送完成标志位:							1,
正在发送的string的首地址指针:								0,
正在发送的string的字符串长度（不包含'\0'）:		0		*/
tx_msg_t tx2_msg = {1, 0, 0};

/*rx_msg初始化
接受缓存字符串：			""
接受的字符串的长度：	0（不包括'\0'）
接受完成标志：				0（即没有可以读取的已接收消息）*/
rx_msg_t rx2_msg = {"", 0, 0};

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
	DMA_DeInit(DMA1_Channel7);
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
	NVIC_IS_Tx.NVIC_IRQChannelCmd = ENABLE;
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

/*USARTDMARx的各项配置默认开启*/
void Serial2_Init_Rx_USART(uint16_t pp, uint16_t sp)
{
	GPIO_StructInit(&GPIO_IS_Rx);
	GPIO_IS_Rx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_IS_Rx.GPIO_Pin = GPIO_Pin_3;
	GPIO_IS_Rx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_IS_Rx);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	NVIC_IS_Rx.NVIC_IRQChannel = USART2_IRQn;
	NVIC_IS_Rx.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Rx.NVIC_IRQChannelPreemptionPriority = pp;
	NVIC_IS_Rx.NVIC_IRQChannelSubPriority = sp;
	NVIC_Init(&NVIC_IS_Rx);
}

/*USARTDMARx的各项配置默认开启*/
void Serial2_Init_Rx_DMA(uint32_t dma_p)
{
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	
	DMA_DeInit(DMA1_Channel6);
	DMA_IS_Rx.DMA_BufferSize = RX2_BUF_MAX_SIZE;	//最大可接受的字符串长度（不包括'\0'）
	DMA_IS_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;		//串口外设为源头
	DMA_IS_Rx.DMA_M2M = DMA_M2M_Disable;					//串口到存储器
	DMA_IS_Rx.DMA_MemoryBaseAddr = (u32)rx2_msg.str;						//暂存接受的字符串的字符串变量的首地址
	DMA_IS_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		//1次读DR读到1Byte对应1Char
	DMA_IS_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;						//指向字符变量的指针每读取一次后自增
	/*目前Rx3单缓存，配置成Normal也无妨（未测试，推测）*/
	DMA_IS_Rx.DMA_Mode = DMA_Mode_Circular;							//1次内接受满BufferSize个字符后计数器自动重装为BufferSize
	DMA_IS_Rx.DMA_PeripheralBaseAddr = (u32)&USART2->DR;						//数据来源是USART2外设的DR数据寄存器
	DMA_IS_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//同上Memory
	DMA_IS_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//数据来源固定为DR，每当有新数据进来触发RXNE就会触发DMA转移1个Byte到字符串变量
	DMA_IS_Rx.DMA_Priority = dma_p;					//DMA通道优先级【para】
	DMA_Init(DMA1_Channel6, &DMA_IS_Rx);
	
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	/*初始化时已经置0，此处是加一层保障*/
	rx2_msg.rc = 0;		//没有新接受的数据可供读取或使用
}

void Serial2_Init_All(void)
{
	Serial2_Init_Com(115200, ENABLE);
	Serial2_Init_Tx_USART();
	/*DMA_Cmd在上一个String发送完并且配置完要发送的String后要发送String时再开启*/
	Serial2_Init_Tx_DMA(DMA_Priority_Medium, 1, 1, DISABLE);
	Serial2_Init_Rx_USART(2, 2);
	Serial2_Init_Rx_DMA(DMA_Priority_Medium);
}

void Serial2_SendByte(char byte)
{
	/*当发送数据寄存器为空不成立时，即发送数据寄存器还有数据时，阻塞*/
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, byte);
}

int8_t Serial2_SendString(char *msg, uint32_t len)
{
	if(msg == NULL || len <=0 || len >= TX2_MSG_LEN_MAX)
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
	
	/*配置完DMA后重启DMA*/
	DMA_Init(DMA1_Channel7, &DMA_IS_Tx);
	DMA_Cmd(DMA1_Channel7, ENABLE);
	
	tx2_msg.tc = 0;
	
	return 1;	//成功
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7) != RESET)
	{
		tstnum2++;	//	【Debug】
		
		/*运行到此说明一个字符串已经发送完成*/
		
		/*关闭DMA_Cmd*/
		DMA_Cmd(DMA1_Channel7, DISABLE);
		
		/*置rx2传输完成标志位*/
		tx2_msg.tc = 1;
		
		DMA_ClearITPendingBit(DMA1_IT_TC7);
	}
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		/*IT_IDLE的标志位必须要读取2个寄存器后才能清零*/
		/*__IO是volatile的宏定义，
		告诉编译器该变量可能随时变化，不要对其访问进行优化*/
		__IO uint32_t tmp = USART2->SR;
		tmp = USART2->DR;
		
		/*运行到此说明已经有一个字符串被接收完，
		并且RDR空闲了一小段时间后触发IT_IDLE，也就是瞒住了IDLE中断条件。
		此时可能DMA快速运行导致重复读取了最后一个字符到rx2_msg.str末尾，
		但暂时不考虑该风险。【可优化】
		先立即将DMA关闭，防止上述风险事件发送或继续。
		对数据的处理：
		现要计算读取到的字符的长度，
		然后在读取到的字符后面加个'\0'，
		本案例直接在rx2_msg.str中加'\0'，
		不过这样会导致可以读取的字符串最大长度-1。【可优化】
		数据处理完成。
		在处理完数据时时置标志位声明已经有一个字符串被接收完。
		将DMA的配置重置。
		开启DMA。
		此处有风险，可能DMA开启后很快便开始接受新消息导致rx2_msg消息被覆盖。
		【可优化】
		plan1：开启DMA前将缓存字符存到消息队列中，
			转存事件vs新消息间隔时间
			消息处理事件vs转存完到新消息读取完的事件*/
		
		DMA_Cmd(DMA1_Channel6, DISABLE);		//关闭DMA
		
		/*RX2_BUF_MAX_SIZE就是DataCounter的初始值，
		DataCounter少了多少就是读取了多少个Byte*/
		rx2_msg.len = RX2_BUF_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		/*在字符末尾加'\0'*/
		rx2_msg.str[rx2_msg.len] = '\0';
		
		/*字符末尾加'\0'后，长度+1*/
		rx2_msg.len++;
		
		/*接受完就可以开始读取、利用*/
		rx2_msg.rc = 1;
		
		/*DataCounter重置为RX2_BUF_MAX_SIZE*/
		DMA_SetCurrDataCounter(DMA1_Channel6, RX2_BUF_MAX_SIZE);
		
		/*重启DMA*/
		DMA_Cmd(DMA1_Channel6, ENABLE);
		
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
	}
}


