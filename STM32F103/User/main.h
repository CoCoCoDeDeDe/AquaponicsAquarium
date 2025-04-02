#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"                  // Device header

#include <string.h>

#include "Array.h"

#include "stdlib.h"

#include "Delay.h"

#include "MyTest.h"

#include "OLED.h"

#include "MySerial.h"

#include "MyTIM.h"

#include "MyFeeder.h"
#include "MyWaterP.h"

#include "MyWaterSS.h"
#include "MyAirS.h"
#include "MyWaterTS.h"

#include "MyWaterQS.h"
#include "MySoilMS.h"
#include "MyIlluminationS.h"

#include "MyADCAndDMA.h"

#include "MyPlantGL.h"
#include "MyAirP.h"
#include "MyWaterH.h"

#include "MyAquariumL.h"

#include "Serial3.h"

/*定义字符串队列*/
//#define QUEUE_SIZE 3

//typedef struct {
//	char *data[QUEUE_SIZE];
//	int idx_front;
//	int idx_rear;
//	int count;
//} MsgQueue_t;

//void InitQueue(MsgQueue_t *q);
//static int8_t enqueue(MsgQueue_t* q, const char *str);
//char *dequeue(MsgQueue_t *q);

#endif
