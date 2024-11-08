#include <spdlog/spdlog.h>
#include <string>
#include <thread>

#include "../../../src/future/when.h"

using namespace czh;

class A
{
public:
    A() = default;

    A(const A&) { std::cout << "const A&" << std::endl; }

    A(A&&) noexcept { std::cout << "A&&" << std::endl; }
};

void test_invoke_try()
{
    czh::Try<int> t(5);
    auto          f = [](czh::Try<int>&& t) {
        if (t.has_value()) {
            std::cout << "test_invoke_try try value -------" << t.value() << std::endl;
        }
        else if (t.has_exception()) {
            auto exp = t.get_exception();
            std::cout << "test_invoke_try try has exception: " << exp << std::endl;
        }
    };

    //    f(std::move(t));   // success！

    auto g = [](int num) { std::cout << "test_invoke_try num-------" << num << std::endl; };
    f(10);             // success！ 隐式转化 int-->czh::Try<int>
    g(std::move(t));   // success！ 隐式转化 czh::Try<int>-->int

    Try<int> exp(std::make_exception_ptr(std::runtime_error("make test exception!")));
    f(std::move(exp));
}




void test_promise()
{
    // test value
    Promise<int> p;
    auto         f   = p.get_future();
    double       num = 4.23;   // 隐式转换了 double->int
    std::thread  t([p, num]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(6000));
        p.set_value(num);
    });
    t.detach();
    auto v = f.wait(std::chrono::milliseconds(5000));
    if (v.has_value()) {
        std::cout << "future wait value: " << v.value() << std::endl;   // 4
    }
    else if (v.has_exception()) {
        auto exp = v.get_exception();
        std::cout << "future wait has exception " << exp << std::endl;
    }

    // test void
    Promise<void> prom;
    auto          fut = prom.get_future();
    std::thread   t1([prom]() mutable { prom.set_value(); });
    t1.detach();

    auto res = fut.then([](Try<void>&& t) {
                      std::cout << "then 2 excute" << std::endl;
                      return 1;
                  })
                   .wait(std::chrono::milliseconds(1000));

    if (res.has_value()) {
        std::cout << "future wait value: " << res.value() << std::endl;
    }
    else if (res.has_exception()) {
        auto exp = res.get_exception();
        std::cout << "future wait has exception " << exp << std::endl;
    }

    // test parm void
    Promise<void> prom2;
    auto          fut2 = prom2.get_future();
    std::thread   t2([prom2]() mutable { prom2.set_value(); });
    t2.detach();
    auto fut_final = fut2.then([]() { std::cout << "fut2 then excute" << std::endl; });
}



void test_set_value()
{
    Promise<A> p;
    A          a;
    auto       fut  = p.get_future();
    auto       func = [p, a]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // 调用 T &&t ------set_value 参数为A&value,m_shared_state->m_value=
        // std::forward<SHIT>(value);
        // std::forward<A&>(value) 会先调用A的构造函数构造Try<A>。
        // 在进行Try<A>移动构造,会包含A的移动构造！
        p.set_value(a);
    };
    func();
}

void test_deduce()
{
    auto f                = [](Try<int>&& t) { std::cout << "future then excute" << std::endl; };
    using Func_type       = decltype(f);
    constexpr bool value  = Invokable<Func_type, int>::value;
    constexpr bool value1 = Invokable<Func_type>::value;
    std::cout << "f(int) :" << value << "---- f(): " << value1 << std::endl;
    // static_assert(value, "f can not be called");
    using type  = CallableResult<Func_type, int>::Return_type;
    auto func   = []() {};
    using typeA = decltype(func);
    std::cout << "type value:" << std::is_same<std::decay<typeA>::type, typeA>::value << std::endl;
}


