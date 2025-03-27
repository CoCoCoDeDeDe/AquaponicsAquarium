#include "MyDHT11.h"



static GPIO_InitTypeDef GPIO_InitStruct;//�����ļ��ڿɷ���
static EXTI_InitTypeDef EXTI_InitStruct;//�����ļ��ڿɷ���


uint8_t MyDHT11_Flag_TIM3ARCounter_On = 0;
uint32_t MyDHT11_Count_TIM3AR = 0;

uint8_t MyDHT11_Flag_ReadCheckTimer_On  = 0;

uint8_t MyDHT11_Flag_WriterSM_On = 0;
uint8_t MyDHT11_State_WriterSM = 0;
uint32_t MyDHT11_Count_WriterSM = 0;	//������㡿CouterΪuint8_t��

uint8_t MyDHT11_BitsArr[40];//ÿ1��Ԫ�ض�ӦҪ��ȡ�����ݵ�ÿһ��bit
uint8_t MyDHT11_DataArr[3];//ÿ1��Ԫ�ض���ת�����һ���ֽ�,������У��λ
MyDHT11_States_ReaderSM MyDHT11_State_ReaderSM = IDLE;//Ĭ��δ��ʼ����
uint32_t MyDHT11_Count_ReadInterval = 0;//�洢FALLING��RISING֮��TIM3CNT��������
uint32_t MyDHT11_Count_ReadIntervalStart = 0;//�洢��ʼ��FALLING��RISING֮��TIM3CNT��������ʱ��TIM3CNTֵ
uint8_t MyDHT11_Flag_ReadSucced = 0;
uint8_t MyDHT11_Count_ReadBit = 0;//�ڳɹ�����1Bitʱ++,�ڽ�����40BIT���߿�ʼ����ʱ=0

void MyDHT11_Init(void) {
	
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

	MyDHT11_SetMode_Write();
}

void MyDHT11_EXTICmd(FunctionalState EXTI_LineCmd) {//����PA4��EXTI
	EXTI_InitStruct.EXTI_LineCmd = EXTI_LineCmd;
	EXTI_Init(&EXTI_InitStruct);
}

