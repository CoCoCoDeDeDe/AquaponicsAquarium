#include "main.h"

int16_t num_test1 = 0;

int main(void)
{
	Delay_ms(1000);	//等待设备电压稳定
	
//RCC=====
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//【错点：遗漏】
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//错点：用EXTI要开启AFIO

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
//NVIC_Group=====
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	//上：NVIC优先级分组的配置对所有中断请求是统一的
	
//Init=====
	MyTest_PB5_Init();
	
	Serial_Init(USART2, 115200, 0, 0);	//Serail2——PC
	Serial_SendStringV2(USART2, "Serial2_On\r\n");//【Debug】
	
	OLED_Init();
	OLED_Clear();
	
	MyTIM1_Init(3, 3);
	MyTIM2_Init(1, 1);
	MyTIM3_Init(1, 1);
	MyTIM4_Init(0, 0);
	
	MyTIMx_ENABLECmd(TIM1);
	MyTIMx_ENABLECmd(TIM2);
	MyTIMx_ENABLECmd(TIM3);
	MyTIMx_ENABLECmd(TIM4);
	
	MyWaterTS_Init();			//DS18B20
	MyAirS_Init(1, 1);			//DHT11
	MyWaterSS_Trig_Init();		//HC-SR04
	MyWaterSS_Echo_Init(2, 2);	//HC-SR04
	
	MyFeeder_Init(500);					//PA8-OC1
	MyWaterP_Init(WaterPVR);			//PA9-OC2
	MyAquariumL_Init(AquariumLVR);		//PA10-OC3
	MyPlantGL_Init(PlantGLVR);			//PA11-OC4
	
	MyWaterQS_Init();
	MySoilMS_Init();
	MyIlluminationS_Init();
	
	MyADCAndDMA_Init(4);
	
	MyAirP_Init();
	MyWaterH_Init();
			
//	Serial_Init(USART3, 115200, 0, 0);	//Serial1——ESP8266
//	Serial_SendStringV2(USARTPC, "Serial3_On\r\n");//【Debug】

	Serial3_Init_All();

//	char tst1[] = "tst1";
//	
//	uint16_t tstNum2 = 0;
	
//	int8_t num_dst = 2;
//	int8_t arr_src[] = {1, 2, 3, 4};
//	
//	OLED_ShowNum(1, 1, num_dst , 2);
//	OLED_ShowNum(2, 1, arr_src[0] , 2);
//	OLED_ShowNum(2, 3, arr_src[1] , 2);
//	OLED_ShowNum(2, 5, arr_src[2] , 2);
//	OLED_ShowNum(2, 7, arr_src[3] , 2);

//	Delay_s(1);
//	
//	DMA_InitTypeDef DMA_IS_TEST;
//	
//	DMA_DeInit(DMA1_Channel2);
//	DMA_IS_TEST.DMA_BufferSize			= 4;//0代表关闭DMA转移
//	DMA_IS_TEST.DMA_DIR					= DMA_DIR_PeripheralDST;//外设为目的地
//	DMA_IS_TEST.DMA_M2M					= DMA_M2M_Enable;//非内存到内存
//	DMA_IS_TEST.DMA_MemoryBaseAddr		= (u32)arr_src;
//	DMA_IS_TEST.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
//	DMA_IS_TEST.DMA_MemoryInc			= DMA_MemoryInc_Enable;
//	DMA_IS_TEST.DMA_Mode				= DMA_Mode_Normal;
//	DMA_IS_TEST.DMA_PeripheralBaseAddr	= (u32)&num_dst;
//	DMA_IS_TEST.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
//	DMA_IS_TEST.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;//【WARNING】每发1Byte和DMA的内存指向地址自增1单位同步
//	DMA_IS_TEST.DMA_Priority			= DMA_Priority_High;
//	DMA_Init(DMA1_Channel2, &DMA_IS_TEST);
//	
//	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);//TC:一次DMA Buffer循环结束，既计数归零时中断
//	
//	DMA_Cmd(DMA1_Channel2, ENABLE);//DMA现在就开始转移数据
	
	/*【Debug】*/
	Serial3_SendString("While\r\n", strlen("While\r\n"));
	
	while(1) {
		
		if(rx3_idle_flag == 1){
			/*【Debug】*/
			//Serial3_SendString("InIf\r\n", strlen("InIf\r\n"));
			
			Serial3_SendString(rx3_buf, strlen(rx3_buf));
		
			rx3_idle_flag = 0;
		}
		
//		Delay_s(1);
		
//		OLED_ShowNum(1, 1, num_dst , 2);
//		OLED_ShowNum(2, 1, arr_src[0] , 2);
//		OLED_ShowNum(2, 3, arr_src[1] , 2);
//		OLED_ShowNum(2, 5, arr_src[2] , 2);
//		OLED_ShowNum(2, 7, arr_src[3] , 2);
//		
//		
//		Delay_s(3);
//		
//		arr_src[0] = arr_src[0] + arr_src[3];
//		arr_src[1] = arr_src[0] + 1;
//		arr_src[2] = arr_src[1] + 1;
//		arr_src[3] = arr_src[2] + 1;
		
		/*在重置DMA时，以下3函数缺一不可*/
//		DMA_Cmd(DMA1_Channel2, DISABLE);
//		DMA_Init(DMA1_Channel2, &DMA_IS_TEST);
//		DMA_Cmd(DMA1_Channel2, ENABLE);
		
		/*在重置DMA时，以下3函数缺一不可*/
		//DMA_Cmd(DMA1_Channel2, DISABLE);
		//DMA_SetCurrDataCounter(DMA1_Channel2, sizeof(arr_src));
		//DMA_Cmd(DMA1_Channel2, ENABLE);
		
		//DMA开始传输，传输完既Buffer为0后后关闭
		
//		tstNum2 ++;
//		
//		OLED_ShowNum(3, 1, tstNum2, 4);
//		
//		Serial3_SendString(tst1, strlen(tst1));
//		
//		OLED_ShowNum(2, 1, tc_count, 4);
//		
//		OLED_ShowNum(1, 1, tst_num1, 4);
//		
//		Delay_us(10000000);
		
//		if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
//			
//			enqueue(&q, Serial_Rx3StringPacket);
//			
//			Serial_RxFlag[Serial_Ch_ESP8266] = 0;
//		}
		
//		if(Serial_RxFlag[2] == 1) {
//			
//			Serial_SendStringV2(USARTESP8266, Serial_Rx2StringPacket);
//			
//			Serial_RxFlag[2] = 0;
//		}
//		
//		if(Serial_RxFlag[3] == 1) {
//			Serial_SendStringV2(USARTPC, "86:\r\n");
//			Serial_SendStringV2(USARTPC, Serial_Rx3StringPacket);
//			
//			Serial_RxFlag[3] = 0;//确定用完了再清除RxFlag标志位
//		}
		
//		OLED_ShowNum(1, 1, MyADCAndDMA_Result[0], 4);
//		OLED_ShowNum(2, 1, MyADCAndDMA_Result[1], 4);
//		OLED_ShowNum(3, 1, MyADCAndDMA_Result[2], 4);
//		OLED_ShowNum(4, 1, MyADCAndDMA_Result[3], 4);
		
//		OLED_ShowNum(3, 1, WaterSD, 16);
		
//		OLED_ShowNum(1, 1, MyTIM_1UpCount, 16);
//		OLED_ShowNum(2, 1, MyTIM_2Count, 16);
//		OLED_ShowNum(3, 1, MyTIM_3Count, 16);
//		OLED_ShowNum(4, 1, MyTIM_4Count, 16);

//		OLED_ShowNum(4, 1, MyAirS_DataArr[0], 8);
//		OLED_ShowNum(4, 9, MyAirS_DataArr[2], 8);
		
//		OLED_ShowNum(4, 1, MyAirS_Count_ReadInterval, 16);

//		OLED_ShowNum(1, 1, MyAirS_Count_ReadBit, 16);
		
		
//		MyAirP_Cmd(Bit_RESET);
		
//		MyWaterH_Cmd(Bit_RESET);
		
//		MyPlantGrowLamp_Cmd(Bit_RESET);
		
		
		
//		OLED_ShowNum(1, 1, MyWaterTS_ReadPacket_16Bit_Temp, 16);
//		OLED_ShowNum(2, 1, MyWaterTS_Result_12Bit_H7Bit, 16);
//		OLED_ShowNum(3, 1, MyWaterTS_Result_12Bit_L4Bit, 16);

//		Delay_us(1);//以免循环太快，CPU压力太大
	}//while(1) END
}//main() END


