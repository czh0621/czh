/**
 * @file       test_key_word.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */
#include "const_static.h"
// extern void g_print();   // 编译报错！ static 隐藏函数
using namespace std;


void test_const_static()
{
    const_static::c_func();
    const_static test;
    //    g_print(); error
    g_func();
}

int main()
{
    test_const_static();
    return 0;
}
