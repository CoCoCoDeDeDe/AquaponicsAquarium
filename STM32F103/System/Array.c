#include "Array.h"


int Array_char_isEqual(char *a, char *b) {
	if(sizeof(a) != sizeof(b)) {//sizeof(arr)算'\0'
		return 0;
	} else {
		for(uint8_t i = 0 ; i < sizeof(a) ; i ++ ) {
			if(a[i] != b[i]) {
				return 0;
			} else {
				//nothing
			}
		}	//遍历完，发现每个元素都相同
		return 1;
	}
}

float MyArray_GetAverage_uint16_t(uint8_t n, uint16_t *arr) {
	uint32_t sum = 0;
	for(int i = 0; i < n; i++) {
		sum += arr[i];
	}
	return sum / (float)n;	//错点：转n为float，否则结果为整数
}

void MyArray_TailAdd_uint16_t(uint16_t *arr, uint16_t ele, uint8_t n) {
	for(int i = 0; i < n - 1; i ++) {
		arr[i] = arr[i + 1];
	}
	arr[n - 1] = ele;	//错点：arr最后一位是arr[n - 1]
}

void MyArray_Copy_Uint16_t(uint16_t *arr1, uint16_t *arr2, uint8_t n) {
	for(uint8_t i = 0; i < n; i ++) arr2[i] = arr1[i];
}

void MyArray_Copy_Uint8_t(uint8_t *arr1, uint8_t *arr2, uint8_t n) {
	for(uint8_t i = 0; i < n; i ++) arr2[i] = arr1[i];
}

int8_t MyArray_IsContain(const char *str, const char *tgt_str) {
	
	if (str == 0 || tgt_str == 0) return -1;
	
	if (*tgt_str == '\0') return 1;
	
	size_t len_str		= strlen(str);		//#include <stddef.h>
	size_t len_tgt_str	= strlen(tgt_str);	//不包括'\0'
	
	if (len_str < len_tgt_str) return 0;
	
	for (size_t i = 0; i <= len_str - len_tgt_str; i++) {
		size_t j;
		for (j = 0; j < len_tgt_str; j++) {
			if (str[i + j] != tgt_str[j]) break;
		}
		if (j == len_tgt_str) {
			return 1;
		}
	}
	return 0;
}

int8_t MyArray_memcpy_char(char *src, char *dest) {
	if (sizeof(dest) < sizeof(src)) {
		return 0;//ERROR
	}//目的数组要>=源数组
	memcpy(dest, src, strlen(src));
	return 1;
}