void EXTI4_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line4) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line4);
		
		MyAirS_ReaderSM();//【错点】写错在EXTI15_10_IEQHandler
		
	}
}

void EXTI15_10_IRQHandler(void) {	//EXTI Line 10to15的中断是合并的
	if(EXTI_GetITStatus(EXTI_Line15) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line15);
		
		MyWaterSS_EchoCtrlerSM();
		
	}
}


//void InitQueue(MsgQueue_t *q) {
//	q->idx_front = 0;
//	q->idx_rear = 0;
//	q->count = 0;
//	for(uint8_t i = 0; i < QUEUE_SIZE; i++) {
//		q->data[i] = NULL;
//	}
//}

//static int8_t enqueue(MsgQueue_t* q, const char *str) {
//	if (q->count >= QUEUE_SIZE) {
//		Serial_SendStringV2(USARTPC, "MsgQueueFull\r\n");
//		return -1;	//队列已满
//	}
//	//分配内存并复制字符串
//	char *newStr = (char*)malloc(strlen(str) + 1);
//	if(!newStr) return -2;	//输入的参数字符串为空
//	strcpy(newStr, str);
//	
//	q->data[q->idx_rear] = newStr;
//	q->idx_rear = (q->idx_rear + 1) % QUEUE_SIZE;
//	q->count ++;
//	return 1;	//成功
//}

//char *dequeue(MsgQueue_t *q) {
//	if(q->count <= 0) {
//		return NULL;	//队列为空
//	}
//	char *str = q->data[q->idx_front];
//	q->data[q->idx_front] = NULL;
//	q->idx_front = (q->idx_front + 1) % QUEUE_SIZE;
//	q->count --;
//	return str;
//}
