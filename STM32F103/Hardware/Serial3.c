#include "Serial3.h"

//TODO:USART3DMASendString
//TODO:USART3DMAReceiveString

#define TX3_BUF_MAX_SIZE 512
#define RX3_BUF_MAX_SIZE 512

int8_t rx3_buf_idx = 0;
int8_t rx3_idle_flag = 0;

char tx3_buf[TX3_BUF_MAX_SIZE] = {0};
char rx3_buf[RX3_BUF_MAX_SIZE] = {0};
char rx3_buf_0[RX3_BUF_MAX_SIZE] = {0};
char rx3_buf_1[RX3_BUF_MAX_SIZE] = {0};
char *rx3_buf_arr[2] = {rx3_buf_0, rx3_buf_1};

int8_t tx3_tc_flag = 0;
uint16_t tc_count = 0;
uint16_t tst_num1 = 0;

GPIO_InitTypeDef GPIO_IS_Tx;
GPIO_InitTypeDef GPIO_IS_Rx;
USART_InitTypeDef USART_IS;
DMA_InitTypeDef DMA_IS_Tx;
DMA_InitTypeDef DMA_IS_Rx;
NVIC_InitTypeDef NVIC_IS_Tx_DMA;
NVIC_InitTypeDef NVIC_IS_Rx_USART;
NVIC_InitTypeDef NVIC_IS_Rx_DMA;

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

void Serial3_Init_Tx_DMA(uint8_t nvic_pp, uint8_t nvic_sp, FunctionalState fs)
{
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	
	DMA_DeInit(DMA1_Channel2);
	DMA_IS_Tx.DMA_BufferSize			= 0;//0����ر�DMAת��
	DMA_IS_Tx.DMA_DIR					= DMA_DIR_PeripheralDST;//����ΪĿ�ĵ�
	DMA_IS_Tx.DMA_M2M					= DMA_M2M_Disable;//���ڴ浽�ڴ�
	DMA_IS_Tx.DMA_MemoryBaseAddr		= (u32)tx3_buf;
	DMA_IS_Tx.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_IS_Tx.DMA_Mode					= DMA_Mode_Normal;
	DMA_IS_Tx.DMA_PeripheralBaseAddr	= (u32)&USART3->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//��WARNING��ÿ��1Byte��DMA���ڴ�ָ���ַ����1��λͬ��
	DMA_IS_Tx.DMA_Priority				= DMA_Priority_High;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	//Upper:TC:һ��DMA Bufferѭ���������ȼ�������ʱ�ж�
	
	NVIC_IS_Tx_DMA.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelPreemptionPriority = nvic_pp;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelSubPriority = nvic_sp;
	NVIC_Init(&NVIC_IS_Tx_DMA);
	
	DMA_Cmd(DMA1_Channel2, fs);
	
	tx3_tc_flag = 1;//��ʼ��ʱȷ��û��δ���͵��ַ���
}

void Serial3_Tx_Cmd(FunctionalState fs)
{
	//USART_DMACmd(USART3, USART_DMAReq_Tx, fs);
	
	DMA_Cmd(DMA1_Channel2, fs);
}

void Serial3_Init_Rx_USART(uint8_t nvic_pp, uint8_t nvic_sp, FunctionalState fs)
{
	GPIO_IS_Rx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_IS_Rx.GPIO_Pin = GPIO_Pin_11;
	GPIO_IS_Rx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_IS_Rx);
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	NVIC_IS_Rx_USART.NVIC_IRQChannel = USART3_IRQn;
	NVIC_IS_Rx_USART.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Rx_USART.NVIC_IRQChannelPreemptionPriority = nvic_pp;
	NVIC_IS_Rx_USART.NVIC_IRQChannelSubPriority = nvic_sp;
	NVIC_Init(&NVIC_IS_Rx_USART);
}

