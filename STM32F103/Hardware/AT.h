#ifndef __AT_H
#define __AT_H

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

#include "math.h"

int8_t AT_Test(void);
void AT_Init_Str(void);
int8_t AT_Report(void);

#endif
