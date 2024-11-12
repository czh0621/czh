//
// Created by chenzhihao on 2024/11/10.
//

#include "export_so.h"

void test_libso()
{
    auto& instance = get_instance();
    instance.say_hello();
};

int main()
{
    test_libso();
}