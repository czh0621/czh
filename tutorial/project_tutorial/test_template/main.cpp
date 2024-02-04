#include "../../../src/core_util/function_router.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <type_traits>

using namespace czh::core;

class A
{
public:
    int test(int x) { return x; }
};

template<class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i)
{
    return bool(i % 2);
}

template<class T, typename = typename std::enable_if<!std::is_integral<T>::value, bool>::type>
bool is_odd(T i)
{
    return false;
}

template<class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T i)
{
    return !bool(i % 2);
}

// template<typename F, typename... Args>
// using return_type = typename std::result_of_t<F(Args...)>;

template<typename F, typename... Args>
using return_type = decltype(std::declval<F>()(std::declval<Args>()...));

void test_enable_if()
{
    A a;
    std::cout << "i is odd: " << is_odd(a) << std::endl;
    // std::cout << "i is even: " << is_even(a) << std::endl;
}

// void test_return_type() {
//     A a;
//     auto f = [](int param) { return param; };
//     using T =return_type<decltype(f), int>(f, 2);
// }

void test_function_traits() {}


int global_func(const std::string& s)
{
    spdlog::info("call global_func param:{}", s);
    return std::stoi(s);
}

static std::string global_static_func(int i, double b)
{
    spdlog::info("call global_static_func ");
    auto num = i + b;
    return std::to_string(num);
}


class TestMemberFunc
{
private:
    void test_class_member_private(int i)
    {
        spdlog::info("call test_class_member_private param:{}", i);
    }

public:
    void test_class_member_public(double d)
    {
        spdlog::info("call test_class_member_public param:{}", d);
    }

    void test_class_member_public_const(double d) const
    {
        spdlog::info("call test_class_member_public_const param:{}", d);
    }

    void test_class_member_public_param(const double& d)
    {
        spdlog::info("call test_class_member_public_const& param:{}", d);
    }

    static void test_class_static() { spdlog::info("call test_class_static "); }
};

void test_function_router()
{
    Router router;
    router.register_func_handler("global_func", &global_func);
    router.register_func_handler("global_static_func", &global_static_func);
    auto ptr = std::make_shared<TestMemberFunc>();
    // 不支持私有函数！
    //     router.register_func_handler(
    //         "test_class_member_private", &test_instance,
    //         &TestMemberFunc::test_class_member_private);
    router.register_func_handler(
        "test_class_member_public", ptr.get(), &TestMemberFunc::test_class_member_public);
    router.register_func_handler("test_class_member_public_const",
                                 ptr.get(),
                                 &TestMemberFunc::test_class_member_public_const);
    router.register_func_handler("test_class_member_public_param",
                                 ptr.get(),
                                 &TestMemberFunc::test_class_member_public_param);
    router.register_func_handler("test_class_static", &TestMemberFunc::test_class_static);
    int         value1;
    std::string value2;

    // 注册的函数原型与调用的函数参数原型必须强一致，不允许类型转化 //不能使用"1 作为参数
    auto   ret = router.invoke_method(value1, "global_func", std::string{"1"});
    double d   = 2;
    ret        = router.invoke_method(value2, "global_static_func", 1, d);
    spdlog::info("router call return value1:{} value2:{}", value1, value2);

    router.call_method("test_class_member_public", d);
    router.call_method("test_class_static");
    router.call_method("test_class_member_public_const", d);
    router.call_method("test_class_member_public_param", d);
}

bool g_test_func(int a, int b)
{
    spdlog::info("call g_test_func {} {}", a, b);
    return a < b;
}

template<typename Func>
void myFunction(Func func)
{
    func(1, 2);   // 可以调用传入的函数
}

template<typename T>
class Test
{
public:
    T m_t;

    void operator()(int x, int y) { m_t(x, y); }
};


void test_template_func()
{   // g_test_func 函数作为参数退化成函数指针
    myFunction(g_test_func);
    myFunction<decltype(g_test_func)>(g_test_func);

    // 编译不过 无法使用函数类型构造 对象，而函数指针类型 可以构造对象 即  T m_t;
    //    Test<decltype(g_test_func)>  t{};

    // 使用类型退化 可以构造
    Test<std::decay<decltype(g_test_func)>::type> t3{};

    // 默认构造函数 所以m_t 为null
    Test<decltype(&g_test_func)> t2;

    // 这里使用 {} 初始化 将g_test_func 赋值给 m_t 所以 m_t 部位null
    Test<decltype(&g_test_func)> t1{g_test_func};

    t1(4, 5);

    t2(3, 4);   // 崩溃 null 越界
}



