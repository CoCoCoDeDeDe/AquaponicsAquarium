#ifndef __ARRAY_H
#define __ARRAY_H

#include "stm32f10x.h"                  // Device header

#include <string.h>

#include <stddef.h>

int Array_char_isEqual(char *a, char *b);//算'\0'

float MyArray_GetAverage_uint16_t(uint8_t n, uint16_t *arr);//取数组前n个元素平均数()

void MyArray_TailAdd_uint16_t(uint16_t *arr, uint16_t ele, uint8_t n);//数组尾部加入新元素并舍去第一个元素()

void MyArray_Copy_Uint16_t(uint16_t *arr1, uint16_t *arr2, uint8_t n);//复制数组1到数组2
void MyArray_Copy_Uint8_t(uint8_t *arr1, uint8_t *arr2, uint8_t n);//复制数组1到数组2

int8_t MyArray_IsContain(const char *str, const char *tgt_str);//判断数组str中是否有字符串tgt_str

int8_t MyArray_memcpy_char(char *src, char *dest);//复制数组src到数组dest，从0到strlen(src)

int8_t MyArray_Char_CopyBToATail(char *a, char *b, uint16_t a_capacity);//找到字符串a中'\0'的位置idx_0，将字符串b填充到idx_0的位置

#endif
