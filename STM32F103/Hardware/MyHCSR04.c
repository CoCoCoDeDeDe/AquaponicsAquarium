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
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//错点：改回非PWM后要改AF为Out
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOTrig, PinTrig, Bit_RESET);
	
	//Serial_SendStringPacketV2(USART2, "MyHCSR04_Trig_Init_End\r\n");
}


void MyHCSR04_Echo_Init(void) {
	//RCC-GPIO, EXTI=====
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//EXTI虽然连接APB2总线，但是其RCC时钟无需手动开启
	
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
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//注意：小心混淆Rising和Falling
	EXTI_Init(&EXTI_InitStruct);
	
	//错点：遗漏NVIC
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
	MyHCSR04_TrigCtrler_Ctrl = 1;//批准MyHCSR04_TrigCtrler()
	MyHCSR04_TrigSETer_Ctrl = 1;//批准MyHCSR04_TrigSETer()
}
//TrigCtrler的信号周期会比TIM3_DIV100的晚1个TIM2中断周期

void MyHCSR04_TrigCtrler(void){//管理Trig信号的发送以及为接收Echo信号做准备
	static uint8_t TrigCtrlerCount2;//用于记录改函数执行次数
	if(MyHCSR04_TrigCtrler_Ctrl == 1){//需要获得MyTIM3_DIV100()下MyHCSR04_TrigCtrlerSwitchOn()批准
		MyHCSR04_TrigSETer();//拉高Trig,开始发送Trig信号
		TrigCtrlerCount2 ++;//来这里2个IT(10us)后可解锁下面if
		if(TrigCtrlerCount2 >= 2){//要在拉高Trig后过15us(15个TIM1IT周期)后拉低Trig
			MyHCSR04_TrigRESETer();//拉低Trig,结束发送Trig信号,配置接收Echo信号的准备工作
			MyHCSR04_TrigCtrler_Ctrl = 0;//复位信号发送完后不再重复
			TrigCtrlerCount2 = 0;//下个周期也要保证SET和RESET之间的间隔
		}
	}
}

void MyHCSR04_TrigSETer(void){//拉高Trig,开始发送Trig信号
	if(MyHCSR04_TrigSETer_Ctrl == 1){//如果允许发送1次复位信号
		GPIO_WriteBit(GPIOTrig,PinTrig,Bit_SET);//将Trig线置高电平
		MyHCSR04_TrigSETer_Ctrl = 0;//将Trig线置高电平1次后不再重复
	}
}

void MyHCSR04_TrigRESETer(void){//拉低Trig,结束发送Trig信号,配置接收Echo信号的准备工作
	GPIO_WriteBit(GPIOTrig,PinTrig,Bit_RESET);//完成Trig信号发送后恢复Trig线为低电平
	MyHCSR04_EchoCtrlerSM_Ctrl = 1;//允许EchoCtrlSM_Ctrl运行对应if的内容
	MyHCSR04_EchoCtrlerSM_State = 0;//防止bug,保证收Echo信号时EchoCtrl状态机状态为0
	MyHCSR04_SetEXITTrig(EXTI_Trigger_Rising);//保证接收Echo信号上升沿时EXTImode为Rising
}

void MyHCSR04_SetEXITTrig(EXTITrigger_TypeDef mode){
	EXTI_InitStruct.EXTI_Trigger=mode;//设置EXTI触发模式为mode
	EXTI_Init(&EXTI_InitStruct);//错点：遗漏：将mode配置通过Init应用
}

void MyHCSR04_EchoCtrlerSM(void){//计算Echo信号相当的CNT值
	static uint8_t MyHCSR04_EchoCtrlerSM_State;//状态机状态
	
	static uint32_t CountEchoStart;//存储Echo信号开始时CNT的数值
	
	if(MyHCSR04_EchoCtrlerSM_Ctrl == 1){
		
		//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_Ctrl_In\r\n");
		
		if(MyHCSR04_EchoCtrlerSM_State == 0) {//状态0时
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State0_In\r\n");
			
			CountEchoStart=TIM_GetCounter(TIM3);//记录Echo信号开始时CNT的数值,总周期短,不考虑TIM3重装
			MyHCSR04_SetEXITTrig(EXTI_Trigger_Falling);//设置下降沿触发EXIT
			MyHCSR04_EchoCtrlerSM_State = 1;//设置状态为1
		} else if(MyHCSR04_EchoCtrlerSM_State == 1) {//状态1时
			//Serial_SendStringPacketV2(USART2, "EchoCtrlerSM_State1_In\r\n");
			
			MyHCSR04_CountEchoSpan = TIM_GetCounter(TIM3) - CountEchoStart;//计算Echo信号相当的CNT值
			MyHCSR04_CollectEchoSpan();
			MyHCSR04_SetEXITTrig(EXTI_Trigger_Rising);//设置上升沿触发EXIT
			MyHCSR04_EchoCtrlerSM_State = 0;//设置状态为0
			MyHCSR04_EchoCtrlerSM_Ctrl = 0;//关闭EchoCtrler状态机
		}
	}
}

void MyHCSR04_CollectEchoSpan(void) {
	static uint8_t NumCount;
	static uint16_t CollectNums[3];
	if(NumCount < 3) {	//让NumCount加到3则往后不再加
		NumCount ++;
	}
	MyArray_TailAdd_uint16_t(CollectNums, MyHCSR04_CountEchoSpan, 3);
	MyHCSR04_CountEchoSpanFiltered= MyArray_GetAverage_uint16_t(NumCount, CollectNums);//结果存入extern变量
}

uint16_t MyHCSR04_GetResult_mm(void) {
	return (float)MyHCSR04_CountEchoSpanFiltered  * 0.1715;
}
