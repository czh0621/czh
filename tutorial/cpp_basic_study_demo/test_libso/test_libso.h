//
// Created by chenzhihao on 2024/11/10.
//

#ifndef CZH_TEST_LIBSO_H
#define CZH_TEST_LIBSO_H

#include <spdlog/spdlog.h>

class test_libso
{
public:
    test_libso() { spdlog::info("test_libso construct"); }
    void say_hello();
};



#endif   // CZH_TEST_LIBSO_H
