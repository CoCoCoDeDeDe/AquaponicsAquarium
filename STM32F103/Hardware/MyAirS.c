#include "MyAirS.h"



static GPIO_InitTypeDef GPIO_InitStruct;//�����ļ��ڿɷ���
static EXTI_InitTypeDef EXTI_InitStruct;//�����ļ��ڿɷ���


uint8_t MyAirS_Flag_TIM3ARCounter_On = 0;
uint32_t MyAirS_Count_TIM3AR = 0;

uint8_t MyAirS_Flag_ReadCheckTimer_On  = 0;

uint8_t MyAirS_Flag_WriterSM_On = 0;
uint8_t MyAirS_State_WriterSM = 0;
uint32_t MyAirS_Count_WriterSM = 0;	//������㡿CouterΪuint8_t��

uint8_t MyAirS_BitsArr[40];//ÿ1��Ԫ�ض�ӦҪ��ȡ�����ݵ�ÿһ��bit
uint8_t MyAirS_DataArr[3];//ÿ1��Ԫ�ض���ת�����һ���ֽ�,������У��λ
MyAirS_States_ReaderSM MyAirS_State_ReaderSM = IDLE;//Ĭ��δ��ʼ����
uint32_t MyAirS_Count_ReadInterval = 0;//�洢FALLING��RISING֮��TIM3CNT��������
uint32_t MyAirS_Count_ReadIntervalStart = 0;//�洢��ʼ��FALLING��RISING֮��TIM3CNT��������ʱ��TIM3CNTֵ
uint8_t MyAirS_Flag_ReadSucced = 0;
uint8_t MyAirS_Count_ReadBit = 0;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

void MyAirS_Init(
	uint8_t NVIC_IRQChannelPreemptionPriority, 
	uint8_t NVIC_IRQChannelSubPriority) {
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
	
	PA4SET//DHT11��DATA����Ĭ�ϸߵ�ƽ
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;//�����ȷ���λ�ź�,�ر�EXTI
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);//����㡿��©Init.

	MyAirS_SetMode_Write();
}

void MyAirS_EXTICmd(FunctionalState EXTI_LineCmd) {//����PA4��EXTI
	EXTI_InitStruct.EXTI_LineCmd = EXTI_LineCmd;
	EXTI_Init(&EXTI_InitStruct);
}

