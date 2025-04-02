#include "MyAirS.h"



static GPIO_InitTypeDef GPIO_InitStruct;//仅本文件内可访问
static EXTI_InitTypeDef EXTI_InitStruct;//仅本文件内可访问


uint8_t MyAirS_Flag_TIM3ARCounter_On = 0;
uint32_t MyAirS_Count_TIM3AR = 0;

uint8_t MyAirS_Flag_ReadCheckTimer_On  = 0;

uint8_t MyAirS_Flag_WriterSM_On = 0;
uint8_t MyAirS_State_WriterSM = 0;
uint32_t MyAirS_Count_WriterSM = 0;	//【【错点】Couter为uint8_t】

uint8_t MyAirS_BitsArr[40];//每1个元素对应要读取的数据的每一个bit
uint8_t MyAirS_DataArr[3];//每1个元素对于转换完的一个字节,不包括校验位
MyAirS_States_ReaderSM MyAirS_State_ReaderSM = IDLE;//默认未开始接收
uint32_t MyAirS_Count_ReadInterval = 0;//存储FALLING和RISING之间TIM3CNT计数次数
uint32_t MyAirS_Count_ReadIntervalStart = 0;//存储开始记FALLING和RISING之间TIM3CNT计数次数时的TIM3CNT值
uint8_t MyAirS_Flag_ReadSucced = 0;
uint8_t MyAirS_Count_ReadBit = 0;//在成功接收1Bit时++,在接收完40BIT或者开始接收时=0

uint8_t AirT = 0;

uint8_t AirH = 0;

void MyAirS_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
	
	PA4SET//DHT11的DATA总线默认高电平
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;//开机先发复位信号,关闭EXTI
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);//【错点】遗漏Init.

	MyAirS_SetMode_Write();
}

void MyAirS_EXTICmd(FunctionalState EXTI_LineCmd) {//开关PA4的EXTI
	EXTI_InitStruct.EXTI_LineCmd = EXTI_LineCmd;
	EXTI_Init(&EXTI_InitStruct);
}

