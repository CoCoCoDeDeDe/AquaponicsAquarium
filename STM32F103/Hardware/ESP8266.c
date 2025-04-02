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
	DMA_IS_Tx.DMA_BufferSize			= 0;//0����ر�DMAת��
	DMA_IS_Tx.DMA_DIR					= DMA_DIR_PeripheralDST;//����ΪĿ�ĵ�
	DMA_IS_Tx.DMA_M2M					= DMA_M2M_Disable;//���ڴ浽�ڴ�
	DMA_IS_Tx.DMA_MemoryBaseAddr		= (u32)tx3_buf;
	DMA_IS_Tx.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_IS_Tx.DMA_Mode					= DMA_Mode_Normal;
	DMA_IS_Tx.DMA_PeripheralBaseAddr	= (u32)USART3->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//��WARNING��ÿ��1Byte��DMA���ڴ�ָ���ַ����1��λͬ��
	DMA_IS_Tx.DMA_Priority				= DMA_Priority_High;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, fs);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, fs);//TC:һ��DMA Bufferѭ���������ȼ�������ʱ�ж�
	
	DMA_Cmd(DMA1_Channel2, fs);
	
	tx3_tc_flag = 1;//��ʼ��ʱȷ��û��δ���͵��ַ���
}

void Serial3_Tx_Cmd(FunctionalState fs)
{
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, fs);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, fs);//TC:һ��DMA Bufferѭ���������ȼ�������ʱ�ж�
	
	DMA_Cmd(DMA1_Channel2, fs);
}

void Serial3_Init_All(void)
{
	Serial3_Init_Com(115200, ENABLE);
	Serial3_Init_Tx_USART(ENABLE);
	Serial3_Init_Tx_DMA(1, 1, DISABLE);
}

/*note:ÿ��Ҫ���µ�stringʱ��DMA_MemoryBaseAddrҪָ��string���׵�ַ��
DMA_BufferSizeӦ����Ϊstring����Ч�ֽ�����������'\0'����
����DMA����ʱ��DMAҪ�ȹرգ������겢ȷ��Ҫ�������ַ���ʱ�ٴ�DMA��
����Ŀ��DMA�Ĺرղ�����TC�ж�����ɣ�
DMA�������������µ�Ҫ���͵�����ʱ�����������͡�*/

/*note:ÿ��һ���ַ���������ʱ��DMA��TC�ᴥ���նˣ�
�ڸ��ն˺����п�������TC��־λ��
����ÿ�������ַ����ĺ����м����һ���ַ����Ƿ��Ѿ�������ɣ�
�����ַ����Ƿ���Է��͡�*/

/*Tx DMA ����Ҫ���õģ�
	tx3_tc_flag = 1;*/

int8_t Serial3_SendByte(char b)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, b);
}

int8_t Serial3_SendString(char *str, uint16_t str_len)//str_len������'\0'
{
	if(str_len == 0 || str == NULL || str_len >= 1000)
	{	
		return -1;	//�����������
	}
	
	while(tx3_tc_flag == 0);//��Ϊ0ʱ����ִ�С�
	
	Serial3_Tx_Cmd(DISABLE);//ȷ���ر�DMA
	
	DMA_IS_Tx.DMA_MemoryBaseAddr	= (u32)&str[0];
	DMA_IS_Tx.DMA_BufferSize 		= str_len;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	Serial3_Tx_Cmd(ENABLE);//����DMA
	Serial3_USART3_Cmd(ENABLE);//��ֹUSART3�����
	
	tx3_tc_flag = 1;//���������������ڴ�����ַ���
	
	tst_num1 ++;
	
	return 1;	//�ɹ�����
}

void Serial3_Init_Rx()
{
	
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
}





