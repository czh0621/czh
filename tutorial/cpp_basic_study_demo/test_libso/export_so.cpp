//
// Created by chenzhihao on 2024/11/10.
//
#include "export_so.h"
#include "../test_singleton/test_singleton.h"
test_libso& get_instance()
{
    return Singleton<test_libso>::instance();
}