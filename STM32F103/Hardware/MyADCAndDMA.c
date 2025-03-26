//=====================================
//���ļ���������ˮ�ʴ����������մ�������ˮ�´�����������ʪ�ȴ�������ADC+ADC_DMACmd����
//STM32F103C8T6��ADC���ã�ADC1��ADC2
//ADCͨ�������Ŷ�Ӧ��ϵ�̶���һ��17��ͨ��������������ADCͨ��
//STM32F103C8T6��DMA���ã�DMA1��7ͨ����
//DMAÿ��ͨ���̶����Դ������豸��һ��ͨ������Ӧ�����豸���豸֮��Ŀ�������ͻ��ADC1->Ch1
//=====================================
#include "MyADCAndDMA.h"

uint16_t MyADCAndDMA_Result[4];

void MyADCAndDMA_Init(uint8_t ADC_Ch_Num) {
	
	//RCC=====
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//ʱ����

	//ADC��ʼ��=====
	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;	//��������ת��
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//���ⲿ����
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;	//ѡ��ADC
	ADC_InitStruct.ADC_NbrOfChannel = ADC_Ch_Num;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;	//����ɨ��ģʽ����Ch��
	ADC_Init(ADC1, &ADC_InitStruct);
		
	
	//ADC_DMACmd=====
	ADC_DMACmd(ADC1, ENABLE);
	
	
	//DMA����=====
	DMA_InitTypeDef DMA_InitStruct;
	DMA_StructInit(&DMA_InitStruct);
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	//�ϣ��������ݼĴ���ADC1->DR
	//�ϣ���㣺��©ADC1->DRǰ��&
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//�ϣ�ADC1 got continue mode, dont need inc
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
	//RSTCAL����λУ׼ (Reset calibration) λ3 
	//��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ���������
	//0��У׼�Ĵ����ѳ�ʼ����
	//1����ʼ��У׼�Ĵ�����
	//ע��������ڽ���ת��ʱ
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	//CAL��A/DУ׼ (A/D Calibration) λ2 
	//��λ����������Կ�ʼУ׼������У׼����ʱ��Ӳ�������
	//0��У׼��ɣ�
	//1����ʼУ׼��
	
	//ADCSTART=====
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//Log=====
	//Serial_SendStringPacketV2(USART2, "MyADCAndDMA_Init_End\r\n");
}
	//ADC1ͨ������
	//	ͨ��0	PA0
	//	ͨ��1	PA1
	//	ͨ��2	PA2
	//	ͨ��3	PA3
	//	ͨ��4	PA4
	//	ͨ��5	PA5
	//	ͨ��6	PA6
	//	ͨ��7	PA7
	//	ͨ��8	PB0
	//	ͨ��9	PB1
	//	ͨ��10	PC0
	//	ͨ��11	PC1
	//	ͨ��12	PC2
	//	ͨ��13	PC3
	//	ͨ��14	PC4
	//	ͨ��15	PC5