void MyAirS_PA4GPIOModeConfig(GPIOMode_TypeDef GPIO_Mode) {//����PA4��GPIOģʽ
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetPA4GPIOMode_Out_PP(void) {//����PA4��GPIOģʽΪǿ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetPA4GPIOMode_IPU(void) {//����PA4��GPIOģʽΪǿ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//�����ź�ѡIPU,��DATA��Ĭ�ϸߵ�ƽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetMode_Write(void) {//����Ϊ���ģʽ
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;//��EXTI
	EXTI_Init(&EXTI_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//����ǿ�����
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyAirS_SetMode_Read(void) {//����Ϊ����ģʽ
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//������������,�ȹ������ֹEXTI����
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//��EXTI
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

// ��¼TIM3��CNT��װ����,��ʼ��ȡ����ǰĬ�ϲ���¼
void MyAirS_Count_TIM3ARer(void) {
	if(MyAirS_Flag_TIM3ARCounter_On == 1) {
		MyAirS_Count_TIM3AR ++;
		//Ϊÿ�μ������ṩ��0��ʼ�ļ�TIM1AR����
	}
}

void MyAirS_SwitchOn(void) {
	MyAirS_Flag_WriterSM_On  = SET;
	MyAirS_State_WriterSM = 0;
	MyAirS_SetMode_Write();//��PA4Ϊ���ģʽ
	MyAirS_State_ReaderSM = IDLE;//��ֹ��ƽ���ȶ�����ReaderSM
	
	MyAirS_ReaderSM_PrePrepare();
	
	MyAirS_Flag_ReadCheckTimer_On = SET;//��ʱ3000us������1��ReadCheck
}
//�������˸�λ�źŷ����Ͳ��ö�ʱ�����ˣ�������EXTI����ƽ�仯����TIM3_GetCounter��¼ʱ��.
//��TIM3_GetCounterΪDHT11��ȡ���ݼ�¼ʱ���迼����װ,��TCKCNT<1�����շ��ʱ��

//��MyAirS_State_WriterSM:2�ھ�������Readʱ
//���������ú���.�ú�����Ҫ������Ҫ��һ�����
//������,MyTIM3_DIV100��Ҫ�������к�
//>=(25,050us+1��TIM2TAR)ʱ����һ��,
//Ȼ��reset,Ȼ�������һ�ֱ�����.
//�ú�����һ��������Ҫ����ʱ��һ�����������Լ
//10,000us,��10,000usǰMyTIM3_DIV()������1��,
//������Writer,Writer�����˸ú�����Ҫ�������ڸ�
//10,000us��.�ֵ�1��������Ҫ����,Ҫ�ڵ�3�����и�
//��Ҫ����ʱ(30,000us)����ReadCheck,���ڼ����
//��λ�ú���״̬�Եȴ�������
void MyAirS_ReadCheckTimer(void) {
	
	static uint8_t Count;
	
	if(MyAirS_Flag_ReadCheckTimer_On  == 1) {
		
		Count ++;
		//��1��: 10,000us;
		//��2��: 20,000us;
		//��3��: 30,000us.
		
		if(Count >= 4) {
			
			MyAirSReadCheck();
			
			Count = 0;
			
			MyAirS_Flag_ReadCheckTimer_On  = 0;
			
			//Serial_SendStringPacketV2(USART2, "RCC\r\n");
		}
	}
}

//���Read�����źųɹ��õı�־λ
//���ж������շ������Ƿ�ɹ�����.
//Ŀǰ���ܽϼ�,�൱�ڶ�ʱ����ӭ���¼������
void MyAirSReadCheck(void) {
	
	if(MyAirS_Flag_ReadSucced == 1) {
		
		MyAirS_ReadSucced();//�ɹ�
		
		//Serial_SendStringPacketV2(USART2, "RCS\r\n");
		
	} else if(MyAirS_Flag_ReadSucced == 0) {
		
		MyAirS_ReadFailed();//ʧ��
		
		//Serial_SendStringPacketV2(USART2, "RCF\r\n");
		//Serial_SendStringPacketV2(USART2, "\r\nFailed\r\n");
	}
}

//�ɹ���Ĵ���;��ʱ������ӭ��������
void MyAirS_ReadSucced(void) {
	
	MyAirS_Reset();
}

//ʧ�ܺ�Ĵ���;��ʱ������ӭ��������
void MyAirS_ReadFailed(void) {
	
	MyAirS_Reset();
	
	//Serial_SendStringPacketV2(USART2, "RF\r\n");
}

//ʧ�ܺ�Ĵ���;��ʱ������ӭ��������
void MyAirS_Reset(void) {
	
	MyAirS_SetMode_Read();//����PA4Ϊ���ģʽ,׼�������ڷ���λ�ź�,�ͷ�ֹEXTI�󴥷�
	
	MyAirS_State_ReaderSM = IDLE;//���ý���״̬
	
	MyAirS_Flag_TIM3ARCounter_On = RESET;//�ر�ReadAR�ƴ�
	
	MyAirS_Flag_ReadSucced = RESET;//��Readʧ�ܱ�־λ
}

void MyAirS_ReaderSM_PrePrepare(void) {//�������ʼ��Read��Pre׼��
	
	MyAirS_Count_ReadBit = 0;//��0��ʼ���յ���Bits
	
	MyAirS_SetEXTITrigFalling();//ֻ���½���,����END_END������
}
//�Ż�����:��Read״̬�µĳ�ʼ������ת�Ƶ�Write״̬ʱ��Write״̬ǰ��ʱ��Ҫ��͵ĵط�

void MyAirS_WriterSM(void) {
	
	if(MyAirS_Flag_WriterSM_On  == 1) {//������д�븴λ�ź�ʱ�����к�����Ҫ����
		
		MyAirS_Count_WriterSM ++;
		
		switch(MyAirS_State_WriterSM) {
			
			case 0:
				
				MyAirS_SetMode_Write();//��PA4Ϊ���ģʽ,׼�������ڷ���λ�ź�,�ͷ�ֹEXTI�󴥷�
			
				PA4RESET//���͵�ƽ
			
				MyAirS_State_WriterSM = 1;//�´�����������״̬1
			
				MyAirS_Count_WriterSM = 0;//����S1ǰ���¼�ʱ,��¼�͵�ƽ����ʱ��
				
				//Serial_SendStringPacketV2(USART2, "0");
				//Serial_SendByte(USART2, '0');
				
				break;
			case 1://����20ms�͵�ƽ,�����ߵ�ƽ30us
				if(MyAirS_Count_WriterSM * 5 >= 20000) {//�ȳ���20ms�͵�ƽ��
					
					MyAirS_SetMode_Read();
					
					MyAirS_State_ReaderSM = RSP_START;
					
					MyAirS_SetEXTITrigFalling();
					
					MyAirS_State_WriterSM = 0;//�´�����������״̬2
					
					MyAirS_Flag_WriterSM_On= 0;//�ر�Writer
					
					//Serial_SendStringPacketV2(USART2, "WriterSM_State: 1 in\r\n");
				}
				//Serial_SendStringPacketV2(USART2, "WriterSM_State: 1 out\r\n");
				break;
		}
	}
}

void MyAirS_ReaderSM(void) {
	
	//Serial_SendStringPacketV2(USART2, "S");
	
	switch (MyAirS_State_ReaderSM) {
		
		case IDLE:
			//����,δ��������ӦEXTI����
			//��״̬ת����������������
		
			//Serial_SendStringPacketV2(USART2, "I");
			//Serial_SendByte(USART2, 'I');
			break;
		
		case RSP_START:
			
			MyAirS_Flag_TIM3ARCounter_On = SET;
			
			MyAirS_Count_ReadInterval_Start();
			
			MyAirS_State_ReaderSM = RSP_END;
			
			//Serial_SendStringPacketV2(USART2, "F1);
			//Serial_SendByte(USART2, 'R');
			break;
			
		case RSP_END:
			
			MyAirS_Count_ReadInterval_Save();
		
			if (100 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 180) {
				
				MyAirS_State_ReaderSM = BIT_END;//��״̬
				
				MyAirS_Count_ReadInterval_Start();
				
				//Serial_SendByte(USART2, 'R');
			} else {
				
				MyAirS_ReadFailed();
				//Serial_SendByte(USART2, 'f');
			}
			//Serial_SendStringPacketV2(USART2, "F2");
			break;
			
		case BIT_END:
			
			MyAirS_Count_ReadInterval_Save();//��ȡ���洢IntervalCount
			
			//Serial_SendByte(USART2, 'B');
		
			if(60 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 90) {
					
					MyAirS_Count_ReadInterval_Start();//����㡿��д��if(40<=MyAirS_Count_ReadBit){}
					MyAirS_BitsArr[MyAirS_Count_ReadBit ++] = 0;
					//Serial_SendByte(USART2, '0');
					
					if(40 <= MyAirS_Count_ReadBit) {
						
						MyAirS_State_ReaderSM = END_END;//ת״̬
						
						MyAirS_SetEXTITrigRising();
					}
				} else if(110 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval  <= 130) {//д�Ĵ������,��������Ҫִ�еĴ�������
					
					MyAirS_Count_ReadInterval_Start();//����㡿��д��if(40<=MyAirS_Count_ReadBit){}
					MyAirS_BitsArr[MyAirS_Count_ReadBit ++] = 1;
					//Serial_SendByte(USART2, '1');
					
					if(40 <= MyAirS_Count_ReadBit) {
						
						MyAirS_State_ReaderSM = END_END;//ת״̬
						
						MyAirS_SetEXTITrigRising();
					}
				} else {//������Ч��Χ��
					//Serial_SendByte(USART2, '2');
					
					MyAirS_ReadFailed();
					
				}
			
			break;
			
		case END_END:
			
			MyAirS_Count_ReadInterval_Save();//��ȡ���洢IntervalCount
		
			if (40 <= MyAirS_Count_ReadInterval && MyAirS_Count_ReadInterval <= 60) {//����㡿��ʾ����[40, 60],�м���&&(//��)
				
				//Serial_SendStringPacketV2(USART2, "s");
				
				MyAirS_Flag_ReadSucced = 1;//��Read�ɹ���־λ,reset��ReadCheckִ��
				
				MyAirS_DataTranslate();//��������
				
			} else {
				
				MyAirS_ReadFailed();//Readʧ�ܳ���
			}
			//Serial_SendStringPacketV2(USART2, "ER");
			break;
			
		default:
			//���ʹ��ĳö�����ͱ�����Ϊswitch���ʽ,��switchҪôΪ��ö���������е�ö�ٱ�����дһ��case,Ҫô��Ҫ����default.default:����û��Ҫִ�е������Ҫ����break;
			
			//Serial_SendStringPacketV2(USART2, "d");
			break;
	}
}

void MyAirS_Count_ReadInterval_Start(void) {
	
	MyAirS_Count_ReadIntervalStart= TIM_GetCounter(TIM3);//�浱��CNT���ڼ���CNT�仯
	
	MyAirS_Count_TIM3AR = 0; //����TIM3��AR����,ֻ�Ǳ�Interval�ڵ�AR����
}

void MyAirS_Count_ReadInterval_Save(void) {
	
	//MyAirS_Count_ReadInterval = 10000 - MyAirS_Count_ReadIntervalStart+ TIM_GetCounter(TIM3) + 10000 * (MyAirS_Count_TIM3AR - 1);
	if(MyAirS_Count_TIM3AR == 0) {
		MyAirS_Count_ReadInterval = TIM_GetCounter(TIM3) - MyAirS_Count_ReadIntervalStart;
	} else {//MyAirS_Count_TIM3AR���Ϊ1
		MyAirS_Count_ReadInterval = TIM_GetCounter(TIM3) + 10000 - MyAirS_Count_ReadIntervalStart;
	}
	
	//Serial_SendByte(USART2, 'i');
	//Serial_SendInteger(USART2, MyAirS_Count_ReadInterval);
	
}

//��ȡ���ݵ�MyAirS_BitsArr[40]ʱ,ÿ��Byte�ǴӸߵ��Ͷ�Ӧ[i]��[i + 7]
void MyAirS_DataTranslate(void) {
	
	uint8_t DataArrUncheck[5] ={0, 0, 0, 0, 0};//����ͨ��'|='��ȡbitҪ��'|='����ÿ��λȫΪ0
	
	uint8_t BitCount = 0;//ÿ�ν��ú�����ˢ��,���ڱ���40��bit
	
	uint8_t Bit = 0;//�����ݴ�bit
	
	uint8_t Sum = 0;//�����ݴ�DataArrUncheck[]ǰ4��Ԫ�صĺ�,����У��
	
	for(uint8_t i = 0; i <= 4; i++) {//ѭ��5��,����DataArrUnckeck[]ÿ��Ԫ��
		
		for(uint8_t j = 0; j <=7; j++) {//ѭ��8��,����DataArrUncheck[i]ÿ��λ
			
			Bit = MyAirS_BitsArr[BitCount];
			
			DataArrUncheck[i] |= (Bit << (7 - j));//��Bit(//0��1)��DataArrUncheck[i]�ĵ�jλ����'|',��DAU�Լ���ǰȫ��ֵ0,����ÿ�ζ���'|='��ͬ��λ,����Bitÿ������0����'|',���ȡ����Bit��ֵ
			
			BitCount ++;//ÿ�λ�ȡ�� MyAirS_BitsArr[]��һ��Ԫ�غ�Ҫ��ȡ��һ��
		}
	}
	//У��,�ж�{����;ʧ��} 
	for(uint8_t i = 0; i <= 3; i ++) {//ѭ��4��,����DataArrUnckeck[]ǰ4��Ԫ��
		
		Sum += DataArrUncheck[i];
	}
	if(Sum == DataArrUncheck[4]) {
		
		for(uint8_t i = 0; i <= 3; i ++) {
			
			MyAirS_DataArr[i] = DataArrUncheck[i];
		}
		AirT = MyAirS_DataArr[2];
		
		AirH = MyAirS_DataArr[0];
		
	} else{
		
		MyAirS_ReadFailed();//Readʧ�ܳ���
	}
}

uint8_t AirT = 0;

uint8_t AirH = 0;