void Serial3_Init_Rx_DMA(FunctionalState fs)
{
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	DMA_DeInit(DMA1_Channel3);
	DMA_IS_Rx.DMA_BufferSize = RX3_BUF_MAX_SIZE;//һ��ѭ������ת���ַ���
	DMA_IS_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;//DRת�Ƶ��洢��
	DMA_IS_Rx.DMA_M2M = DMA_M2M_Disable;//���赽�洢��
	DMA_IS_Rx.DMA_MemoryBaseAddr = (u32)rx3_buf;//DRת�Ƶ�rx3_buf����rx3_buf[0]��ʼ
	DMA_IS_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//1Char~1Byte
	DMA_IS_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;//rx3_buf������ַ����
	DMA_IS_Rx.DMA_Mode = DMA_Mode_Circular;//������������Զ���װΪBufferSize
	DMA_IS_Rx.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//USART3->DRת�Ƶ�rx3_buf[x]
	DMA_IS_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//1Char~1Byte
	DMA_IS_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�½��յ�Byte��Զ��USART3->DR
	DMA_IS_Rx.DMA_Priority = DMA_Priority_VeryHigh;//ʹRx3���ȼ���DMA�����
	DMA_Init(DMA1_Channel3, &DMA_IS_Rx);//���ø�USART3_Rx��Ӧ��DMA1_Channel3
	
	DMA_Cmd(DMA1_Channel3, fs);
}

void Serial3_Init_All(void)
{
	Serial3_Init_Com(115200, ENABLE);
	Serial3_Init_Tx_USART(ENABLE);
	Serial3_Init_Tx_DMA(1, 1, DISABLE);//�ոճ�ʼ����û��Ҫת�Ƶ�����
	Serial3_Init_Rx_USART(0, 0, ENABLE);
	Serial3_Init_Rx_DMA(ENABLE);//���ճ���
}

/*note:ÿ��Ҫ���µ�stringʱ��DMA_MemoryBaseAddrҪָ��string���׵�ַ��
DMA_BufferSizeӦ����Ϊstring����Ч�ֽ�����������'\0'����
����DMA����ʱ��DMAҪ�ȹرգ������겢ȷ��Ҫ�������ַ���ʱ�ٴ�DMA��
����Ŀ��DMA�Ĺرղ�����TC�ж�����ɣ�
DMA�������������µ�Ҫ���͵�����ʱ�����������͡�
*/

/*note:ÿ��һ���ַ���������ʱ��DMA��TC�ᴥ���նˣ�
�ڸ��ն˺����п�������TC��־λ��
����ÿ�������ַ����ĺ����м����һ���ַ����Ƿ��Ѿ�������ɣ�
�����ַ����Ƿ���Է��͡�
*/

/*Tx DMA ����Ҫ���õģ�
	tx3_tc_flag = 1;*/

int8_t Serial3_SendByte(char b)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, b);
	
	return 1;	//SUCCESS
}

int8_t Serial3_SendString(char *str, uint16_t str_len)//str_len������'\0'
{
	if(str_len == 0 || str == NULL || str_len >= 1000)
	{	
		return -1;	//�����������
	}
	
	while(tx3_tc_flag == 0);
	
	/*ȷ���ر�DMA*/
	Serial3_Tx_Cmd(DISABLE);
	
	/*��������DMA*/
	DMA_IS_Tx.DMA_MemoryBaseAddr	= (u32)str;
	DMA_IS_Tx.DMA_BufferSize 		= str_len;
	
	/*����DMA*/
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	Serial3_Tx_Cmd(ENABLE);//����DMA
	Serial3_USART3_Cmd(ENABLE);//��ֹUSART3�����
	
	//tx3_tc_flag = 0;//���������������ڴ�����ַ���
	
	return 1;	//�ɹ�����
}

void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET)
	{
		Serial3_Tx_Cmd(DISABLE);
		
		tx3_tc_flag = 1;
		
		DMA_ClearITPendingBit(DMA1_IT_TC2);
	}
}


