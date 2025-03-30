#ifndef __MYESP8266_H
#define __MYESP8266_H

#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "MySerial.h"
#include "Array.h"
#include "OLED.h"

void MyESP8266_TIM3DIVz(void);		//分频TIM3,触发SenderSM
void MyESP8266_SenderSM(void);		//由MyESP8266_TIM3DIVz触发
void MyESP8266_ReceiverSM(void);	//由TIM4_IRQHandler触发

#endif
