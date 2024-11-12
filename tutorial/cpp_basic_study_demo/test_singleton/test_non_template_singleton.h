//
// Created by chenzhihao on 2024/11/10.
//

#ifndef CZH_TEST_NON_TEMPLATE_SINGLETON_H
#define CZH_TEST_NON_TEMPLATE_SINGLETON_H

#include <spdlog/spdlog.h>

class test_non_template_singleton
{
public:
    static test_non_template_singleton& instance();

private:
    test_non_template_singleton() { spdlog::info("test_non_template_singleton construct!"); }
    ~test_non_template_singleton()                                             = default;
    test_non_template_singleton(const test_non_template_singleton&)            = delete;
    test_non_template_singleton& operator=(const test_non_template_singleton&) = delete;
};


class test_singleton
{
public:
    static test_singleton& instance()
    {
        static test_singleton p_singleton;
        return p_singleton;
    };

private:
    test_singleton() { spdlog::info("test_singleton construct!"); }
    ~test_singleton()                                = default;
    test_singleton(const test_singleton&)            = delete;
    test_singleton& operator=(const test_singleton&) = delete;
};



#endif   // CZH_TEST_NON_TEMPLATE_SINGLETON_H
