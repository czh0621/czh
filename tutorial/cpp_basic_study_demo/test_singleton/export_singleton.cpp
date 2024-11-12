//
// Created by viruser on 2024/11/8.
//

#include "test_non_template_singleton.h"
#include "test_singleton.h"
#include <spdlog/spdlog.h>

extern "C" void test_singleton()
{
    auto& instance = Singleton<TestSingletonRef>::instance(1);
    spdlog::info("DLL instance num:{}", instance.value());
    instance.increase();
    spdlog::info("DLL instance increase num:{}", instance.value());
    auto& second = Singleton<TestSingletonRef>::instance(3);
    spdlog::info("DLL second instance increase num:{}", second.value());
    spdlog::info("DLL singleton 1 addr:{}, 2 addr:{}", (int64_t)&instance, (int64_t)&second);
}

extern "C" void test_non_template_singleton()
{
    auto& instance = test_non_template_singleton::instance();
    spdlog::info("DLL test_non_template_singleton addr:{}", (int64_t)&instance);
    auto& instance2 = test_singleton::instance();
    spdlog::info("DLL test_singleton addr:{}", (int64_t)&instance2);
}