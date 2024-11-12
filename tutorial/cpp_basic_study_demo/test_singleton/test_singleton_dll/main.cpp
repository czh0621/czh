//
// Created by chenzhihao on 2024/11/10.
//

#include "dlfcn.h"
#include <spdlog/spdlog.h>

int main()
{
    using test_singleton_handle = void (*)();

    void* fd1 = dlopen("/home/czh/c_code/tmp/tmp.wGfbanx1We/product/lib/libplugin1.so", RTLD_LAZY);
    if (!fd1) {
        spdlog::info("{}", dlerror());
    }
    test_singleton_handle f1 = (test_singleton_handle)dlsym(fd1, "f");
    if (!f1) {
        spdlog::info("{}", dlerror());
    }
    f1();
    void* fd2 = dlopen("/home/czh/c_code/tmp/tmp.wGfbanx1We/product/lib/libplugin2.so", RTLD_LAZY);
    test_singleton_handle f2 = (test_singleton_handle)dlsym(fd2, "g");
    f2();
    return 0;
}