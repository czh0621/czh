//
// Created by viruser on 2024/11/8.
//

#include "test_core.h"
#include <spdlog/spdlog.h>

extern "C" void test_singleton()
{
    auto& instance = Singleton<TestSingletonRef>::instance(1);
    spdlog::info("so instance num:{}", instance.value());
    instance.increase();
    spdlog::info("so instance increase num:{}", instance.value());
    auto& second = Singleton<TestSingletonRef>::instance(3);
    spdlog::info("so second instance increase num:{}", instance.value());
}