#include <iostream>
#include <string>
#include <thread>

#include "../../src/future/future.h"

using namespace czh;

class A {
public:
    A() = default;

    A(const A &) { std::cout << "const A&" << std::endl; }

    A(A &&) noexcept { std::cout << "A&&" << std::endl; }
};

//void test_invoke_try() {
//    czh::Try<int> t(5);
//    auto f = [](czh::Try<int> &&t) {
//        if (t.has_value()) {
//            std::cout << "test_invoke_try try value -------" << t.value()
//                      << std::endl;
//        } else if (t.has_exception()) {
//            auto exp = t.get_exception();
//            std::cout << "test_invoke_try try has exception: " << exp << std::endl;
//        }
//    };
//    // f(std::move(t)); //success！
//    auto g = [](int num) {
//        std::cout << "test_invoke_try num-------" << num << std::endl;
//    };
//    f(10);            // success！ 隐式转化
//    g(std::move(t));  // success！ 隐式转化
//
//    Try<int> exp(
//            std::make_exception_ptr(std::runtime_error("make test exception!")));
//    f(std::move(exp));
//}
//



void test_promise() {
    Promise<int> p;
    auto f = p.get_future();
    double num = 4.23;  // 隐式转换了 double->int
    std::thread t([p, num]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(6000));
        p.set_value(num);
    });
    t.detach();
    auto v = f.wait(std::chrono::milliseconds(5000));
    if (v.has_value()) {
        std::cout << "future wait value: " << v.value() << std::endl;  // 4
    } else if (v.has_exception()) {
        auto exp = v.get_exception();
        std::cout << "future wait has exception " << exp << std::endl;
    }

    Promise<void> prom;
    auto fut = prom.get_future();
    std::thread t2([prom]() mutable {
        prom.set_value();
    });
    t2.detach();

    auto res = fut.then([](Try<void> &&t) {
        std::cout << "then 2 excute" << std::endl;
        return 1;
    }).wait(std::chrono::milliseconds(1000));

    if (res.has_value()) {
        std::cout << "future wait value: " << res.value() << std::endl;
    } else if (res.has_exception()) {
        auto exp = res.get_exception();
        std::cout << "future wait has exception " << exp << std::endl;
    }
}



//void test_set_value() {
//    Promise<A> p;
//    A a;
//    auto fut = p.get_future();
//    auto func = [p, a]() mutable {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        p.set_value(a);
//        // 调用 T &&t ------set_value 参数为A&value,m_shared_state->m_value=
//        // std::forward<SHIT>(value);
//        // std::forward<A&>(value) 会先调用A的拷贝构造函数构造Try<A>。
//        // 在进行Try<A>移动构造,会包含A的移动构造！
//    };
//    func();
//}
//
//void test_deduce() {
//    auto f = [](Try<int> &&t) { std::cout << "future then excute" << std::endl; };
//    using Func_type = decltype(f);
//    constexpr bool value = Invokable<Func_type, int>::value;
//    constexpr bool value1 = Invokable<Func_type>::value;
//    std::cout << "f(int) :" << value << "---- f(): " << value1 << std::endl;
//    // static_assert(value, "f can not be called");
//    using type = CallableResult<Func_type, int>::Return_type;
//    auto func = []() {};
//    using typeA = decltype(func);
//    std::cout << std::is_same<std::decay<typeA>::type, typeA>::value;
//}
//

void test_future_then() {
//    Promise<int> p;
//    auto f = p.get_future();
//    int x = 5;
//    const int &y = x;
//    std::thread t([p, &y]() mutable {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        p.set_value(y);
//    });
//    t.detach();
//    f.then([](Try<int> &&t) {
//                if (t.has_exception()) {
//                    auto exp = t.get_exception();
//                    std::cout << "future then excute exception:" << exp << std::endl;
//                } else if (t.has_value()) {
//                    std::cout << "future then excute value:" << t.value() << std::endl;
//                }
//            })
//            .then([](Try<void> &&t) {
//                std::cout << "future then excute void" << std::endl;
//            })
//            .then([]() { std::cout << "future then excute void2" << std::endl; });
//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));


    Promise<int> prom;
    Promise<void> inner_prom;
    auto cb = []() { std::cout << "cb" << std::endl; };
    auto fut = prom.get_future();
    auto res = fut.then([inner_prom](Try<int> &&t) {
        if (t.has_exception()) {
            auto exp = t.get_exception();
            std::cout << "future then excute exception:" << exp << std::endl;
        } else if (t.has_value()) {
            std::cout << "future then excute value:" << t.value() << std::endl;
        }
    }).then([inner_prom, cb](Try<void> &&t) mutable {
        if (t.has_value()) {
            std::cout << "future then excute void" << std::endl;
        }
//        auto fut2 = MakeReadyFuture();
//        return fut2;
        auto fut_inner = inner_prom.get_future();
        auto res = fut_inner.then([](Try<void> &&t) { std::cout << "fut_inner done!" << std::endl; }).then(cb);
        return res;
    }).then([]() {
        std::cout << "future then excute future<void>" << std::endl;
    }).then(cb);
    std::thread t2([prom, inner_prom]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        prom.set_value(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        inner_prom.set_value();
    });
    t2.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void test_ready_future() {
    int x = 5;
    auto fut_value = MakeReadyFuture(x);
    fut_value.then([](Try<int> &&i) {});
    auto fut_void = MakeReadyFuture();
}

void test_exception() {
    Promise<int> prom;
    auto fut = prom.get_future();
    fut.then([](Try<int> &&t) {
        std::cout << "then 1 start" << std::endl;
        throw std::runtime_error("test exception");
        return 2;
    }).then([](Try<int> &&t) {
        try {
            if (t.has_exception()) {
                std::cout << "then 1 exception : " << t.get_exception() << std::endl;
                throw std::runtime_error("test exception 2");
            } else {
                std::cout << "then 2 no exception" << std::endl;
            }
            std::cout << "then 2 finish" << std::endl;
        }
        catch (const std::exception &e) {
            std::cout << "exp " << e.what() << std::endl;
        }
    });

    prom.set_value(12);
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}


int main() {
    // test_invoke_try();
    // test_promise();
    // test_deduce();
    test_future_then();
    //test_set_value();
    //test_ready_future();

    //test_exception();
    return 0;
}