void test_tuple()
{
    std::tuple<int, bool, std::string> t3 = std::forward_as_tuple(11, true, "ok");
    //    std::tuple<int&, bool&, std::string&> t(11, true, "ok");
}

void test_decltype()
{
    TestMemberFunc t;
    int            a = 3, *p = &a, &r = a;
    // decltype val
    using type_val = decltype(t);

    // decltype expr

    using type_expr_1 = decltype(a + 2);
    using type_expr_2 = decltype(*p);
    using type_expr_3 = decltype(r = 4);
    using type_expr_4 = decltype(g_test_func(1, 2));

    // decltype func
    using type_func = decltype(g_test_func);
    spdlog::info("val_1:{},expr_1:{},expr_2:{},expr_3:{},expr_4:{},func_1:{}",
                 typeid(type_val).name(),
                 typeid(type_expr_1).name(),
                 typeid(type_expr_2).name(),
                 typeid(type_expr_3).name(),
                 typeid(type_expr_4).name(),
                 typeid(type_func).name());

    spdlog::info("val_1 is TestMemberFunc:{},expr_1 is int :{},expr_2 is int&:{},expr_3 is "
                 "int&:{},expr_4 is bool:{},func_1 is bool(int,int):{}",
                 std::is_same<type_val, TestMemberFunc>::value,
                 std::is_same<type_expr_1, int>::value,
                 std::is_same<type_expr_2, int&>::value,
                 std::is_same<type_expr_3, int&>::value,
                 std::is_same<type_expr_4, bool>::value,
                 std::is_same<type_func, bool(int, int)>::value);

    auto      val  = r;
    auto&     val2 = r;
    int&&     b    = 4;
    auto      val3 = b;
    const int x    = 3;
    auto      val4 = x;
    spdlog::info("auto val:{},val2:{},val3:{},val4:{}",
                 std::is_same<decltype(val), int&>::value,
                 std::is_same<decltype(val2), int&>::value,
                 std::is_same<decltype(val3), int&&>::value,
                 std::is_same<decltype(val4), const int>::value);
}

namespace test {
// void expandArgs() {}

template<typename T, typename... Ts>
void expandArgs(const T& t, const Ts&... ts)
{
    std::cout << t << '\n';
    //    expandArgs(ts...); //递归求解 ok
    //    expandArgs(ts)...;   // 不允许...作为语句结束 not ok
    // std::tuple{expandArgs(ts)...} //不允许 void 作为tuple<void> 实例化

    // std::tuple{(expandArgs(ts), 42)...}; 不允许 missing template arguments before ‘{’

    // 至此使用参数直接解包,不是递归实现
    std::initializer_list<int>{(expandArgs(ts), 42)...};
}
template<typename F, typename T, size_t... N>
void printTupleImpl(F&& f, const T& t, std::index_sequence<N...>)
{
    (void)std::initializer_list<int>{(f(std::get<N>(t)), 0)...};
    //    (f(std::get<N>(t)), ...);   // C++17
}

template<typename F, typename... Ts>
void printTuple(F&& f, const std::tuple<Ts...>& t)
{
    printTupleImpl(f, t, std::index_sequence_for<Ts...>{});
}
}   // namespace test

void test_fold_expr()
{
    using namespace test;
    expandArgs(1, 2, 3, 4);
    printTuple([](auto& t) { std::cout << t << std::endl; }, std::tuple<int, int>{1, 2});
};

//
template<typename... Ts>
void printSizeOf()
{
    std::cout << "Size of Ts: " << sizeof(Ts...) << std::endl;
}

template<typename... Ts>
void printSizeOf()
{
    std::cout << "Size of Ts: " << sizeof...(Ts) << std::endl;
}


void test_sizeof() {}


int main()
{
    //    test_enable_if();

    //    test_function_router();

    //    test_template_func();

    //    test_tuple();

    //    test_decltype();

    //    test_fold_expr();

    return 0;
}