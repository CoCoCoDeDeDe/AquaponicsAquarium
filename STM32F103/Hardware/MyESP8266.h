#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "MySerial.h"
#include "Array.h"
#include "OLED.h"

void MyESP8266_TIM3DIVz(void);		//��ƵTIM3,����SenderSM
void MyESP8266_SenderSM(void);		//��MyESP8266_TIM3DIVz����
void MyESP8266_ReceiverSM(void);	//��TIM4_IRQHandler����

#endif