void MyDHT11_PA4GPIOModeConfig(GPIOMode_TypeDef GPIO_Mode) {//����PA4��GPIOģʽ
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyDHT11_SetPA4GPIOMode_Out_PP(void) {//����PA4��GPIOģʽΪǿ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyDHT11_SetPA4GPIOMode_IPU(void) {//����PA4��GPIOģʽΪǿ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//�����ź�ѡIPU,��DATA��Ĭ�ϸߵ�ƽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyDHT11_SetMode_Write(void) {//����Ϊ���ģʽ
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;//��EXTI
	EXTI_Init(&EXTI_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//����ǿ�����
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MyDHT11_SetMode_Read(void) {//����Ϊ����ģʽ
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//������������,�ȹ������ֹEXTI����
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//��EXTI
	EXTI_Init(&EXTI_InitStruct);
}

void MyDHT11_SetEXTITrigRising(void) {
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
}

void MyDHT11_SetEXTITrigFalling(void) {
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

// ��¼TIM3��CNT��װ����,��ʼ��ȡ����ǰĬ�ϲ���¼
void MyDHT11_Count_TIM3ARer(void) {
	if(MyDHT11_Flag_TIM3ARCounter_On == 1) {
		MyDHT11_Count_TIM3AR ++;
		//Ϊÿ�μ������ṩ��0��ʼ�ļ�TIM1AR����
	}
}

void MyDHT11_SwitchOn(void) {
	MyDHT11_Flag_WriterSM_On  = SET;
	MyDHT11_State_WriterSM = 0;
	MyDHT11_SetMode_Write();//��PA4Ϊ���ģʽ
	MyDHT11_State_ReaderSM = IDLE;//��ֹ��ƽ���ȶ�����ReaderSM
	
	MyDHT11_ReaderSM_PrePrepare();
	
	MyDHT11_Flag_ReadCheckTimer_On = SET;//��ʱ3000us������1��ReadCheck
}
//�������˸�λ�źŷ����Ͳ��ö�ʱ�����ˣ�������EXTI����ƽ�仯����TIM3_GetCounter��¼ʱ��.
//��TIM3_GetCounterΪDHT11��ȡ���ݼ�¼ʱ���迼����װ,��TCKCNT<1�����շ��ʱ��

//��MyDHT11_State_WriterSM:2�ھ�������Readʱ
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
void MyDHT11_ReadCheckTimer(void) {
	
	static uint8_t Count;
	
	if(MyDHT11_Flag_ReadCheckTimer_On  == 1) {
		
		Count ++;
		//��1��: 10,000us;
		//��2��: 20,000us;
		//��3��: 30,000us.
		
		if(Count >= 4) {
			
			MyDHT11ReadCheck();
			
			Count = 0;
			
			MyDHT11_Flag_ReadCheckTimer_On  = 0;
			
			//Serial_SendStringPacketV2(USART2, "RCC\r\n");
		}
	}
}

//���Read�����źųɹ��õı�־λ
//���ж������շ������Ƿ�ɹ�����.
//Ŀǰ���ܽϼ�,�൱�ڶ�ʱ����ӭ���¼������
void MyDHT11ReadCheck(void) {
	
	if(MyDHT11_Flag_ReadSucced == 1) {
		
		MyDHT11_ReadSucced();//�ɹ�
		
		//Serial_SendStringPacketV2(USART2, "RCS\r\n");
		
	} else if(MyDHT11_Flag_ReadSucced == 0) {
		
		MyDHT11_ReadFailed();//ʧ��
		
		//Serial_SendStringPacketV2(USART2, "RCF\r\n");
		//Serial_SendStringPacketV2(USART2, "\r\nFailed\r\n");
	}
}

//�ɹ���Ĵ���;��ʱ������ӭ��������
void MyDHT11_ReadSucced(void) {
	
	MyDHT11_Reset();
}

//ʧ�ܺ�Ĵ���;��ʱ������ӭ��������
void MyDHT11_ReadFailed(void) {
	
	MyDHT11_Reset();
	
	//Serial_SendStringPacketV2(USART2, "RF\r\n");
}

//ʧ�ܺ�Ĵ���;��ʱ������ӭ��������
void MyDHT11_Reset(void) {
	
	MyDHT11_SetMode_Read();//����PA4Ϊ���ģʽ,׼�������ڷ���λ�ź�,�ͷ�ֹEXTI�󴥷�
	
	MyDHT11_State_ReaderSM = IDLE;//���ý���״̬
	
	MyDHT11_Flag_TIM3ARCounter_On = RESET;//�ر�ReadAR�ƴ�
	
	MyDHT11_Flag_ReadSucced = RESET;//��Readʧ�ܱ�־λ
}

void MyDHT11_ReaderSM_PrePrepare(void) {//�������ʼ��Read��Pre׼��
	
	MyDHT11_Count_ReadBit = 0;//��0��ʼ���յ���Bits
	
	MyDHT11_SetEXTITrigFalling();//ֻ���½���,����END_END������
}
//�Ż�����:��Read״̬�µĳ�ʼ������ת�Ƶ�Write״̬ʱ��Write״̬ǰ��ʱ��Ҫ��͵ĵط�

void MyDHT11_WriterSM(void) {
	
	if(MyDHT11_Flag_WriterSM_On  == 1) {//������д�븴λ�ź�ʱ�����к�����Ҫ����
		
		MyDHT11_Count_WriterSM ++;
		
		switch(MyDHT11_State_WriterSM) {
			
			case 0:
				
				MyDHT11_SetMode_Write();//��PA4Ϊ���ģʽ,׼�������ڷ���λ�ź�,�ͷ�ֹEXTI�󴥷�
			
				PA4RESET//���͵�ƽ
			
				MyDHT11_State_WriterSM = 1;//�´�����������״̬1
			
				MyDHT11_Count_WriterSM = 0;//����S1ǰ���¼�ʱ,��¼�͵�ƽ����ʱ��
				
				//Serial_SendStringPacketV2(USART2, "0");
				//Serial_SendByte(USART2, '0');
				
				break;
			case 1://����20ms�͵�ƽ,�����ߵ�ƽ30us
				if(MyDHT11_Count_WriterSM * 5 >= 20000) {//�ȳ���20ms�͵�ƽ��
					
					MyDHT11_SetMode_Read();
					
					MyDHT11_State_ReaderSM = RSP_START;
					
					MyDHT11_SetEXTITrigFalling();
					
					MyDHT11_State_WriterSM = 0;//�´�����������״̬2
					
					MyDHT11_Flag_WriterSM_On= 0;//�ر�Writer
					
					//Serial_SendStringPacketV2(USART2, "WriterSM_State: 1 in\r\n");
				}
				//Serial_SendStringPacketV2(USART2, "WriterSM_State: 1 out\r\n");
				break;
		}
	}
}

void MyDHT11_ReaderSM(void) {
	
	//Serial_SendStringPacketV2(USART2, "S");
	
	switch (MyDHT11_State_ReaderSM) {
		
		case IDLE:
			//����,δ��������ӦEXTI����
			//��״̬ת����������������
		
			//Serial_SendStringPacketV2(USART2, "I");
			//Serial_SendByte(USART2, 'I');
			break;
		
		case RSP_START:
			
			MyDHT11_Flag_TIM3ARCounter_On = SET;
			
			MyDHT11_Count_ReadInterval_Start();
			
			MyDHT11_State_ReaderSM = RSP_END;
			
			//Serial_SendStringPacketV2(USART2, "F1);
			//Serial_SendByte(USART2, 'R');
			break;
			
		case RSP_END:
			
			MyDHT11_Count_ReadInterval_Save();
		
			if (100 <= MyDHT11_Count_ReadInterval && MyDHT11_Count_ReadInterval <= 180) {
				
				MyDHT11_State_ReaderSM = BIT_END;//��״̬
				
				MyDHT11_Count_ReadInterval_Start();
				
				//Serial_SendByte(USART2, 'R');
			} else {
				
				MyDHT11_ReadFailed();
				//Serial_SendByte(USART2, 'f');
			}
			//Serial_SendStringPacketV2(USART2, "F2");
			break;
			
		case BIT_END:
			
			MyDHT11_Count_ReadInterval_Save();//��ȡ���洢IntervalCount
			
			//Serial_SendByte(USART2, 'B');
		
			if(60 <= MyDHT11_Count_ReadInterval && MyDHT11_Count_ReadInterval <= 90) {
					
					MyDHT11_Count_ReadInterval_Start();//����㡿��д��if(40<=MyDHT11_Count_ReadBit){}
					MyDHT11_BitsArr[MyDHT11_Count_ReadBit ++] = 0;
					//Serial_SendByte(USART2, '0');
					
					if(40 <= MyDHT11_Count_ReadBit) {
						
						MyDHT11_State_ReaderSM = END_END;//ת״̬
						
						MyDHT11_SetEXTITrigRising();
					}
				} else if(110 <= MyDHT11_Count_ReadInterval && MyDHT11_Count_ReadInterval  <= 130) {//д�Ĵ������,��������Ҫִ�еĴ�������
					
					MyDHT11_Count_ReadInterval_Start();//����㡿��д��if(40<=MyDHT11_Count_ReadBit){}
					MyDHT11_BitsArr[MyDHT11_Count_ReadBit ++] = 1;
					//Serial_SendByte(USART2, '1');
					
					if(40 <= MyDHT11_Count_ReadBit) {
						
						MyDHT11_State_ReaderSM = END_END;//ת״̬
						
						MyDHT11_SetEXTITrigRising();
					}
				} else {//������Ч��Χ��
					//Serial_SendByte(USART2, '2');
					
					MyDHT11_ReadFailed();
					
				}
			
			break;
			
		case END_END:
			
			MyDHT11_Count_ReadInterval_Save();//��ȡ���洢IntervalCount
		
			if (40 <= MyDHT11_Count_ReadInterval && MyDHT11_Count_ReadInterval <= 60) {//����㡿��ʾ����[40, 60],�м���&&(//��)
				
				//Serial_SendStringPacketV2(USART2, "s");
				
				MyDHT11_Flag_ReadSucced = 1;//��Read�ɹ���־λ,reset��ReadCheckִ��
				
				MyDHT11_DataTranslate();//��������
				
			} else {
				
				MyDHT11_ReadFailed();//Readʧ�ܳ���
			}
			//Serial_SendStringPacketV2(USART2, "ER");
			break;
			
		default:
			//���ʹ��ĳö�����ͱ�����Ϊswitch���ʽ,��switchҪôΪ��ö���������е�ö�ٱ�����дһ��case,Ҫô��Ҫ����default.default:����û��Ҫִ�е������Ҫ����break;
			
			//Serial_SendStringPacketV2(USART2, "d");
			break;
	}
}

void MyDHT11_Count_ReadInterval_Start(void) {
	
	MyDHT11_Count_ReadIntervalStart= TIM_GetCounter(TIM3);//�浱��CNT���ڼ���CNT�仯
	
	MyDHT11_Count_TIM3AR = 0; //����TIM3��AR����,ֻ�Ǳ�Interval�ڵ�AR����
}

void MyDHT11_Count_ReadInterval_Save(void) {
	
	//MyDHT11_Count_ReadInterval = 10000 - MyDHT11_Count_ReadIntervalStart+ TIM_GetCounter(TIM3) + 10000 * (MyDHT11_Count_TIM3AR - 1);
	if(MyDHT11_Count_TIM3AR == 0) {
		MyDHT11_Count_ReadInterval = TIM_GetCounter(TIM3) - MyDHT11_Count_ReadIntervalStart;
	} else {//MyDHT11_Count_TIM3AR���Ϊ1
		MyDHT11_Count_ReadInterval = TIM_GetCounter(TIM3) + 10000 - MyDHT11_Count_ReadIntervalStart;
	}
	
	//Serial_SendByte(USART2, 'i');
	//Serial_SendInteger(USART2, MyDHT11_Count_ReadInterval);
	
}

//��ȡ���ݵ�MyDHT11_BitsArr[40]ʱ,ÿ��Byte�ǴӸߵ��Ͷ�Ӧ[i]��[i + 7]
void MyDHT11_DataTranslate(void) {
	
	uint8_t DataArrUncheck[5] ={0, 0, 0, 0, 0};//����ͨ��'|='��ȡbitҪ��'|='����ÿ��λȫΪ0
	
	uint8_t BitCount = 0;//ÿ�ν��ú�����ˢ��,���ڱ���40��bit
	
	uint8_t Bit = 0;//�����ݴ�bit
	
	uint8_t Sum = 0;//�����ݴ�DataArrUncheck[]ǰ4��Ԫ�صĺ�,����У��
	
	for(uint8_t i = 0; i <= 4; i++) {//ѭ��5��,����DataArrUnckeck[]ÿ��Ԫ��
		
		for(uint8_t j = 0; j <=7; j++) {//ѭ��8��,����DataArrUncheck[i]ÿ��λ
			
			Bit = MyDHT11_BitsArr[BitCount];
			
			DataArrUncheck[i] |= (Bit << (7 - j));//��Bit(//0��1)��DataArrUncheck[i]�ĵ�jλ����'|',��DAU�Լ���ǰȫ��ֵ0,����ÿ�ζ���'|='��ͬ��λ,����Bitÿ������0����'|',���ȡ����Bit��ֵ
			
			BitCount ++;//ÿ�λ�ȡ�� MyDHT11_BitsArr[]��һ��Ԫ�غ�Ҫ��ȡ��һ��
		}
	}
	//У��,�ж�{����;ʧ��} 
	for(uint8_t i = 0; i <= 3; i ++) {//ѭ��4��,����DataArrUnckeck[]ǰ4��Ԫ��
		
		Sum += DataArrUncheck[i];
	}
	if(Sum == DataArrUncheck[4]) {
		
		for(uint8_t i = 0; i <= 3; i ++) {
			
			MyDHT11_DataArr[i] = DataArrUncheck[i];
		}
	} else{
		
		MyDHT11_ReadFailed();//Readʧ�ܳ���
	}
}







