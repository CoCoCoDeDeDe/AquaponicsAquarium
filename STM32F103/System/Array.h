#ifndef __ARRAY_H
#define __ARRAY_H



int Array_char_isEqual(char *a, char *b);
//取数组前n个元素平均数()
float MyArray_GetAverage_uint16_t(uint8_t n, uint16_t *arr);
//数组尾部加入新元素并舍去第一个元素()
void MyArray_TailAdd_uint16_t(uint16_t *arr, uint16_t ele, uint8_t n);
//复制数组1到数组2
void MyArray_Copy_Uint16_t(uint16_t *arr1, uint16_t *arr2, uint8_t n);
void MyArray_Copy_Uint8_t(uint8_t *arr1, uint8_t *arr2, uint8_t n);
#endif
