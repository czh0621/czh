/**
 * @file       test_memory_leak.cpp
 * @date       2023/12/22
 * @since      2023-12-22
 * @author     czh
 */

#include <condition_variable>
#include <mutex>
#include <spdlog/spdlog.h>

class Allocate
{
    char s[32];

public:
    ~Allocate() { spdlog::info("~Allocate"); }
};


void test_allocate()
{
    Allocate* m = new Allocate();
    delete m;
    delete m;
}



int main()
{
    test_allocate();
    std::mutex                   mutex;
    std::unique_lock<std::mutex> l(mutex);
    std::condition_variable      condition_var;
    //    condition_var.wait(l);
}