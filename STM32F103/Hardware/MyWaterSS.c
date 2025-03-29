#include "MyWaterSS.h"

#define GPIOTrig GPIOA
#define PinTrig GPIO_Pin_12
#define GPIOEcho GPIOB
#define PinEcho GPIO_Pin_15

EXTI_InitTypeDef EXTI_InitStruct;

uint8_t MyWaterSS_TrigCtrler_Ctrl=0;
uint8_t MyWaterSS_TrigSETer_Ctrl=0;


uint8_t MyWaterSS_EchoCtrlerSM_Ctrl=0;
uint8_t MyWaterSS_EchoCtrlerSM_State=0;

uint32_t MyWaterSS_CountEchoSpan=0;

uint16_t MyWaterSS_CountEchoSpanFiltered= 0;

uint16_t WaterSD = 0;

void MyWaterSS_Trig_Init(void) {	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//��㣺�Ļط�PWM��Ҫ��AFΪOut
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOTrig, PinTrig, Bit_RESET);
	
	//Serial_SendStringPacketV2(USART2, "MyWaterSS_Trig_Init_End\r\n");
}


void MyWaterSS_Echo_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	NVIC_Init(&NVIC_InitStruct);
	
}

uint16_t MyWaterSS_ResultFilter(uint32_t t) {
	
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

void MyWaterSS_TrigCtrlerSwitchOn(void) {
	MyWaterSS_TrigCtrler_Ctrl = 1;//��׼MyWaterSS_TrigCtrler()
	MyWaterSS_TrigSETer_Ctrl = 1;//��׼MyWaterSS_TrigSETer()
}
//TrigCtrler���ź����ڻ��TIM3_DIV100����1��TIM2�ж�����

void MyWaterSS_TrigCtrler(void){//����Trig�źŵķ����Լ�Ϊ����Echo�ź���׼��
	static uint8_t TrigCtrlerCount2;//���ڼ�¼�ĺ���ִ�д���
	if(MyWaterSS_TrigCtrler_Ctrl == 1){//��Ҫ���MyTIM3_DIV100()��MyWaterSS_TrigCtrlerSwitchOn()��׼
		MyWaterSS_TrigSETer();//����Trig,��ʼ����Trig�ź�
		TrigCtrlerCount2 ++;//������2��IT(10us)��ɽ�������if
		if(TrigCtrlerCount2 >= 2){//Ҫ������Trig���15us(15��TIM1IT����)������Trig
			MyWaterSS_TrigRESETer();//����Trig,��������Trig�ź�,���ý���Echo�źŵ�׼������
			MyWaterSS_TrigCtrler_Ctrl = 0;//��λ�źŷ���������ظ�
			TrigCtrlerCount2 = 0;//�¸�����ҲҪ��֤SET��RESET֮��ļ��
		}
	}
}

void MyWaterSS_TrigSETer(void){//����Trig,��ʼ����Trig�ź�
	if(MyWaterSS_TrigSETer_Ctrl == 1){//���������1�θ�λ�ź�
		GPIO_WriteBit(GPIOTrig,PinTrig,Bit_SET);//��Trig���øߵ�ƽ
		MyWaterSS_TrigSETer_Ctrl = 0;//��Trig���øߵ�ƽ1�κ����ظ�
	}
}

void MyWaterSS_TrigRESETer(void){//����Trig,��������Trig�ź�,���ý���Echo�źŵ�׼������
	GPIO_WriteBit(GPIOTrig,PinTrig,Bit_RESET);//���Trig�źŷ��ͺ�ָ�Trig��Ϊ�͵�ƽ
	MyWaterSS_EchoCtrlerSM_Ctrl = 1;//����EchoCtrlSM_Ctrl���ж�Ӧif������
	MyWaterSS_EchoCtrlerSM_State = 0;//��ֹbug,��֤��Echo�ź�ʱEchoCtrl״̬��״̬Ϊ0
	MyWaterSS_SetEXITTrig(EXTI_Trigger_Rising);//��֤����Echo�ź�������ʱEXTImodeΪRising
}

void MyWaterSS_SetEXITTrig(EXTITrigger_TypeDef mode){
	EXTI_InitStruct.EXTI_Trigger=mode;//����EXTI����ģʽΪmode
	EXTI_Init(&EXTI_InitStruct);//��㣺��©����mode����ͨ��InitӦ��
}

void MyWaterSS_EchoCtrlerSM(void){//����Echo�ź��൱��CNTֵ
	static uint8_t MyWaterSS_EchoCtrlerSM_State;//״̬��״̬
	
	static uint32_t CountEchoStart;//�洢Echo�źſ�ʼʱCNT����ֵ
	
	if(MyWaterSS_EchoCtrlerSM_Ctrl == 1){
		
		//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_Ctrl_In\r\n");
		
		if(MyWaterSS_EchoCtrlerSM_State == 0) {//״̬0ʱ
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State0_In\r\n");
			
			CountEchoStart=TIM_GetCounter(TIM3);//��¼Echo�źſ�ʼʱCNT����ֵ,�����ڶ�,������TIM3��װ
			MyWaterSS_SetEXITTrig(EXTI_Trigger_Falling);//�����½��ش���EXIT
			MyWaterSS_EchoCtrlerSM_State = 1;//����״̬Ϊ1
		} else if(MyWaterSS_EchoCtrlerSM_State == 1) {//״̬1ʱ
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State1_In\r\n");
			
			MyWaterSS_CountEchoSpan = TIM_GetCounter(TIM3) - CountEchoStart;//����Echo�ź��൱��CNTֵ
			MyWaterSS_CollectEchoSpan();
			MyWaterSS_SetEXITTrig(EXTI_Trigger_Rising);//���������ش���EXIT
			MyWaterSS_EchoCtrlerSM_State = 0;//����״̬Ϊ0
			MyWaterSS_EchoCtrlerSM_Ctrl = 0;//�ر�EchoCtrler״̬��
		}
	}
}

void MyWaterSS_CollectEchoSpan(void) {
	static uint8_t NumCount;
	static uint16_t CollectNums[3];
	if(NumCount < 3) {	//��NumCount�ӵ�3�������ټ�
		NumCount ++;
	}
	MyArray_TailAdd_uint16_t(CollectNums, MyWaterSS_CountEchoSpan, 3);
	MyWaterSS_CountEchoSpanFiltered= MyArray_GetAverage_uint16_t(NumCount, CollectNums);//�������extern����

	WaterSD = MyWaterSS_GetResult_mm();
}

uint16_t MyWaterSS_GetResult_mm(void) {
	return (float)MyWaterSS_CountEchoSpanFiltered  * 0.1715;
}
