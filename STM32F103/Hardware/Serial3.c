#include "Serial3.h"

#include "stm32f10x.h"                  // Device header

#include <stdlib.h>						// atoi()

#include <stdio.h>

#include "AT.h"

#include <string.h>

#include "Array.h"

#include "math.h"

#include "MyTIM.h"

#include "common_types.h"

#include "OLED.h"

#include "MyTIM.h"

#include "MyFeeder.h"
#include "MyWaterP.h"

#include "MyWaterSS.h"
#include "MyAirS.h"
#include "MyWaterTS.h"

#include "MyWaterQS.h"
#include "MySoilMS.h"
#include "MyIlluminationS.h"

#include "MyADCAndDMA.h"

#include "MyPlantGL.h"
#include "MyAirP.h"
#include "MyWaterH.h"

#include "Serial2.h"

#include "MyAquariumL.h"

//TODO:USART3DMASendString
//TODO:USART3DMAReceiveString

/** 
  * @brief  创建并初始化Serial3消息结构体
  */
rx3_msg_t rx3_msg = {
	0,
	"",
	MSG_Default,
	MSG_Default
};

int8_t tx3_tc_flag = 0;

static GPIO_InitTypeDef GPIO_IS_Tx;
static GPIO_InitTypeDef GPIO_IS_Rx;
static USART_InitTypeDef USART_IS;
static DMA_InitTypeDef DMA_IS_Tx;
static DMA_InitTypeDef DMA_IS_Rx;
static NVIC_InitTypeDef NVIC_IS_Tx_DMA;
static NVIC_InitTypeDef NVIC_IS_Rx_USART;
/*目前Serial3_Rx使用USART_IDLE中断检测一个字符串发送完成*/
//static NVIC_InitTypeDef NVIC_IS_Rx_DMA;

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

void Serial3_Init_Tx_DMA(uint32_t dma_p, uint8_t nvic_pp, uint8_t nvic_sp, FunctionalState fs)
{
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	
	DMA_DeInit(DMA1_Channel2);
	DMA_IS_Tx.DMA_BufferSize			= 0;//0代表关闭DMA转移
	DMA_IS_Tx.DMA_DIR					= DMA_DIR_PeripheralDST;//外设为目的地
	DMA_IS_Tx.DMA_M2M					= DMA_M2M_Disable;//非内存到内存
	DMA_IS_Tx.DMA_MemoryBaseAddr		= (u32)0;
	DMA_IS_Tx.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_IS_Tx.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_IS_Tx.DMA_Mode					= DMA_Mode_Normal;
	DMA_IS_Tx.DMA_PeripheralBaseAddr	= (u32)&USART3->DR;
	DMA_IS_Tx.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_IS_Tx.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//【WARNING】每发1Byte和DMA的内存指向地址自增1单位同步
	DMA_IS_Tx.DMA_Priority				= dma_p;
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	//Upper:TC:一次DMA Buffer循环结束，既计数归零时中断
	
	NVIC_IS_Tx_DMA.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelCmd = ENABLE;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelPreemptionPriority = nvic_pp;
	NVIC_IS_Tx_DMA.NVIC_IRQChannelSubPriority = nvic_sp;
	NVIC_Init(&NVIC_IS_Tx_DMA);
	
	DMA_Cmd(DMA1_Channel2, fs);
	
	tx3_tc_flag = 1;//初始化时确定没有未发送的字符串
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

void Serial3_Init_Rx_DMA(uint32_t dma_p,FunctionalState fs)
{
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	DMA_DeInit(DMA1_Channel3);
	DMA_IS_Rx.DMA_BufferSize = RX3_BUF_MAX_SIZE;//一个循环最大可转移字符数
	DMA_IS_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;//DR转移到存储器
	DMA_IS_Rx.DMA_M2M = DMA_M2M_Disable;//外设到存储器
	DMA_IS_Rx.DMA_MemoryBaseAddr = (u32)rx3_msg.buf;//DR转移到rx3_buf，从rx3_buf[0]开始
	DMA_IS_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//1Char~1Byte
	DMA_IS_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;//rx3_buf索引地址递增
	DMA_IS_Rx.DMA_Mode = DMA_Mode_Circular;//计数器归零后自动重装为BufferSize
	DMA_IS_Rx.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//USART3->DR转移到rx3_buf[x]
	DMA_IS_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//1Char~1Byte
	DMA_IS_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//新接收的Byte永远在USART3->DR
	DMA_IS_Rx.DMA_Priority = dma_p;//使Rx3优先级在DMA中最高
	DMA_Init(DMA1_Channel3, &DMA_IS_Rx);//配置给USART3_Rx对应的DMA1_Channel3
	
	DMA_Cmd(DMA1_Channel3, fs);
}

void Serial3_Init_All(void)
{
	Serial3_Init_Com(115200, ENABLE);
	Serial3_Init_Tx_USART(ENABLE);
	
	/*note:保证DMA中断优先级高于USART的优先级，
	否则当在USART中断中用USARTDMA发送连续发送一个以上字符串就会卡死*/
	Serial3_Init_Tx_DMA(DMA_Priority_High, 0, 0, DISABLE);//刚刚初始化，没有要转移的数据
	Serial3_Init_Rx_USART(1, 1, ENABLE);
	Serial3_Init_Rx_DMA(DMA_Priority_High, ENABLE);//接收常开
}

/*note:每当要发新的string时，DMA_MemoryBaseAddr要指向string的首地址，
DMA_BufferSize应设置为string的有效字节数（不包括'\0'）。
配置DMA参数时，DMA要先关闭，配置完并确定要发送新字符串时再打开DMA。
本项目将DMA的关闭操作在TC中断中完成，
DMA开启在配置完新的要发送的数组时用于启动发送。
*/

/*note:每当一个字符串发送完时，DMA的TC会触发终端，
在该终端函数中可用于置TC标志位，
用于每个发送字符串的函数中检测上一个字符串是否已经发送完成，
既新字符串是否可以发送。
*/

/*Tx DMA 启动要配置的：
	tx3_tc_flag = 1;*/

int8_t Serial3_SendByte(char b)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, b);
	
	return 1;	//SUCCESS
}

