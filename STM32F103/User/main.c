#include "main.h"

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
	
//	MsgQueue_t q;
//	InitQueue(&q);
	
//	ESP8266_Init_Str();
			
	Serial_Init(USART3, 115200, 0, 0);	//Serial1——ESP8266
	Serial_SendStringV2(USARTPC, "Serial3_On\r\n");//【Debug】
	
	//Run=====
	while(1) {
		
//		if(Serial_RxFlag[Serial_Ch_ESP8266] == 1) {
//			
//			enqueue(&q, Serial_Rx3StringPacket);
//			
//			Serial_RxFlag[Serial_Ch_ESP8266] = 0;
//		}
		
		if(Serial_RxFlag[2] == 1) {
			
			Serial_SendStringV2(USARTESP8266, Serial_Rx2StringPacket);
			
			Serial_RxFlag[2] = 0;
		}
		
		if(Serial_RxFlag[3] == 1) {
			Serial_SendStringV2(USARTPC, "86:\r\n");
			Serial_SendStringV2(USARTPC, Serial_Rx3StringPacket);
			
			Serial_RxFlag[3] = 0;//确定用完了再清除RxFlag标志位
		}
		
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
	}
}



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
