#ifndef __ARRAY_H
#define __ARRAY_H

#include "stm32f10x.h"                  // Device header

#include <string.h>

#include <stddef.h>

int Array_char_isEqual(char *a, char *b);//��'\0'

float MyArray_GetAverage_uint16_t(uint8_t n, uint16_t *arr);//ȡ����ǰn��Ԫ��ƽ����()

void MyArray_TailAdd_uint16_t(uint16_t *arr, uint16_t ele, uint8_t n);//����β��������Ԫ�ز���ȥ��һ��Ԫ��()

void MyArray_Copy_Uint16_t(uint16_t *arr1, uint16_t *arr2, uint8_t n);//��������1������2
void MyArray_Copy_Uint8_t(uint8_t *arr1, uint8_t *arr2, uint8_t n);//��������1������2

int8_t MyArray_IsContain(const char *str, const char *tgt_str);//�ж�����str���Ƿ����ַ���tgt_str

int8_t MyArray_memcpy_char(char *src, char *dest);//��������src������dest����0��strlen(src)

#endif
