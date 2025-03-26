#include "MyHCSR04.h"

#define GPIOTrig GPIOA
#define PinTrig GPIO_Pin_12
#define GPIOEcho GPIOB
#define PinEcho GPIO_Pin_15

EXTI_InitTypeDef EXTI_InitStruct;

uint8_t MyHCSR04_TrigCtrler_Ctrl=0;
uint8_t MyHCSR04_TrigSETer_Ctrl=0;


uint8_t MyHCSR04_EchoCtrlerSM_Ctrl=0;
uint8_t MyHCSR04_EchoCtrlerSM_State=0;

uint32_t MyHCSR04_CountEchoSpan=0;

uint16_t MyHCSR04_CountEchoSpanFiltered= 0;

void MyHCSR04_Trig_Init(void) {	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//��㣺�Ļط�PWM��Ҫ��AFΪOut
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOTrig, PinTrig, Bit_RESET);
	
	//Serial_SendStringPacketV2(USART2, "MyHCSR04_Trig_Init_End\r\n");
}


void MyHCSR04_Echo_Init(void) {
	//RCC-GPIO, EXTI=====
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//EXTI��Ȼ����APB2���ߣ�������RCCʱ�������ֶ�����
	
	//Init-GPIO, EXTI=====
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//EXTI=====
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	//EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_StructInit(&EXTI_InitStruct);
	EXTI_InitStruct.EXTI_Line = EXTI_Line15;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//ע�⣺С�Ļ���Rising��Falling
	EXTI_Init(&EXTI_InitStruct);
	
	//��㣺��©NVIC
	//NVIC=====
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
}

uint16_t MyHCSR04_ResultFilter(uint32_t t) {
	
	static uint8_t ResultFilter_Counter = 0;
	ResultFilter_Counter ++;
	
	static uint16_t ResultFilter_Arr[10];
	
	MyArray_TailAdd_uint16_t(ResultFilter_Arr, t, 10);
	
	if (ResultFilter_Counter < 10) {
		return MyArray_GetAverage_uint16_t(ResultFilter_Counter, ResultFilter_Arr);
	} else{
		return MyArray_GetAverage_uint16_t(10, ResultFilter_Arr);
	}
}

void MyHCSR04_TrigCtrlerSwitchOn(void) {
	MyHCSR04_TrigCtrler_Ctrl = 1;//��׼MyHCSR04_TrigCtrler()
	MyHCSR04_TrigSETer_Ctrl = 1;//��׼MyHCSR04_TrigSETer()
}
//TrigCtrler���ź����ڻ��TIM3_DIV100����1��TIM2�ж�����

void MyHCSR04_TrigCtrler(void){//����Trig�źŵķ����Լ�Ϊ����Echo�ź���׼��
	static uint8_t TrigCtrlerCount2;//���ڼ�¼�ĺ���ִ�д���
	if(MyHCSR04_TrigCtrler_Ctrl == 1){//��Ҫ���MyTIM3_DIV100()��MyHCSR04_TrigCtrlerSwitchOn()��׼
		MyHCSR04_TrigSETer();//����Trig,��ʼ����Trig�ź�
		TrigCtrlerCount2 ++;//������2��IT(10us)��ɽ�������if
		if(TrigCtrlerCount2 >= 2){//Ҫ������Trig���15us(15��TIM1IT����)������Trig
			MyHCSR04_TrigRESETer();//����Trig,��������Trig�ź�,���ý���Echo�źŵ�׼������
			MyHCSR04_TrigCtrler_Ctrl = 0;//��λ�źŷ���������ظ�
			TrigCtrlerCount2 = 0;//�¸�����ҲҪ��֤SET��RESET֮��ļ��
		}
	}
}

void MyHCSR04_TrigSETer(void){//����Trig,��ʼ����Trig�ź�
	if(MyHCSR04_TrigSETer_Ctrl == 1){//���������1�θ�λ�ź�
		GPIO_WriteBit(GPIOTrig,PinTrig,Bit_SET);//��Trig���øߵ�ƽ
		MyHCSR04_TrigSETer_Ctrl = 0;//��Trig���øߵ�ƽ1�κ����ظ�
	}
}

void MyHCSR04_TrigRESETer(void){//����Trig,��������Trig�ź�,���ý���Echo�źŵ�׼������
	GPIO_WriteBit(GPIOTrig,PinTrig,Bit_RESET);//���Trig�źŷ��ͺ�ָ�Trig��Ϊ�͵�ƽ
	MyHCSR04_EchoCtrlerSM_Ctrl = 1;//����EchoCtrlSM_Ctrl���ж�Ӧif������
	MyHCSR04_EchoCtrlerSM_State = 0;//��ֹbug,��֤��Echo�ź�ʱEchoCtrl״̬��״̬Ϊ0
	MyHCSR04_SetEXITTrig(EXTI_Trigger_Rising);//��֤����Echo�ź�������ʱEXTImodeΪRising
}

void MyHCSR04_SetEXITTrig(EXTITrigger_TypeDef mode){
	EXTI_InitStruct.EXTI_Trigger=mode;//����EXTI����ģʽΪmode
	EXTI_Init(&EXTI_InitStruct);//��㣺��©����mode����ͨ��InitӦ��
}

void MyHCSR04_EchoCtrlerSM(void){//����Echo�ź��൱��CNTֵ
	static uint8_t MyHCSR04_EchoCtrlerSM_State;//״̬��״̬
	
	static uint32_t CountEchoStart;//�洢Echo�źſ�ʼʱCNT����ֵ
	
	if(MyHCSR04_EchoCtrlerSM_Ctrl == 1){
		
		//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_Ctrl_In\r\n");
		
		if(MyHCSR04_EchoCtrlerSM_State == 0) {//״̬0ʱ
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State0_In\r\n");
			
			CountEchoStart=TIM_GetCounter(TIM3);//��¼Echo�źſ�ʼʱCNT����ֵ,�����ڶ�,������TIM3��װ
			MyHCSR04_SetEXITTrig(EXTI_Trigger_Falling);//�����½��ش���EXIT
			MyHCSR04_EchoCtrlerSM_State = 1;//����״̬Ϊ1
		} else if(MyHCSR04_EchoCtrlerSM_State == 1) {//״̬1ʱ
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State1_In\r\n");
			
			MyHCSR04_CountEchoSpan = TIM_GetCounter(TIM3) - CountEchoStart;//����Echo�ź��൱��CNTֵ
			MyHCSR04_CollectEchoSpan();
			MyHCSR04_SetEXITTrig(EXTI_Trigger_Rising);//���������ش���EXIT
			MyHCSR04_EchoCtrlerSM_State = 0;//����״̬Ϊ0
			MyHCSR04_EchoCtrlerSM_Ctrl = 0;//�ر�EchoCtrler״̬��
		}
	}
}

void MyHCSR04_CollectEchoSpan(void) {
	static uint8_t NumCount;
	static uint16_t CollectNums[3];
	if(NumCount < 3) {	//��NumCount�ӵ�3�������ټ�
		NumCount ++;
	}
	MyArray_TailAdd_uint16_t(CollectNums, MyHCSR04_CountEchoSpan, 3);
	MyHCSR04_CountEchoSpanFiltered= MyArray_GetAverage_uint16_t(NumCount, CollectNums);//�������extern����
}

uint16_t MyHCSR04_GetResult_mm(void) {
	return (float)MyHCSR04_CountEchoSpanFiltered  * 0.1715;
}
