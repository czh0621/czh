#include "../../src/future/try.h"
#include "../../src/future/future.h"
#include <iostream>
#include <thread>

using namespace czh;

void test_invoke_try() {
    czh::Try<int> t(5);
    auto f = [](czh::Try<int> &&t) {
        if (t.has_value()) {
            std::cout << "test_invoke_try try value -------" << t.value() << std::endl;
        } else if (t.has_exception()) {
            std::cout << "test_invoke_try try has exception" << std::endl;
        }
    };
    // f(std::move(t)); //success！
    auto g = [](int num) {
        std::cout << "test_invoke_try num-------" << num << std::endl;
    };
    f(10);           // success！ 隐式转化
    g(std::move(t)); // success！ 隐式转化

    Try<int> exp(std::make_exception_ptr(std::current_exception()));
    f(std::move(exp));
}

void test_promise() {
    Promise<int> p;
    auto f = p.get_future();
    std::thread t([p]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        p.set_value(5);
    });
    t.detach();
    auto x = f.wait(std::chrono::milliseconds(5000));
    std::cout << "future wait value: " << x.value() << std::endl;
}

int main() {
    // test_invoke_try();
    test_promise();
    return 0;
}