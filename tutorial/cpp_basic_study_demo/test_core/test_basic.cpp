/**
 * @file       test_basic.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */
#include "test_core.h"
#include <dlfcn.h>
#include <spdlog/spdlog.h>
void test_int()
{
    // void* 8
    // int 有符号 根据平台来显示一般32位 主要是兼容性 4
    // int32_t 有符号 指定32长度 4
    // size_t 无符号 根据平台来显示 8
    // long 有符号 根据平台来显示 8
    // long long 强指定64位 有符号 8

    spdlog::info("int {},int32_t {},size_t {},long {}, long long {} platform:{}",
                 sizeof(int),
                 sizeof(int32_t),
                 sizeof(size_t),
                 sizeof(long),
                 sizeof(long long),
                 sizeof(void*));
}

class Metrics
{
private:
    int m_value;

public:
    Metrics() = default;
    void set_value(int v) { m_value = v; }
    int  get_value() const { return m_value; }
};


void test_lambda()
{
    std::string s("111");
    auto        func = [s](int i) mutable {
        s = s + std::to_string(i);
        spdlog::info("{}", s);
    };
    func(2);

    Metrics m{};
    // auto    f1 = [](const Metrics& m) mutable { m.set_value(2); };
    auto f2 = [](const std::shared_ptr<Metrics>& ptr) mutable { ptr->set_value(2); };
}



void test_instance()
{
    // 普通单例
    auto& instance = Singleton<TestSingletonRef>::instance(1);
    spdlog::info("instance num:{}", instance.value());
    instance.increase();
    spdlog::info("instance increase num:{}", instance.value());
    auto& second = Singleton<TestSingletonRef>::instance(3);
    spdlog::info("second instance increase num:{}", instance.value());
    // 动态库测试单例
    using test_singleton_handle            = void (*)();
    void*                 handle           = dlopen("./singleton.so", RTLD_LAZY);
    test_singleton_handle singleton_handle = (test_singleton_handle)dlsym(handle, "test_singleton");
    singleton_handle();
}




int main()
{
    //    test_int();
    //    test_lambda();
    test_instance();
    return 0;
}