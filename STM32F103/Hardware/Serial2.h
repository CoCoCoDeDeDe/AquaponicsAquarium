#ifndef __SERIAL2_H
#define __SERIAL2_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>

void Serial2_Init_All(void);
void Serial2_SendByte(char byte);
int8_t Serial2_SendString(char *msg, uint32_t len);





#endif
