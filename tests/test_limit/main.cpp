#include <cstdlib>
#include <iostream>

#include "../../src/limit/limit.h"

using namespace czh::rpc;

void test_simple_limit() {
    auto limit = std::make_shared<SimpleLimit>();
    limit->set_threshold(std::chrono::milliseconds(10), 5);
    limit->start();
    for (int i = 0; i < 10; i++) {
        std::thread t([limit, i]() {
            if (limit->try_pass()) {
                std::cout << "pass num: " << i << std::endl;
            } else {
                std::cout << "reject num: " << i << std::endl;
            }
        });
        t.detach();
    }
}

void test_slide_limit() {
    auto limit = std::make_shared<SlideWindowLimit>(50, 1000, 10);
    // test_thread

    /*   for (int i = 0; i < 10; i++) {
        std::thread t([limit, i]() {
          if (limit->try_pass()) {
            std::cout << "pass num: " << i << std::endl;
          } else {
            std::cout << "reject num: " << i << std::endl;
          }
        });
        t.detach();
      } */
    // 测试固定窗口缺点带来的局部窗口double压力问题！
    int count = 0;
    for (int j = 0; j < 100; j++) {
        int fail = 0;
        count = 0;
        for (int i = 0; i < 50; i++) {
            if (limit->try_pass()) {
                count++;
            } else {
                fail++;
            }
        }
        std::cout << "process 1 :" << j << " suceess " << count << " fail limit "
                  << fail << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        count = 0;
        fail = 0;
        for (int i = 0; i < 50; i++) {
            if (limit->try_pass()) {
                count++;
            } else {
                fail++;
            }
        }
        std::cout << "process 2 :" << j << " suceess " << count << " fail limit "
                  << fail << std::endl;
        auto sleep = (rand() % 50);
        if (sleep == 0) {
            std::cout << sleep << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }
}

int main() {
    // test_simple_limit();
    test_slide_limit();
    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}