void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
	{
		__IO uint32_t tmp = USART3->SR;
		tmp = USART3->DR;
		
		/*note:�����������ˣ�����1���ַ��������꣬
		���Ҷ�ʱ����û���µ��ַ������µ��ַ����Ѿ����ܵ���rx3_buf��
		���Ƚ��ܵ��ַ���ת�Ƶ�rx3_buf[0]�����idx���ε�����
		���USART_IT_IDLE�����������ܿ��ܻ��ظ��������һ���ַ����Σ�
		����rx3_buf���ַ�����ʵ�ʽ��ܵ��ַ�������λ������ĩβ��λ�ַ���ͬ��
		*/
		/*note:���е��˴���ҪֹͣDMA����rx3_buf��д�����ݣ�
		����ת�Ƶ�rx3_buf���ַ���,
		��rx3_buf���ܵ����1λ���ݺ���д��'\0'�ַ����������ţ�
		������Buffer������������ɺ���ENABLE Rx3 DMA��
		��rx3_buf��������������������rx3_buf��д�����ݡ�
		
		����˫rx3_buf�ķ���ʱ���ڴ�Ҫ��DISABLE Rx3 DMA��
		��DMAĿ�ĵ�ַ��rx3_buf_1��rx3_buf_0֮�������䡣
		
		���⣬��Ҫ��Ҫ����rx3_buf���ݵĳ��򾡿�����rx3_buf�����ݡ�
		����ִ��ʱ��̵ĺ��������ڸ��ж��������ꡣ
		����ִ��ʱ�䳤�ĺ��������ڴ��ñ�־λ��������������Ӧ��־λ��
		����ִ��ʱ��̫���ĳ�����߽�������̫Ƶ�����������ݳ���
		�������ݼ��Ƶ�������ݣ���Ҫ����ʵ�ֽ�rx3_buf�����ݼ�ʱ�洢�����ݶ����У�
		ͨ�����ݶ�����غ����������������������ݿ��Զ�ȡ��
		���ҹ������ݶ��еĶ�ȡ��д�롣
		����rx3_buf���ݵ����ݶ��л��߻�����Գ�Ҳ��DMA��*/
		
		/*��Debug��*/
		//Serial3_SendString("InITIDLE\r\n", strlen("InITIDLE\r\n"));
		
		/*�ر�DMA��1��Ϊ���������ã�2��Ϊ�˷�ֹ���ݸ���*/
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		uint16_t read_len = RX3_BUF_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		
		rx3_buf[read_len] = '\0';
		
		rx3_idle_flag = 1;
		//note:���޻���ֱ������rx3_buf�����ᵼ�½���Ƶ������ֻ�ܶ������һ��

//		rx3_buf_arr[rx3_buf_idx][read_len] = '\0';
//		
//		/*����rx3_buf*/
//		rx3_idle_flag = 1;
//		switch(rx3_buf_idx)//��ʱidx��δ�ı�
//		{
//			case 0:
//				Serial3_SendString(rx3_buf_0, sizeof(rx3_buf_0));
//				break;
//			case 1:
//				Serial3_SendString(rx3_buf_1, sizeof(rx3_buf_1));
//				break;
//		}
//		
//		/*���ü�������������DMA*/
//		DMA_SetCurrDataCounter(DMA1_Channel3, RX3_BUF_MAX_SIZE);
//		rx3_buf_idx = !rx3_buf_idx;//0��1����������Ŀ�����顣�����ڴ˺�����idxҪȡ��
//		switch(rx3_buf_idx)
//		{
//			case 0:
//				DMA_IS_Rx.DMA_MemoryBaseAddr = (u32)&rx3_buf_0;
//				break;
//			case 1:
//				DMA_IS_Rx.DMA_MemoryBaseAddr = (u32)&rx3_buf_1;
//				break;
//		}
		
		/*����DMA�����Կ�ʼ������Rx����*/
		DMA_SetCurrDataCounter(DMA1_Channel3, RX3_BUF_MAX_SIZE);
		DMA_Init(DMA1_Channel3, &DMA_IS_Rx);
		DMA_Cmd(DMA1_Channel3, ENABLE);
		Serial3_USART3_Cmd(ENABLE);
		
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
}




