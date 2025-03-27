#include "stm32f10x.h"                  // Device header

#include <string.h>

#include "Array.h"

#include "Delay.h"

#include "MyTest.h"

#include "OLED.h"

#include "Serial.h"

#include "MyTIM.h"

#include "MySG90.h"
#include "MyWaterPump.h"

#include "MyHCSR04.h"
#include "MyDHT11.h"
#include "MyDS18B20.h"

#include "MyWaterQSensor.h"
#include "MySoilMoistureSensor.h"
#include "MyLightSensor.h"

#include "MyADCAndDMA.h"

#include "MyPlantGrowLamp.h"
#include "MyAirPump.h"
#include "MyAquariumHeater.h"

#include "MyAquariumLight.h"

#include "MyESP8266.h"

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
	
	OLED_Init();
	OLED_Clear();
	
	Serial_Init(USART3, 115200, 0, 0);	//Serial1——ESP8266
	Serial_SendStringPacketV2(USARTMyESP8266, "AT\r\n");//【Debug】
	
	Serial_Init(USART2, 115200, 3, 3);	//Serail2——PC
	Serial_SendStringPacketV2(USART2, "Serial2_On\r\n");//【Debug】
	
	MyTIM1_Init(3, 3);
	MyTIM2_Init(0, 0);
	MyTIM3_Init(1, 1);
	MyTIM4_Init(3, 3);
	
	MyTIMx_ENABLECmd(TIM1);
	MyTIMx_ENABLECmd(TIM2);
	MyTIMx_ENABLECmd(TIM3);
	
	MyAquariumLight_Init(20000);	//PA8-OC1
	MySG90_Init(1500);			//PA9-OC2
	MyWaterPump_Init(0);		//PA10-OC3
	
	MyHCSR04_Trig_Init();	//PA8-OC1
	MyHCSR04_Echo_Init(2, 2);	//PB15-EXTI
	
	MyDHT11_Init(1, 1);//【错点】遗漏main中运行Init
	MyDS18B20_Init();//【错点】遗漏Init
	
	MyWaterQSensor_Init();
	MySoilMoistureSensor_Init();
	MyLightSensor_Init();
	
	MyADCAndDMA_Init(4);
	
	MyPlantGrowLamp_Init();
	MyAirPump_Init();
	MyAquariumHeater_Init();
	
	
	
	
	
//Run=====
	while(1) {
//		if(Serial_RxFlag[3] == 1) {
//			OLED_ShowString(1, 1, "USART3:");
//			OLED_ShowString(2, 1, "                ");
//			OLED_ShowString(2, 1, Serial_Rx3StringPacket);
//			
//			Serial_SendStringPacketV2(USART2, Serial_Rx3StringPacket);
//			
//			Serial_RxFlag[3] = 0;
//		}

//		if (Serial_RxFlag[2] == 1) {
//			OLED_ShowString(3, 1, "USART2:");
//			OLED_ShowString(4, 1, "                ");
//			OLED_ShowString(4, 1, Serial_Rx2StringPacket);
//			
//			Serial_SendStringPacketV2(USART3, Serial_Rx2StringPacket);
//			
//			Serial_RxFlag[2] = 0;
//		}
		

		
//		OLED_ShowNum(1, 1, MyADCAndDMA_Result[0], 4);
//		OLED_ShowNum(2, 1, MyADCAndDMA_Result[1], 4);
//		OLED_ShowNum(3, 1, MyADCAndDMA_Result[2], 4);
//		OLED_ShowNum(4, 1, MyADCAndDMA_Result[3], 4);
		
//		float HCSR04_distanceTemp = HCSR04_distance;
//		if(HCSR04_distanceTemp != HCSR04_distance) {
//			OLED_ShowString(3, 1, "                ");
//			OLED_ShowNum(3, 1, (uint16_t)HCSR04_distance, 4);
//			
//			HCSR04_distanceTemp = HCSR04_distance;
//		}
		
//		OLED_ShowNum(1, 1, MyTIM_1UpCount, 16);
//		OLED_ShowNum(2, 1, MyTIM_2Count, 16);
//		OLED_ShowNum(3, 1, MyTIM_3Count, 16);
//		OLED_ShowNum(4, 1, MyTIM_4Count, 16);

//		OLED_ShowNum(4, 1, MyDHT11_DataArr[0], 8);
//		OLED_ShowNum(4, 9, MyDHT11_DataArr[2], 8);
		
//		OLED_ShowNum(4, 1, MyDHT11_Count_ReadInterval, 16);

//		OLED_ShowNum(1, 1, MyDHT11_Count_ReadBit, 16);
		
		
//		MyAirPump_Cmd(Bit_RESET);
		
//		MyAquariumHeater_Cmd(Bit_RESET);
		
//		MyPlantGrowLamp_Cmd(Bit_RESET);
		
		
		
//		OLED_ShowNum(1, 1, MyDS18B20_ReadPacket_16Bit_Temp, 16);
//		OLED_ShowNum(2, 1, MyDS18B20_Result_12Bit_H7Bit, 16);
//		OLED_ShowNum(3, 1, MyDS18B20_Result_12Bit_L4Bit, 16);

		Delay_ms(100);//以免循环太快，CPU压力太大
	}
}



void EXTI4_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line4) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line4);
		
		MyDHT11_ReaderSM();//【错点】写错在EXTI15_10_IEQHandler
		
	}
}

void EXTI15_10_IRQHandler(void) {	//EXTI Line 10to15的中断是合并的
	if(EXTI_GetITStatus(EXTI_Line15) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line15);
		
		MyHCSR04_EchoCtrlerSM();
		
	}
}


