#include <random>
#include <chrono>
#include "size.h"
// 生成一个介于0和PAGE_SIZE之间的随机数（包含0但不包含PAGE_SIZE）
int generateRandomNumber() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    unsigned seed = static_cast<unsigned>(value.count());

    std::mt19937 gen(seed); // 使用 Mersenne Twister 引擎，设置种子为当前时间戳
    std::uniform_int_distribution<> dis(0, PAGE_SIZE - 1); // 定义均匀分布

    return dis(gen); // 生成随机数
}