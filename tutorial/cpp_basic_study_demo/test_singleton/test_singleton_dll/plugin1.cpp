//
// Created by chenzhihao on 2024/11/10.
//
#include "../test_non_template_singleton.h"
#include "../test_singleton.h"

extern "C" void f()
{
    spdlog::info("test_non_template_singleton addr:{}",
                 (int64_t)&test_non_template_singleton::instance());
    auto& s = Singleton<TestSingletonRef>::instance(1);
    spdlog::info("test_singleton addr:{} value:{}", (int64_t)&s, s.value());
}