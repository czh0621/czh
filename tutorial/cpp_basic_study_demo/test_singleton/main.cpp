//
// Created by chenzhihao on 2024/11/10.
//

#include "test_non_template_singleton.h"
#include "test_singleton.h"
#include <dlfcn.h>
#include <spdlog/spdlog.h>

void test_instance()
{
    // 普通单例
    auto& instance = Singleton<TestSingletonRef>::instance(1);
    spdlog::info("instance num:{}", instance.value());
    instance.increase();
    spdlog::info("instance increase num:{}", instance.value());
    auto& second = Singleton<TestSingletonRef>::instance(3);
    spdlog::info("second instance increase num:{}", second.value());
    if (&instance == &second) {
        spdlog::info("is singleton 1 addr:{}, 2 addr:{}", (int64_t)&instance, (int64_t)&second);
    }
    spdlog::info("---------------------------------------");
    // 动态加载动态库测试单例
    using test_singleton_handle = void (*)();
    std::string dll_path        = "/home/czh/c_code/tmp/tmp.RwtOJBZBWC/product/lib/libsingleton.so";

    // std::string dll_path = "libsingleton.so";  动态库显示加载可以缩写路径 通过export
    // LD_LIBRARY_PATH=/tmp/czh/product/lib:$LD_LIBRARY_PATH 进当前窗口有效
    void* handle = dlopen(dll_path.c_str(), RTLD_NOW);
    if (!handle) {
        spdlog::error("load dll error dll_path:{}", dll_path);
        return;
    }
    test_singleton_handle singleton_handle = (test_singleton_handle)dlsym(handle, "test_singleton");
    const auto*           dlsym_error      = dlerror();
    if (dlsym_error) {
        spdlog::error("Cannot load symbol 'test_singleton': {}", dlsym_error);
        dlclose(handle);
        return;
    }
    singleton_handle();
    spdlog::info("---------------------------------------");

    auto& test_non_template_singleton = test_non_template_singleton::instance();
    auto& test_singleton              = test_singleton::instance();
    spdlog::info("---------------------------------------");
    test_singleton_handle test_non_template_singleton_handle =
        (test_singleton_handle)dlsym(handle, "test_non_template_singleton");
    const auto* dlsym_error2 = dlerror();
    if (dlsym_error2) {
        spdlog::error("Cannot load symbol 'test_non_template_singleton': {}", dlsym_error2);
        dlclose(handle);
        return;
    }
    test_non_template_singleton_handle();
    spdlog::info("---------------------------------------");
    spdlog::info("test_non_template_singleton addr:{} test_singleton addr:{}",
                 (int64_t)&test_non_template_singleton::instance(),
                 (int64_t)&test_singleton::instance());
}

int main()
{
    test_instance();
}
