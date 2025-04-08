#include "main.h"

#include "stm32f10x.h"                  // Device header

#include <string.h>

#include "Array.h"

#include "stdlib.h"

#include "Delay.h"

#include "MyTest.h"

#include "OLED.h"



#include "math.h"

#include "MyTIM.h"

#include "MyFeeder.h"
#include "MyWaterP.h"

//#include "MyWaterSS.h"
#include "AirS.h"
#include "MyWaterTS.h"

#include "WaterLS.h"
#include "WaterQS.h"
#include "LightS.h"

#include "MyADCAndDMA.h"

#include "PlantGL.h"
#include "AirP.h"
#include "MyWaterH.h"

#include "Serial3.h"
#include "AT.h"

#include "Serial2.h"

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
//	MyTest_PB5_Init();
	
//	Serial_Init(USART2, 115200, 0, 0);	//Serail2——PC
//	Serial_SendStringV2(USART2, "Serial2_On\r\n");//【Debug】
	
	OLED_Init();
	OLED_Clear();
	
	MyTIM1_Init(3, 3);
	MyTIM2_Init(1, 1);
	MyTIM3_Init(1, 1);
//	MyTIM4_Init(3, 3);
	
	MyTIMx_ENABLECmd(TIM1);
	MyTIMx_ENABLECmd(TIM2);
	MyTIMx_ENABLECmd(TIM3);
//	MyTIMx_ENABLECmd(TIM4);
	
	MyWaterTS_Init();			//DS18B20
	MyAirS_Init(1, 1);			//DHT11
//	MyWaterSS_Trig_Init();		//HC-SR04
//	MyWaterSS_Echo_Init(0, 0);	//HC-SR04
	
	MyFeeder_Init(500);					//PA8-OC1
	MyWaterP_Init(WaterPVR);			//PA9-OC2
	
	MyPlantGL_Init(PlantGLVR);			//PA11-OC4
	
	
	WaterLS_Init();
	WaterQS_Init();
	LightS_Init();
	
	MyADCAndDMA_Init(4);
	
	MyAirP_Init();
	MyWaterH_Init();

	Serial2_Init_All();
	Serial2_SendString("Serial2_On\r\n", strlen("Serial2_On\r\n"));	//【Debug】
	
	/*让AT_SM在其他外设都初始化完后才能开始处理消息和连接ESP8266*/
	AT_Init_Str();
	Serial3_Init_All(115200, DMA_Priority_VeryHigh, 0, 0, 1, 1);
//	Serial3_SendString("DuBug:\r\nSerial3_On\r\n", strlen("DuBug:\r\nSerial3_On\r\n"));	//【Debug】
	at_sm_status.runtimes = 0;
	at_sm_status.runtimes_repriod = 10000;
	at_sm_status.state = AT_SM_S_PowerOn;
//	Serial3_SendString("DuBug:\r\nSetAT_SM_S_PowerOn\r\n", strlen("DuBug:\r\nSetAT_SM_S_PowerOn\r\n"));	//【Debug】
	
	
	while(1)
	{
//		if(rx2_msg.rc == 1)
//		{
//			Serial3_SendString(rx2_msg.str, rx2_msg.len);
//			
//			rx2_msg.rc = 0;
//		}
		
//		if(rx3_msg.rc_1 == 1)
//		{
//			Serial2_SendString("Rx3:\r\n", strlen("Rx3:\r\n"));
//			Serial2_SendString(rx3_msg.buf, strlen(rx3_msg.buf));
//			
//			rx3_msg.rc_1 = 0;
//		}
		
		
		Delay_ms(1);
		
//		OLED_ShowNum(1, 1, WaterPVR, 4);
//		OLED_ShowNum(2, 1, AirPRS, 4);
//		OLED_ShowNum(3, 1, WaterHRS, 4);
//		OLED_ShowNum(4, 1, AquariumLVR, 4);
//		OLED_ShowNum(1, 9, PlantGLVR, 4);
//		OLED_ShowNum(2, 9, FeederRS, 4);
//		
//		Delay_us(100);
		
//		OLED_ShowNum(1, 1, MyADCAndDMA_Result[0], 4);
//		OLED_ShowNum(2, 1, MyADCAndDMA_Result[1], 4);
//		OLED_ShowNum(3, 1, MyADCAndDMA_Result[2], 4);
//		OLED_ShowNum(4, 1, MyADCAndDMA_Result[3], 4);
		

//		OLED_ShowNum(1, 1, MyWaterTS_ReadPacket_16Bit_Temp, 16);
//		OLED_ShowNum(1, 9, MyWaterTS_Result_12Bit_H7Bit, 2);
//		OLED_ShowNum(3, 1, MyWaterTS_Result_12Bit_L4Bit, 2);

//		OLED_ShowNum(2, 9, MyAirS_DataArr[0], 2);
//		OLED_ShowNum(3, 9, MyAirS_DataArr[2], 2);
//		OLED_ShowNum(4, 9, WaterSD, 4);
		
//		OLED_ShowNum(4, 1, MyAirS_Count_ReadInterval, 16);

//		OLED_ShowNum(1, 1, MyAirS_Count_ReadBit, 16);
		
		
//		MyAirP_Cmd(Bit_RESET);
		
//		MyWaterH_Cmd(Bit_RESET);
		
//		MyPlantGrowLamp_Cmd(Bit_RESET);
		
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
		
//		MyWaterSS_EchoCtrlerSM();
	}
}


/*函数注释模板*/
/**
* @name	
* @brief		
*  
*  
* @param	
* @param	
* @param	
* @param	
* @param	
* @param	
* @retval		
*/

/** 
  * @brief  Bit_SET and Bit_RESET enumeration  
  */

