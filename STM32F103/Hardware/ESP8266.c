#include "ESP8266.h"

//TODO:USART3DMASendString
//TODO:USART3DMAReceiveString

#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

char tx3_buf[TX3_BUF_MAX_SIZE] = {0};
char rx3_buf[RX3_BUF_MAX_SIZE] = {0};

int8_t tx3_tc_flag = 0;
uint16_t tc_count = 0;
uint16_t tst_num1 = 0;

GPIO_InitTypeDef GPIO_IS_Tx;
GPIO_InitTypeDef GPIO_IS_Rx;
USART_InitTypeDef USART_IS;
DMA_InitTypeDef DMA_IS_Tx;
DMA_InitTypeDef DMA_IS_Rx;
NVIC_InitTypeDef NVIC_IS_Tx;
NVIC_InitTypeDef NVIC_IS_Rx;

void Serial3_Init_Com(uint32_t br, FunctionalState fs)
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
	
	USART_Cmd(USART3, fs);
}

void Serial3_USART3_Cmd(FunctionalState fs)
{
	USART_Cmd(USART3, fs);
}

void Serial3_Init_Tx_USART(FunctionalState fs)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_IS_Tx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_IS_Tx.GPIO_Pin = GPIO_Pin_10;
	GPIO_IS_Tx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_IS_Tx);
	
	//DMAITCFG,USARTITCFG,NVICINIT
}

void Serial3_Init_Tx_DMA(uint8_t nvic_pp, uint8_t nvic_sp, FunctionalState fs) {
	
	NVIC_IS_Tx.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_IS_Tx.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Tx.NVIC_IRQChannelPreemptionPriority = nvic_pp;
	NVIC_IS_Tx.NVIC_IRQChannelSubPriority = nvic_sp;
	NVIC_Init(&NVIC_IS_Tx);
	
	DMA_DeInit(DMA1_Channel2);
	DMA_IS_Tx.DMA_BufferSize			= 0;//0代表关闭DMA转移
	DMA_IS_Tx.DMA_DIR					= DMA_DIR_PeripheralDST;//外设为目的地
	DMA_IS_Tx.DMA_M2M					= DMA_M2M_Disable;//非内存到内存
	DMA_IS_Tx.DMA_MemoryBaseAddr		= (u32)tx3_buf;
	DMA_IS_Tx.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_IS_Tx.DMA_Mode					= DMA_Mode_Normal;
	DMA_IS_Tx.DMA_PeripheralBaseAddr	= (u32)USART3->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//【WARNING】每发1Byte和DMA的内存指向地址自增1单位同步
	DMA_IS_Tx.DMA_Priority				= DMA_Priority_High;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, fs);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, fs);//TC:一次DMA Buffer循环结束，既计数归零时中断
	
	DMA_Cmd(DMA1_Channel2, fs);
	
	tx3_tc_flag = 1;//初始化时确定没有未发送的字符串
}

void Serial3_Tx_Cmd(FunctionalState fs)
{
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, fs);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, fs);//TC:一次DMA Buffer循环结束，既计数归零时中断
	
	DMA_Cmd(DMA1_Channel2, fs);
}

void Serial3_Init_All(void)
{
	Serial3_Init_Com(115200, ENABLE);
	Serial3_Init_Tx_USART(ENABLE);
	Serial3_Init_Tx_DMA(1, 1, DISABLE);
}

/*note:每当要发新的string时，DMA_MemoryBaseAddr要指向string的首地址，
DMA_BufferSize应设置为string的有效字节数（不包括'\0'）。
配置DMA参数时，DMA要先关闭，配置完并确定要发送新字符串时再打开DMA。
本项目将DMA的关闭操作在TC中断中完成，
DMA开启在配置完新的要发送的数组时用于启动发送。*/

/*note:每当一个字符串发送完时，DMA的TC会触发终端，
在该终端函数中可用于置TC标志位，
用于每个发送字符串的函数中检测上一个字符串是否已经发送完成，
既新字符串是否可以发送。*/

/*Tx DMA 启动要配置的：
	tx3_tc_flag = 1;*/

int8_t Serial3_SendByte(char b)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, b);
}

int8_t Serial3_SendString(char *str, uint16_t str_len)//str_len不包括'\0'
{
	if(str_len == 0 || str == NULL || str_len >= 1000)
	{	
		return -1;	//传入参数有误
	}
	
	while(tx3_tc_flag == 0);//不为0时向下执行。
	
	Serial3_Tx_Cmd(DISABLE);//确保关闭DMA
	
	DMA_IS_Tx.DMA_MemoryBaseAddr	= (u32)&str[0];
	DMA_IS_Tx.DMA_BufferSize 		= str_len;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	Serial3_Tx_Cmd(ENABLE);//启动DMA
	Serial3_USART3_Cmd(ENABLE);//防止USART3被误关
	
	tx3_tc_flag = 1;//告诉其他函数有在传输的字符串
	
	tst_num1 ++;
	
	return 1;	//成功开启
}

void Serial3_Init_Rx()
{
	
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
}





