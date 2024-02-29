/**
 * @file       smart_ptr.h
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#ifndef CZH_SMART_PTR_H_
#define CZH_SMART_PTR_H_
#include <memory>
#include <spdlog/spdlog.h>
#include <string>


class TestPtr : public std::enable_shared_from_this<TestPtr>
{
public:
    std::shared_ptr<TestPtr> get_ptr()
    {   // 错误，等价于shared_ptr<Test> spa(new TestPtr())两次，两个智能指针引用计数独立
        return std::shared_ptr<TestPtr>(this);
    }

    std::shared_ptr<TestPtr> get_shared_ptr() { return shared_from_this(); }


    std::string m_str{"test"};
};

class Base : public std::enable_shared_from_this<Base>
{
public:
    virtual ~Base() = default;

    std::shared_ptr<Base> get_shared_ptr() { return shared_from_this(); }

    virtual void call() { spdlog::info("this is base call"); }

    void call_base_func() { spdlog::info("this is base call_base_func"); }

    std::string m_base{"base"};
};

class Base2
{
public:
    virtual ~Base2() = default;
    virtual void call() { spdlog::info("this is base2 call"); }
    void         call_base2_func() { spdlog::info("this is base2 call_base2_func"); }
    std::string  m_base2{"base2"};
};

class Derived : public Base, public Base2
{
public:
    std::string m_derived{"derived"};

    void call() override { spdlog::info("this is derived call"); }
};



#endif   // CZH_SMART_PTR_H_