void test_future_then()
{
    Promise<int> p;
    auto         fut1 = p.get_future();
    int          x    = 5;
    const int&   y    = x;
    std::thread  t([p, &y, x]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        p.set_value(y);
    });
    t.detach();
    fut1.then([](Try<int>&& t) {
            if (t.has_exception()) {
                auto exp = t.get_exception();
                std::cout << "future then excute exception:" << exp << std::endl;
            }
            else if (t.has_value()) {
                std::cout << "future then excute value:" << t.value() << std::endl;
            }
        })
        .then([](Try<void>&& t) { std::cout << "future then excute void" << std::endl; })
        .then([]() { std::cout << "future then excute void2" << std::endl; });
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    Promise<int>  prom;
    Promise<void> inner_prom;
    auto          cb  = []() { std::cout << "cb call" << std::endl; };
    auto          fut = prom.get_future();
    auto          next_res =
        fut.then([inner_prom](Try<int>&& t) {
               if (t.has_exception()) {
                   auto exp = t.get_exception();
                   std::cout << "future then excute exception:" << exp << std::endl;
               }
               else if (t.has_value()) {
                   std::cout << "future then excute value:" << t.value() << std::endl;
               }
           })
            .then([inner_prom, cb](Try<void>&& t) mutable {
                if (t.has_value()) {
                    std::cout << "future then excute void" << std::endl;
                }
                //        auto fut2 = make_ready_future();
                //        return fut2;
                auto fut_inner = inner_prom.get_future();
                auto res =
                    fut_inner
                        .then([](Try<void>&& t) { std::cout << "fut_inner done!" << std::endl; })
                        .then(cb);
                return res;
            })
            .then([]() { std::cout << "future then excute future<void>" << std::endl; })
            .then(cb);
    std::thread t2([prom, inner_prom]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        prom.set_value(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        inner_prom.set_value();
    });
    t2.detach();
}

void test_ready_future()
{
    int  x         = 5;
    auto fut_value = make_ready_future(x);
    fut_value.then([](Try<int>&& i) {});
    auto fut_void = make_ready_future();
}

void test_return_fut()
{
    int  x            = 5;
    auto fut_value    = make_ready_future(x);
    auto return_fut   = fut_value.then([](Try<int>&& i) { return make_ready_future(55); });
    auto fut          = make_ready_future(3);
    auto return_value = fut.then([](Try<int>&& i) { return 1; });
    spdlog::info("return_fut type:{} return_value type:{}",
                 typeid(return_fut).name(),
                 typeid(return_value).name());
}

void test_exception()
{
    Promise<int> prom;
    auto         fut = prom.get_future();
    fut.then([](Try<int>&& t) {
           std::cout << "then 1 start" << std::endl;
           throw std::runtime_error("test exception");
           return 2;
       })
        .then([](Try<int>&& t) {
            try {
                if (t.has_exception()) {
                    std::cout << "then 1 exception : " << t.get_exception() << std::endl;
                    throw std::runtime_error("test exception 2");
                }
                else {
                    std::cout << "then 2 no exception" << std::endl;
                }
                std::cout << "then 2 finish" << std::endl;
            }
            catch (const std::exception& e) {
                std::cout << "exp " << e.what() << std::endl;
            }
        });

    prom.set_value(12);
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}

void test_is_future()
{
    Future<int>                 fut;
    Future<Future<int>>         fut2;
    Future<Future<Future<int>>> fut3;
    auto                        value1 = IsFuture<decltype(fut)>::value;
    // int
    auto type1  = typeid(IsFuture<decltype(fut)>::inner_type).name();
    auto value2 = IsFuture<decltype(fut2)>::value;
    // Future<int>
    auto type2  = typeid(IsFuture<decltype(fut2)>::inner_type).name();
    auto value3 = IsFuture<decltype(fut3)>::value;
    // Future<Future<int>>
    auto type3 = typeid(IsFuture<decltype(fut3)>::inner_type).name();
    spdlog::info("fut type_value:{} inner_type:{} fut2 type_value:{} inner_type:{} fut3 "
                 "type_value:{} inner_type:{}",
                 value1,
                 type1,
                 value2,
                 type2,
                 value3,
                 type3);
}

void test_when_all()
{

    Promise<int> prom1;
    auto         f1 = prom1.get_future().then([](Try<int>&& i) {
        spdlog::info("future1 call:{} ", i.value());
        return 1;
    });

    Promise<int> prom2;
    auto         f2 =
        prom2.get_future().then([](Try<int>&& i) { spdlog::info("future2 call:{} ", i.value()); });

    auto fut = when_all(f1, f2).then([](std::tuple<Try<int>, Try<void>>&& t) {
        spdlog::info("call when all then");
        spdlog::info("get fut1value:{}", std::get<0>(t).value());
    });

    std::thread t([prom1, prom2]() mutable {
        prom1.set_value(1);
        spdlog::info("finish prom1  set value");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        prom2.set_value(2);
        spdlog::info("finish  prom2 set value");
    });
    t.detach();
    prom1.set_value(1);
    prom2.set_value(2);

    spdlog::info("finish when all");
}

int main()
{
    //    test_invoke_try();
    //
    //
    //    test_promise();


    // test_deduce();

    //    test_future_then();
    //    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // test_set_value();

    // test_ready_future();

    // test_exception();

    //    test_is_future();

    //    test_when_all();
    test_return_fut();
    spdlog::info("this_thread sleep");
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}