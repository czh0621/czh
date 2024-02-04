#include <cstdlib>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <sstream>


#include "../../../src/core_util/limit_filter/limiter.h"


using namespace czh::limiter;

void test_simple_limit()
{
    auto limit = std::make_shared<SimpleLimit>();
    limit->set_threshold(std::chrono::milliseconds(1000), 2);
    // 单线程测试
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    for (int i = 0; i < 10; i++) {

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        if (limit->try_pass()) {
            spdlog::debug("can pass {} thread:{}", i, ss.str());
        }
        else {
            spdlog::error("reject {} thread:{}", i, ss.str());
        }
    }
    // 多线程测试
    for (int i = 0; i < 10; i++) {
        std::thread t([limit, i]() {
            if (i > 5) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            if (limit->try_pass()) {
                spdlog::debug("can pass {}", i);
            }
            else {
                spdlog::error("reject {}", i);
            }
        });
        t.detach();
    }
}

void test_slide_limit()
{
    int  qps           = 2;
    auto slide_limiter = std::make_shared<SlideWindowLimit>(qps, 1000, 10);
    int  count = 20, sleep = 300, success = count * sleep / 1000 * 2;
    int  num = 0;
    spdlog::debug(
        "当前QPS限制为:{},当前测试次数:{},间隔:{}ms,预计成功次数:{}", qps, count, sleep, success);
    for (int i = 0; i < count; i++) {
        //        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        if (slide_limiter->try_pass()) {
            spdlog::info("try_acquire success");
            num++;
        }
        else {
            spdlog::error("try_acquire fail");
        }
    }
    spdlog::info("total success:{}", num);
}


void test_slide_limit_acquire()
{
    int  qps           = 2;
    auto slide_limiter = std::make_shared<SlideWindowLimit>(qps, 1000, 10);
    int  count = 20, sleep = 300, success = count * sleep / 1000 * 2;
    int  num = 0;
    spdlog::debug(
        "当前QPS限制为:{},当前测试次数:{},间隔:{}ms,预计成功次数:{}", qps, count, sleep, success);
    for (int i = 0; i < count; i++) {
        //        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        if (slide_limiter->try_acqurie()) {
            spdlog::info("try_acquire success");
            num++;
        }
        else {
            spdlog::error("try_acquire fail");
        }
    }
    spdlog::info("total success:{}", num);
}



int main()
{
    spdlog::set_level(spdlog::level::debug);
    //    test_simple_limit();
    //    test_slide_limit();
    //    spdlog::info("------------------------------------");
    //    test_slide_limit_acquire();
    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}