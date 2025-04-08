//=====================================
//本文件用于配置水质传感器、光照传感器、水温传感器、土壤湿度传感器的ADC+ADC_DMACmd配置
//STM32F103C8T6的ADC配置：ADC1，ADC2
//ADC通道和引脚对应关系固定，一共17个通道，部分引脚无ADC通道
//STM32F103C8T6的DMA配置：DMA1（7通道）
//DMA每个通道固定可以触发的设备，一个通道常对应几个设备，设备之间的开启不冲突。ADC1->Ch1
//=====================================
#include "MyADCAndDMA.h"


/* ADC 相关宏定义（根据硬件配置修改） */
#define ADC_RESOLUTION    12              // ADC 分辨率（12位）
#define ADC_FULL_SCALE    ((1 << ADC_RESOLUTION) - 1)  // 4095
#define VREF_VOLTAGE      3.3f            // 参考电压（V）

uint16_t MyADCAndDMA_Result[4];

void MyADCAndDMA_Init(uint8_t ADC_Ch_Num) {
	
	//RCC=====
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//时钟树

	//ADC初始化=====
	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;	//开启连续转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//无外部触发
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;	//选单ADC
	ADC_InitStruct.ADC_NbrOfChannel = ADC_Ch_Num;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;	//开启扫描模式（多Ch）
	ADC_Init(ADC1, &ADC_InitStruct);
		
	
	//ADC_DMACmd=====
	ADC_DMACmd(ADC1, ENABLE);
	
	
	//DMA配置=====
	DMA_InitTypeDef DMA_InitStruct;
	DMA_StructInit(&DMA_InitStruct);
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	//上：规则数据寄存器ADC1->DR
	//上：错点：遗漏ADC1->DR前的&
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//上：ADC1 got continue mode, dont need inc
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)MyADCAndDMA_Result;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = ADC_Ch_Num;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	
	
	//Cmd=====
	ADC_Cmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	
	//ADC_Calibration=====
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	//RSTCAL：复位校准 (Reset calibration) 位3 
	//该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。
	//0：校准寄存器已初始化；
	//1：初始化校准寄存器。
	//注：如果正在进行转换时
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	//CAL：A/D校准 (A/D Calibration) 位2 
	//该位由软件设置以开始校准，并在校准结束时由硬件清除。
	//0：校准完成；
	//1：开始校准。
	
	//ADCSTART=====
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//Log=====
	//Serial_SendStringV2(USART2, "MyADCAndDMA_Init_End\r\n");
}


/* 将 ADC 原始值转换为电压（带滤波，保留2位小数） */
float adc_to_voltage(uint16_t adc_value) {
    /* 核心公式：(ADC值 × 参考电压) / 满量程值 */
    float voltage = (float)adc_value * VREF_VOLTAGE / ADC_FULL_SCALE;
    
    /* 可选：四舍五入到2位小数（避免浮点噪声） */
    voltage = (int)(voltage * 100 + 0.5f) / 100.0f;
    return voltage;
}

	//ADC1通道配置
	//	通道0	PA0
	//	通道1	PA1
	//	通道2	PA2
	//	通道3	PA3
	//	通道4	PA4
	//	通道5	PA5
	//	通道6	PA6
	//	通道7	PA7
	//	通道8	PB0
	//	通道9	PB1
	//	通道10	PC0
	//	通道11	PC1
	//	通道12	PC2
	//	通道13	PC3
	//	通道14	PC4
	//	通道15	PC5
