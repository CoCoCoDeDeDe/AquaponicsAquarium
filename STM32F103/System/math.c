#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float round_to_2decimal(float value) {
    // 核心逻辑：乘以100 + 0.5后取整，再除以100
    return (int)(value * 100 + 0.5f) / 100.0f;
}

// 生成指定范围内步进的随机数的函数
int random_step(int min, int max, int step) {
    if (min > max || step <= 0) {
        return -1; // 处理无效输入
    }

    // 初始化随机数种子
    srand(time(NULL));

    // 计算可能的步数
    int num_steps = (max - min) / step;

    // 生成随机步数
    int random_steps = rand() % (num_steps + 1);

    // 计算随机数
    return min + random_steps * step;
}    

