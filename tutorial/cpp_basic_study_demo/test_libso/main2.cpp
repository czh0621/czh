//
// Created by chenzhihao on 2024/11/10.
//

#include "test_libso.h"
#include <dlfcn.h>
#include <spdlog/spdlog.h>

int main()
{
    // 动态加载动态库测试单例
    using test_libso_handle = test_libso& (*)();
    std::string dll_path    = "/home/czh/c_code/tmp/tmp.wGfbanx1We/product/lib/libtest_libso.so";
    void*       handle      = dlopen(dll_path.c_str(), RTLD_LAZY);
    if (!handle) {
        spdlog::error("load dll error dll_path:{}", dll_path);
        return -1;
    }
    test_libso_handle singleton_handle = (test_libso_handle)dlsym(handle, "get_instance");
    const auto*       dlsym_error      = dlerror();
    if (dlsym_error) {
        spdlog::error("Cannot load symbol 'hello': {}", dlsym_error);
        dlclose(handle);
        return -1;
    }
    auto& test_libso = singleton_handle();
    test_libso.say_hello();
    return 0;
}