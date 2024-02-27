/**
 * @file       class.h
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#ifndef CZH_CLASS_H_
#define CZH_CLASS_H_
#include <spdlog/spdlog.h>
class EmptyClass
{};

class VirtualClass
{
    VirtualClass()          = default;
    virtual ~VirtualClass() = default;
    // 会默认使用内存对齐 所以为16字节
    int32_t m_i;
};

class CommonClass
{
    int m_i;
};

// 默认8字节对齐（最小8B）
class alignas(8) VirtualClassAlignas
{
    virtual ~VirtualClassAlignas() = default;
    VirtualClassAlignas()          = default;
    int32_t m_i;
};

class Base
{
public:
    virtual void print() { spdlog::info("call Base print func"); }

    void common() { spdlog::info("call Base common func"); }

    void f1() { spdlog::info("call Base f1 func"); }

    virtual void override() { spdlog::info("call Base override func"); }

    void redefine() { spdlog::info("call Base redefine func"); }


private:
    int m_base = 1;
};

class Derived : public Base
{
public:
    virtual void print() { spdlog::info("call Derived print func"); }

    void f2() { spdlog::info("call Derived f2 func"); }

    virtual void override() override { spdlog::info("call Derived override func"); }

    void redefine() { spdlog::info("call Derived redefine func"); }

    void redefine(int t) { spdlog::info("call Derived redefine func with param"); }


private:
    int m_derived  = 2;   // 8+vptr(8) =16
    int m_derived1 = 2;   // 12+vptr(8) 对齐后为24
};

class BaseA
{
public:
    virtual void funcA() { spdlog::info("call BaseA funcA"); }

    virtual void common1() { spdlog::info("call BaseA common1"); };

    int m_BaseA;
};

class BaseB
{
public:
    virtual void funcB() { spdlog::info("call BaseB funcB"); }

    virtual void common2() { spdlog::info("call BaseB common2"); };

    int m_BaseB;
};

class Derived2 : public BaseA, public BaseB
{
public:
    virtual void funcA() override { spdlog::info("call Derived2 funcA"); }

    virtual void funcB() override { spdlog::info("call Derived2 funcB"); }

    int m_Derived2;
};

class Test_Base
{
public:
    Test_Base(int x)
    {
        m_base = x;
        // 此处的this
        // 为基类指针，但是指针地址为子类地址（也是子类对象中的基类部分首地址），它指向正在初始化的对象的内存地址
        spdlog::info(
            "base constructor this ptr type:{} addr:{}", typeid(*this).name(), (int64_t)this);
    }

    void T_func1()
    {
        // 此处隐式this 指针 基类类型！会形成多态！
        this->T_func_common();

        T_func_common();

        this->interface_api();

        interface_api();
    }

    virtual void T_func_common() { spdlog::info("call Test_Base T_func_common"); }

    virtual void interface_api() = 0;

    int m_base;
};

class Test_Derived : public Test_Base
{
public:
    Test_Derived(int x)
        : m_derived(x)
        , Test_Base(x)
    {
        spdlog::info(
            "derived constructor this ptr type:{} addr:{}", typeid(*this).name(), (int64_t)this);
    }

    void T_func_common() override { spdlog::info("call Test_Derived T_func_common"); }

    void interface_api() override { spdlog::info("call interface_api instance"); }

    int m_derived;
};


class ObjectA
{};

class Base_b
{
public:
    int publicMember;

protected:
    int protectedMember;

private:
    int privateMember;
};

class Derived_d : public Base_b
{
public:
    void accessBaseMembers()
    {
        publicMember    = 42;   // 子类可以访问基类的公有成员
        protectedMember = 42;   // 子类可以访问基类的保护成员
        //        privateMember   = 42;子类不能直接访问基类的私有成员
    }
};

#include <iostream>

class MyClass
{
public:
    // 默认构造函数
    MyClass()
        : data(0)
    {
        std::cout << "Default constructor" << std::endl;
    }

    // 析构函数
    ~MyClass() { std::cout << "Destructor" << std::endl; }

    // 拷贝构造函数
    // 这里通常采用const + & 的格式。因为函数传参分为值传参（会copy）和地址传参（不会copy）
    // A(const A& obj) //拷贝构造函数 加引用这样可以避免在函数调用时对实参的一次拷贝，提高了效率。
    // 加const 不希望修改，防止使用别人的，修改了别人的东西。
    MyClass(const MyClass& other)
        : data(other.data)
    {
        std::cout << "Copy constructor" << std::endl;
    }

    // 移动构造函数
    MyClass(MyClass&& other) noexcept
        : data(std::move(other.data))
    {
        std::cout << "Move constructor" << std::endl;
    }

    // 拷贝赋值操作符
    MyClass& operator=(const MyClass& other)
    {
        std::cout << "Copy assignment operator" << std::endl;
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    // 移动赋值操作符
    MyClass& operator=(MyClass&& other) noexcept
    {
        std::cout << "Move assignment operator" << std::endl;
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

private:
    int data;
};






#endif   // CZH_CLASS_H_
