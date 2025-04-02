#include "math.h"

float round_to_2decimal(float value) {
    // 核心逻辑：乘以100 + 0.5后取整，再除以100
    return (int)(value * 100 + 0.5f) / 100.0f;
}