void MyAirS_PA4GPIOModeConfig(GPIOMode_TypeDef GPIO_Mode) {//配置PA4的GPIO模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetPA4GPIOMode_Out_PP(void) {//配置PA4的GPIO模式为强推输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetPA4GPIOMode_IPU(void) {//配置PA4的GPIO模式为强推输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//接收信号选IPU,因DATA线默认高电平
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetMode_Write(void) {//设置为输出模式
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;//关EXTI
	EXTI_Init(&EXTI_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//设置强推输出
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetMode_Read(void) {//设置为输入模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//设置上拉输入,先关输出防止EXTI跳变
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//开EXTI
	EXTI_Init(&EXTI_InitStruct);
}

void MyAirS_SetEXTITrigRising(void) {
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
}

void MyAirS_SetEXTITrigFalling(void) {
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

// 记录TIM3的CNT重装次数,开始读取数据前默认不记录
void MyAirS_Count_TIM3ARer(void) {
	if(MyAirS_Flag_TIM3ARCounter_On == 1) {
		MyAirS_Count_TIM3AR ++;
		//为每次计算间隔提供从0开始的计TIM1AR次数
	}
}

void MyAirS_SwitchOn(void) {
	MyAirS_Flag_WriterSM_On  = SET;
	MyAirS_State_WriterSM = 0;
	MyAirS_SetMode_Write();//设PA4为输出模式
	MyAirS_State_ReaderSM = IDLE;//防止电平不稳定误入ReaderSM
	
	MyAirS_ReaderSM_PrePrepare();
	
	MyAirS_Flag_ReadCheckTimer_On = SET;//定时3000us后运行1次ReadCheck
}
//当触发了复位信号发射后就不用定时处理了，而是用EXTI检测电平变化并用TIM3_GetCounter记录时间.
//用TIM3_GetCounter为DHT11读取数据记录时间需考虑重装,因TCKCNT<1整次收发最长时间

//由MyAirS_State_WriterSM:2在决定启动Read时
//允许启动该函数.该函数主要内容需要在一个检测
//周期内,MyTIM3_DIV100主要内容运行后
//>=(25,050us+1个TIM2TAR)时运行一次,
//然后reset,然后待在下一轮被启动.
//该函数第一次运行主要内容时是一个检测周期内约
//10,000us,因10,000us前MyTIM3_DIV()运行了1次,
//启动了Writer,Writer启动了该函数主要内容仍在该
//10,000us内.现第1次运行主要内容,要在第3次运行该
//主要内容时(30,000us)启动ReadCheck,并在检查完
//后复位该函数状态以等待新周期
void MyAirS_ReadCheckTimer(void) {
	
	static uint8_t Count;
	
	if(MyAirS_Flag_ReadCheckTimer_On  == 1) {
		
		Count ++;
		//第1次: 10,000us;
		//第2次: 20,000us;
		//第3次: 30,000us.
		
		if(Count >= 4) {
			
			MyAirSReadCheck();
			
			Count = 0;
			
			MyAirS_Flag_ReadCheckTimer_On  = 0;
			
			//Serial_SendStringV2(USART2, "RCC\r\n");
		}
	}
}

//检查Read结束信号成功置的标志位
//来判断整个收发周期是否成功结束.
//目前功能较简单,相当于定时重置迎接新检测周期
void MyAirSReadCheck(void) {
	
	if(MyAirS_Flag_ReadSucced == 1) {
		
		MyAirS_ReadSucced();//成功
		
		//Serial_SendStringV2(USART2, "RCS\r\n");
		
	} else if(MyAirS_Flag_ReadSucced == 0) {
		
		MyAirS_ReadFailed();//失败
		
		//Serial_SendStringV2(USART2, "RCF\r\n");
		//Serial_SendStringV2(USART2, "\r\nFailed\r\n");
	}
}

//成功后的处理;定时重置以迎接新周期
void MyAirS_ReadSucced(void) {
	
	MyAirS_Reset();
}

//失败后的处理;定时重置以迎接新周期
void MyAirS_ReadFailed(void) {
	
	MyAirS_Reset();
	
	//Serial_SendStringV2(USART2, "RF\r\n");
}

//失败后的处理;定时重置以迎接新周期
void MyAirS_Reset(void) {
	
	MyAirS_SetMode_Read();//设置PA4为输出模式,准备新周期发复位信号,和防止EXTI误触发
	
	MyAirS_State_ReaderSM = IDLE;//设置接收状态
	
	MyAirS_Flag_TIM3ARCounter_On = RESET;//关闭ReadAR计次
	
	MyAirS_Flag_ReadSucced = RESET;//置Read失败标志位
}

void MyAirS_ReaderSM_PrePrepare(void) {//在周期最开始对Read的Pre准备
	
	MyAirS_Count_ReadBit = 0;//从0开始数收到的Bits
	
	MyAirS_SetEXTITrigFalling();//只测下降沿,除了END_END上升沿
}
//优化方案:将Read状态下的初始化代码转移到Write状态时或Write状态前对时序要求低的地方

void MyAirS_WriterSM(void) {
	
	if(MyAirS_Flag_WriterSM_On  == 1) {//当开启写入复位信号时才运行函数主要内容
		
		MyAirS_Count_WriterSM ++;
		
		switch(MyAirS_State_WriterSM) {
			
			case 0:
				
				MyAirS_SetMode_Write();//设PA4为输出模式,准备新周期发复位信号,和防止EXTI误触发
			
				PA4RESET//拉低电平
			
				MyAirS_State_WriterSM = 1;//下次来本函数进状态1
			
				MyAirS_Count_WriterSM = 0;//进入S1前重新计时,记录低电平持续时间
				
				//Serial_SendStringV2(USART2, "0");
				//Serial_SendByte(USART2, '0');
				
				break;
			case 1://持续20ms低电平,后拉高电平30us
				if(MyAirS_Count_WriterSM * 5 >= 20000) {//等持续20ms低电平后
					
					MyAirS_SetMode_Read();
					
					MyAirS_State_ReaderSM = RSP_START;
					
					MyAirS_SetEXTITrigFalling();
					
					MyAirS_State_WriterSM = 0;//下次来本函数进状态2
					
					MyAirS_Flag_WriterSM_On= 0;//关闭Writer
					
					//Serial_SendStringV2(USART2, "WriterSM_State: 1 in\r\n");
				}
				//Serial_SendStringV2(USART2, "WriterSM_State: 1 out\r\n");
				break;
		}
	}
}

void MyAirS_ReaderSM(void) {
	
	//Serial_SendStringV2(USART2, "S");
	
	switch (MyAirS_State_ReaderSM) {
		
		case IDLE:
			//空闲,未经允许不响应EXTI触发
			//该状态转出由其他函数控制
		
			//Serial_SendStringV2(USART2, "I");
			//Serial_SendByte(USART2, 'I');
			break;
		
		case RSP_START:
			
			MyAirS_Flag_TIM3ARCounter_On = SET;
			
			MyAirS_Count_ReadInterval_Start();
			
			MyAirS_State_ReaderSM = RSP_END;
			
			//Serial_SendStringV2(USART2, "F1);
			//Serial_SendByte(USART2, 'R');
			break;
			
		case RSP_END:
			
			MyAirS_Count_ReadInterval_Save();
		
			if (100 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 180) {
				
				MyAirS_State_ReaderSM = BIT_END;//设状态
				
				MyAirS_Count_ReadInterval_Start();
				
				//Serial_SendByte(USART2, 'R');
			} else {
				
				MyAirS_ReadFailed();
				//Serial_SendByte(USART2, 'f');
			}
			//Serial_SendStringV2(USART2, "F2");
			break;
			
		case BIT_END:
			
			MyAirS_Count_ReadInterval_Save();//获取并存储IntervalCount
			
			//Serial_SendByte(USART2, 'B');
		
			if(60 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 90) {
					
					MyAirS_Count_ReadInterval_Start();//【错点】误写在if(40<=MyAirS_Count_ReadBit){}
					MyAirS_BitsArr[MyAirS_Count_ReadBit ++] = 0;
					//Serial_SendByte(USART2, '0');
					
					if(40 <= MyAirS_Count_ReadBit) {
						
						MyAirS_State_ReaderSM = END_END;//转状态
						
						MyAirS_SetEXTITrigRising();
					}
				} else if(110 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval  <= 130) {//写的代码多了,但运行中要执行的代码少了
					
					MyAirS_Count_ReadInterval_Start();//【错点】误写在if(40<=MyAirS_Count_ReadBit){}
					MyAirS_BitsArr[MyAirS_Count_ReadBit ++] = 1;
					//Serial_SendByte(USART2, '1');
					
					if(40 <= MyAirS_Count_ReadBit) {
						
						MyAirS_State_ReaderSM = END_END;//转状态
						
						MyAirS_SetEXTITrigRising();
					}
				} else {//不在有效范围内
					//Serial_SendByte(USART2, '2');
					
					MyAirS_ReadFailed();
					
				}
			
			break;
			
		case END_END:
			
			MyAirS_Count_ReadInterval_Save();//获取并存储IntervalCount
		
			if (40 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 60) {//【错点】表示属于[40, 60],中间用&&(//且)
				
				//Serial_SendStringV2(USART2, "s");
				
				MyAirS_Flag_ReadSucced = 1;//置Read成功标志位,reset由ReadCheck执行
				
				MyAirS_DataTranslate();//翻译数据
				
			} else {
				
				MyAirS_ReadFailed();//Read失败程序
			}
			//Serial_SendStringV2(USART2, "ER");
			break;
			
		default:
			//如果使用某枚举类型变量作为switch表达式,则switch要么为该枚举类型所有的枚举变量都写一个case,要么就要加上default.default:后若没有要执行的语句则要加上break;
			
			//Serial_SendStringV2(USART2, "d");
			break;
	}
}

void MyAirS_Count_ReadInterval_Start(void) {
	
	MyAirS_Count_ReadIntervalStart= TIM_GetCounter(TIM3);//存当下CNT用于计算CNT变化
	
	MyAirS_Count_TIM3AR = 0; //重置TIM3的AR次数,只记本Interval内的AR次数
}

void MyAirS_Count_ReadInterval_Save(void) {
	
	//MyAirS_Count_ReadInterval = 10000 - MyAirS_Count_ReadIntervalStart+ TIM_GetCounter(TIM3) + 10000 * (MyAirS_Count_TIM3AR - 1);
	if(MyAirS_Count_TIM3AR == 0) {
		MyAirS_Count_ReadInterval = TIM_GetCounter(TIM3) - MyAirS_Count_ReadIntervalStart;
	} else {//MyAirS_Count_TIM3AR最多为1
		MyAirS_Count_ReadInterval = TIM_GetCounter(TIM3) + 10000 - MyAirS_Count_ReadIntervalStart;
	}
	
	//Serial_SendByte(USART2, 'i');
	//Serial_SendInteger(USART2, MyAirS_Count_ReadInterval);
	
}

//读取数据到MyAirS_BitsArr[40]时,每个Byte是从高到低对应[i]到[i + 7]
void MyAirS_DataTranslate(void) {
	
	uint8_t DataArrUncheck[5] ={0, 0, 0, 0, 0};//后续通过'|='获取bit要求被'|='的数每个位全为0
	
	uint8_t BitCount = 0;//每次进该函数都刷新,用于遍历40个bit
	
	uint8_t Bit = 0;//用于暂存bit
	
	uint8_t Sum = 0;//用于暂存DataArrUncheck[]前4个元素的和,用于校验
	
	for(uint8_t i = 0; i <= 4; i++) {//循环5次,遍历DataArrUnckeck[]每个元素
		
		for(uint8_t j = 0; j <=7; j++) {//循环8次,遍历DataArrUncheck[i]每个位
			
			Bit = MyAirS_BitsArr[BitCount];
			
			DataArrUncheck[i] |= (Bit << (7 - j));//将Bit(//0或1)与DataArrUncheck[i]的第j位进行'|',因DAU以及提前全赋值0,而且每次都是'|='不同的位,所以Bit每次是与0进行'|',结果取决于Bit的值
			
			BitCount ++;//每次获取完 MyAirS_BitsArr[]的一个元素后要获取下一个
		}
	}
	//校验,判断{复制;失败} 
	for(uint8_t i = 0; i <= 3; i ++) {//循环4次,遍历DataArrUnckeck[]前4个元素
		
		Sum += DataArrUncheck[i];
	}
	if(Sum == DataArrUncheck[4]) {
		
		for(uint8_t i = 0; i <= 3; i ++) {
			
			MyAirS_DataArr[i] = DataArrUncheck[i];
		}
		AirT = MyAirS_DataArr[2];
		
		AirH = MyAirS_DataArr[0];
		
	} else{
		
		MyAirS_ReadFailed();//Read失败程序
	}
}