int8_t Serial3_SendString(char *str, uint16_t str_len)//str_len不包括'\0'
{
	
	if(str_len <= 0 || str == NULL || str_len >= 1000)
	{	
		return -1;	//传入参数有误
	}
	
	while(tx3_tc_flag == 0);
	
	/*确保关闭DMA*/
	Serial3_Tx_Cmd(DISABLE);
	
	/*重新配置DMA*/
	DMA_IS_Tx.DMA_MemoryBaseAddr	= (u32)str;
	DMA_IS_Tx.DMA_BufferSize 		= str_len;
	
	/*重启DMA*/
	/*启用DMA_IS_Tx的配置：MemoryBaseAddr,BufferSize*/
	DMA_Init(DMA1_Channel2, &DMA_IS_Tx);
	Serial3_Tx_Cmd(ENABLE);//启动DMA
	Serial3_USART3_Cmd(ENABLE);//防止USART3被误关
	
	tx3_tc_flag = 0;//告诉其他函数有在传输的字符串
	
	return 1;	//成功开启
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
		
		/*note:程序运行至此，代表1个字符串接受完，
		并且段时间内没有新的字符串，新的字符串已经接受到了rx3_buf，
		最先接受的字符串转移到rx3_buf[0]，随后idx依次递增。
		如果USART_IT_IDLE触发的慢，很可能会重复接受最后一个字符数次，
		导致rx3_buf的字符串比实际接受的字符串长几位，而且末尾几位字符相同。
		*/
		/*note:运行到此处，要停止DMA再在rx3_buf上写入数据，
		计算转移到rx3_buf的字符数,
		在rx3_buf接受的最后1位数据后面写入'\0'字符串结束符号，
		并重置Buffer计数。配置完成后再ENABLE Rx3 DMA。
		当rx3_buf的数据利用完后才运行在rx3_buf上写入数据。
		
		采用双rx3_buf的方案时，在此要先DISABLE Rx3 DMA，
		将DMA目的地址在rx3_buf_1和rx3_buf_0之间轮流变。
		
		此外，还要让要利用rx3_buf数据的程序尽快利用rx3_buf的数据。
		对于执行时间短的函数可以在该中断中运行完。
		对于执行时间长的函数可以在此置标志位，等其他程序响应标志位。
		对于执行时间太长的程序或者接受数据太频繁、接受数据长、
		接受数据间隔频繁的数据，还要另外实现将rx3_buf的数据及时存储到数据队列中，
		通过数据队列相关函数提醒其他程序有新数据可以读取，
		并且管理数据队列的读取与写入。
		复制rx3_buf数据到数据队列或者缓存可以尝也用DMA。*/
		
		/*【Debug】*/
		//Serial3_SendString("InITIDLE\r\n", strlen("InITIDLE\r\n"));
		
		/*关闭DMA，1是为了重新配置，2是为了防止数据覆盖*/
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		uint16_t read_len = RX3_BUF_MAX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		read_len++;
		
		if( read_len - 1 < RX3_BUF_MAX_SIZE)
		{
			/*在rx3_buf的第read_len个char写入'\0'后，
			rx3_buf便成了一个长为read_len的字符串（包含'\0'），
			其终止符索引为read_len-1，位数为read_len*/
			rx3_msg.buf[read_len - 1] = '\0';
		}
		
		/*一个标志位只能给一个使用方使用，一个使用方使用完清零可能导致其他使用方无法检测*/
		rx3_msg.rc_1 = 1;
		
		//note:简单无缓存直接利用rx3_buf方案会导致接收频繁数据只能读到最后一串
		
//		rx3_msg.type = AT_ParseMessage(rx3_msg.buf, read_len, msg_keywords);	//type供下面switch使用
//		rx3_msg.type_2 = rx3_msg.type;		//type_2供AT_SM()使用
//		
//		//【WARNING小心AT_SM中断嵌套将type清零为Default】
//		switch(rx3_msg.type)
//			{
//				case MSG_NONE:
//					
////					Serial3_SendString("NONE\r\n", strlen("NONE\r\n"));	//【Debug】
//					Serial2_SendString("NONE\r\n", strlen("NONE\r\n"));	//【Debug】
//					break;
//				case MSG_POWERON:
//					
////					Serial3_SendString("POWERON\r\n", strlen("POWERON\r\n"));	//【Debug】
//					Serial2_SendString("POWERON\r\n", strlen("POWERON\r\n"));	//【Debug】
//					break;
//				case MSG_OK:
//					
////					Serial3_SendString("OK\r\n", strlen("OK\r\n"));	//【Debug】
//					Serial2_SendString("OK\r\n", strlen("OK\r\n"));	//【Debug】
//					break;
//				case MSG_ERROR:
//					
////					Serial3_SendString("ERROR\r\n", strlen("ERROR\r\n"));	//【Debug】
//					Serial2_SendString("ERROR\r\n", strlen("ERROR\r\n"));	//【Debug】
//					break;
//				case MSG_WIFI_CONN:
//					wifi.isConnect = CONNECT;
////					Serial3_SendString("WIFI_CONN\r\n", strlen("WIFI_CONN\r\n"));	//【Debug】
//					Serial2_SendString("WIFI_CONN\r\n", strlen("WIFI_CONN\r\n"));	//【Debug】
//					break;
//				case MSG_WIFI_GOTIP:
//					wifi.isConnect = CONNECT;
////					Serial3_SendString("WIFI_GOTIP\r\n", strlen("WIFI_GOTIP\r\n"));	//【Debug】
//					Serial2_SendString("WIFI_GOTIP\r\n", strlen("WIFI_GOTIP\r\n"));	//【Debug】
//					break;
//				case MSG_WIFI_DISCONN:
//					wifi.isConnect = DISCONNECT;
////					Serial3_SendString("WIFI_DISCONN\r\n", strlen("WIFI_DISCONN\r\n"));	//【Debug】
//					Serial2_SendString("WIFI_DISCONN\r\n", strlen("WIFI_DISCONN\r\n"));	//【Debug】
//					break;
//				case MSG_MQTT_DISCONN:
//					mqtt.isMqttConnect = DISCONNECT;
////					Serial3_SendString("MQTT_DISCONN\r\n", strlen("MQTT_DISCONN\r\n"));	//【Debug】
//					Serial2_SendString("MQTT_DISCONN\r\n", strlen("MQTT_DISCONN\r\n"));	//【Debug】
//					break;
//				case MSG_MQTT_CONN_SUCCESS:
//					
////					Serial3_SendString("MQTT_CONN_SUCCESS\r\n", strlen("MQTT_CONN_SUCCESS\r\n"));	//【Debug】
//					Serial2_SendString("MQTT_CONN_SUCCESS\r\n", strlen("MQTT_CONN_SUCCESS\r\n"));	//【Debug】
//					break;
//				case MSG_WIFI_CONN_SUCCESS:
//					
////					Serial3_SendString("WIFI_CONN_SUCCESS\r\n", strlen("WIFI_CONN_SUCCESS\r\n"));	//【Debug】
//					Serial2_SendString("WIFI_CONN_SUCCESS\r\n", strlen("WIFI_CONN_SUCCESS\r\n"));	//【Debug】
//					break;
//				case MSG_DOWNCMD:
//					
////					Serial3_SendString("DOWNCMD\r\n", strlen("DOWNCMD\r\n"));	//【Debug】
//					Serial2_SendString("DOWNCMD\r\n", strlen("DOWNCMD\r\n"));	//【Debug】
//					
////					【TODO】在此处调用下行命令相关函数
//					AT_ParseCmdMsg(rx3_msg.buf, read_len, cmd_keywords, &cmd);
//				
//					switch(cmd.type)
//					{
//						case CMD_UNKNOWN:
////							Serial3_SendString("CMD_UNKNOWN\r\n", strlen("CMD_UNKNOWN\r\n"));	//【Debug】
//							Serial2_SendString("CMD_UNKNOWN\r\n", strlen("CMD_UNKNOWN\r\n"));	//【Debug】
//							break;
//						case CMD_WPVR:
////							Serial3_SendString("CMD_WPVR\r\n", strlen("CMD_WPVR\r\n"));	//【Debug】
//							Serial2_SendString("CMD_WPVR\r\n", strlen("CMD_WPVR\r\n"));	//【Debug】
//							MyWaterP_SetVoltageRatio(atoi(cmd.para_value));
//							break;
//						case CMD_APRS:
////							Serial3_SendString("CMD_APRS\r\n", strlen("CMD_APRS\r\n"));	//【Debug】
//							Serial2_SendString("CMD_APRS\r\n", strlen("CMD_APRS\r\n"));	//【Debug】
//							MyAirP_SetRunStatus(atoi(cmd.para_value));
//							break;
//						case CMD_WHRS:
////							Serial3_SendString("CMD_WHRS\r\n", strlen("CMD_WHRS\r\n"));	//【Debug】
//							Serial2_SendString("CMD_WHRS\r\n", strlen("CMD_WHRS\r\n"));	//【Debug】
//							MyWaterH_SetRunStatus(atoi(cmd.para_value));
//							break;
//						case CMD_ALVR:
////							Serial3_SendString("CMD_ALVR\r\n", strlen("CMD_ALVR\r\n"));	//【Debug】
//							Serial2_SendString("CMD_ALVR\r\n", strlen("CMD_ALVR\r\n"));	//【Debug】
//							MyAquariumL_SetVoltageRatio(atoi(cmd.para_value));
//							break;
//						case CMD_PGLVR:
////							Serial3_SendString("CMD_PGLVR\r\n", strlen("CMD_PGLVR\r\n"));	//【Debug】
//							Serial2_SendString("CMD_PGLVR\r\n", strlen("CMD_PGLVR\r\n"));	//【Debug】
//							MyPlantGL_SetVoltageRatio(atoi(cmd.para_value));
//							break;
//						case CMD_FT:
////							Serial3_SendString("CMD_FT\r\n", strlen("CMD_FT\r\n"));	//【Debug】
//							Serial2_SendString("CMD_FT\r\n", strlen("CMD_FT\r\n"));	//【Debug】
//							MyFeeder_Triger(atoi(cmd.para_value));
//							break;
//						default:
////							Serial3_SendString("CMDTYPEERROR\r\n", strlen("CMDTYPEERROR\r\n"));	//【Debug】
//							Serial2_SendString("CMDTYPEERROR\r\n", strlen("CMDTYPEERROR\r\n"));	//【Debug】
//					}
//					if(cmd.type != CMD_UNKNOWN)//如果命令类型没有识别失败
//					{
//						/*重置main字符串*/
//						memset(ATCMD_MQTTPUB_UPRSP_main,0,ATCMD_MQTTPUB_UPRSP_LEN);
//						
//						/*将body复制到main并将request_id嵌入*/
//						snprintf(
//							ATCMD_MQTTPUB_UPRSP_main,
//							ATCMD_MQTTPUB_UPRSP_LEN,
//							ATCMD_MQTTPUB_UPRSP_body,
//							cmd.request_id);
//						
//						/*发送上行响应*/
//						Serial3_SendString(
//							ATCMD_MQTTPUB_UPRSP_main, 
//							strlen(ATCMD_MQTTPUB_UPRSP_main));
//						Serial2_SendString(
//							ATCMD_MQTTPUB_UPRSP_main, 
//							strlen(ATCMD_MQTTPUB_UPRSP_main));	//【Debug】
//					}
//					break;
//				default:
////					Serial3_SendString("MSGUNKNOWN\r\n", strlen("MSGUNKNOWN\r\n"));
//					Serial2_SendString("MSGUNKNOWN\r\n", strlen("MSGUNKNOWN\r\n"));
//			}
//		
		/*重启DMA，可以开始接受新Rx数据*/
		DMA_SetCurrDataCounter(DMA1_Channel3, RX3_BUF_MAX_SIZE);
		DMA_Init(DMA1_Channel3, &DMA_IS_Rx);
		DMA_Cmd(DMA1_Channel3, ENABLE);
		Serial3_USART3_Cmd(ENABLE);
		
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
}




