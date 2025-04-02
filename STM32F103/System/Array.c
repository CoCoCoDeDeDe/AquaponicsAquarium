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

/**
 * @brief	检查字符串 str 是否含子字符串 tgt_str （区分大小写）
 * @param	str 主字符串
 * @param	tgt_str 目标子字符串（非空）
 * @return	-1：参数错误，0：不包含，1：包含
 * @note	1. 空字符串 tgt_str 视为存在（返回1）
 * 			2. 嵌入式场景建议固定 str/tgt 最大长度防止溢出
 */
int8_t MyArray_IsContain(const char *str, const char *tgt_str) {
	
	if (str == NULL || tgt_str == NULL) return -1;
	
	if (*tgt_str == '\0') return 1;	//包含
	
	const size_t MAX_ACCEPT_LEN = 256;	//根据硬件限制设定
	size_t len_str		= strlen(str);		//size_t #include <stddef.h>
	size_t len_tgt_str	= strlen(tgt_str);	//不包括'\0'
	
	if (len_str > MAX_ACCEPT_LEN || len_tgt_str > MAX_ACCEPT_LEN) return -1;//长度超限
	
	if (len_str < len_tgt_str) return 0;
	
	for (size_t i = 0; i <= len_str - len_tgt_str; i++) {
		int8_t ismatch = 1;
		for (size_t j = 0; j < len_tgt_str; j++) {
			if (str[i + j] != tgt_str[j]) {
				ismatch = 0;
				break;
			}
		}
		if (ismatch) {
			return 1;	//包含
		}
	}
	return 0;	//不包含，没有找到
}

int8_t MyArray_memcpy_char(char *src, char *dest) {
	if (sizeof(dest) < sizeof(src)) {
		return 0;//ERROR
	}//目的数组要>=源数组
	memcpy(dest, src, strlen(src));
	return 1;
}

//找到字符串a中'\0'的位置idx_0，将字符串b填充到idx_0的位置
int8_t MyArray_Char_CopyBToATail(char *a, char *b, uint16_t a_capacity) {
	//当数组作为函数的参数传递会退化为指针，sizeof(数组名)得到的是指针变量大小，在32位系统中固定位4字节
	uint16_t idx_a_0 = strlen(a);
	uint16_t len_b = strlen(b);
	if(a_capacity - idx_a_0 < len_b) {
		return -1;	
	}
	for(uint8_t i = 0; b[i] != '\0'; i++) {
			a[idx_a_0 + i] = b[i];
		}
	return 1;
}

