/**
 * @file       test_refercence.cpp
 * @date       2024/2/26
 * @since      2024-02-26
 * @author     czh
 */
#include <iostream>
#include <spdlog/spdlog.h>
#include <type_traits>
void test_lvalue()
{
    int&& a{1};   // a的type 类型为int&& 值类型为lvalue
    auto  ptr = &a;
    // &a 导致ptr为 prvalue 没有地址无法打印
    spdlog::info("int&& a = 1  ptr:{}", reinterpret_cast<void*>(ptr));
}

void test_xvalue()
{
    bool b{true};   // lvalue
    std::move(b);
}


int main()
{
    test_lvalue();
    return 0;
}