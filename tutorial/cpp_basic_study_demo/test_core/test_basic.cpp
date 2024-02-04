/**
 * @file       test_basic.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#include <spdlog/spdlog.h>
void test_int()
{
    // int 有符号 根据平台来显示一般32位、64位
    // int32_t 有符号 指定32长度
    // size_t 无符号 根据平台来显示
    // long 有符号 根据平台来显示 32位等于int
    // long long 强指定64位 有符号

    spdlog::info("int {},int32_t {},size_t {},long {}, long long {}",
                 sizeof(int),
                 sizeof(int32_t),
                 sizeof(size_t),
                 sizeof(long),
                 sizeof(long long));
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
    //    auto    f1 = [](const Metrics& m) mutable { m.set_value(2); };
    auto f2 = [](const std::shared_ptr<Metrics>& ptr) mutable { ptr->set_value(2); };
}
void test_mod()
{
    int  x   = -3;
    int  y   = 5;
    auto mod = x % y;
    spdlog::info("x ={},y={},mod:{}", x, y, mod);
}



int main()
{
    //    test_int();
    //    test_lambda();
    test_mod();
    return 0;
}