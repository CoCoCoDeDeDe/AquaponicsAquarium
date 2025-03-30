#include "Array.h"


int Array_char_isEqual(char *a, char *b) {
	if(sizeof(a) != sizeof(b)) {//sizeof(arr)��'\0'
		return 0;
	} else {
		for(uint8_t i = 0 ; i < sizeof(a) ; i ++ ) {
			if(a[i] != b[i]) {
				return 0;
			} else {
				//nothing
			}
		}	//�����꣬����ÿ��Ԫ�ض���ͬ
		return 1;
	}
}

float MyArray_GetAverage_uint16_t(uint8_t n, uint16_t *arr) {
	uint32_t sum = 0;
	for(int i = 0; i < n; i++) {
		sum += arr[i];
	}
	return sum / (float)n;	//��㣺תnΪfloat��������Ϊ����
}

void MyArray_TailAdd_uint16_t(uint16_t *arr, uint16_t ele, uint8_t n) {
	for(int i = 0; i < n - 1; i ++) {
		arr[i] = arr[i + 1];
	}
	arr[n - 1] = ele;	//��㣺arr���һλ��arr[n - 1]
}

void MyArray_Copy_Uint16_t(uint16_t *arr1, uint16_t *arr2, uint8_t n) {
	for(uint8_t i = 0; i < n; i ++) arr2[i] = arr1[i];
}

void MyArray_Copy_Uint8_t(uint8_t *arr1, uint8_t *arr2, uint8_t n) {
	for(uint8_t i = 0; i < n; i ++) arr2[i] = arr1[i];
}

/**
 * @brief	����ַ��� str �Ƿ����ַ��� tgt_str �����ִ�Сд��
 * @param	str ���ַ���
 * @param	tgt_str Ŀ�����ַ������ǿգ�
 * @return	-1����������0����������1������
 * @note	1. ���ַ��� tgt_str ��Ϊ���ڣ�����1��
 * 			2. Ƕ��ʽ��������̶� str/tgt ��󳤶ȷ�ֹ���
 */
int8_t MyArray_IsContain(const char *str, const char *tgt_str) {
	
	if (str == NULL || tgt_str == NULL) return -1;
	
	if (*tgt_str == '\0') return 1;	//����
	
	const size_t MAX_ACCEPT_LEN = 256;	//����Ӳ�������趨
	size_t len_str		= strlen(str);		//size_t #include <stddef.h>
	size_t len_tgt_str	= strlen(tgt_str);	//������'\0'
	
	if (len_str > MAX_ACCEPT_LEN || len_tgt_str > MAX_ACCEPT_LEN) return -1;//���ȳ���
	
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
			return 1;	//����
		}
	}
	return 0;	//��������û���ҵ�
}

int8_t MyArray_memcpy_char(char *src, char *dest) {
	if (sizeof(dest) < sizeof(src)) {
		return 0;//ERROR
	}//Ŀ������Ҫ>=Դ����
	memcpy(dest, src, strlen(src));
	return 1;
}

//�ҵ��ַ���a��'\0'��λ��idx_0�����ַ���b��䵽idx_0��λ��
int8_t MyArray_Char_CopyBToATail(char *a, char *b, uint16_t a_capacity) {
	//��������Ϊ�����Ĳ������ݻ��˻�Ϊָ�룬sizeof(������)�õ�����ָ�������С����32λϵͳ�й̶�λ4�ֽ�
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

