/**
 * @file       test_basic.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#include <spdlog/spdlog.h>
void test_int()
{
    // int 有符号 根据平台来显示一般32位、64位
    // int32_t 有符号 指定32长度
    // size_t 无符号 根据平台来显示
    // long 有符号 根据平台来显示 32位等于int
    // long long 强指定64位 有符号

    spdlog::info("int {},int32_t {},size_t {},long {}, long long {}",
                 sizeof(int),
                 sizeof(int32_t),
                 sizeof(size_t),
                 sizeof(long),
                 sizeof(long long));
}


int main()
{
    test_int();
    return 0;
}