/**
 * @file       class.h
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#ifndef CZH_CLASS_H_
#define CZH_CLASS_H_
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

using namespace std;
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

class BaseA
{
public:
    virtual void funcA() { spdlog::info("call virtual BaseA funcA"); }

    virtual void commonA() { spdlog::info("call virtual BaseA commonA"); };

    void common_func() { spdlog::info("call  BaseA common_func"); }

    void func() { spdlog::info("call  BaseA func"); }

    int m_BaseA;
};

class BaseB
{
public:
    virtual void funcB() { spdlog::info("call virtual BaseB funcB"); }

    virtual void commonB() { spdlog::info("call virtual BaseB commonB"); };

    void func2() { spdlog::info("call  BaseB func2"); }

    int m_BaseB;
};

class Derived2 : public BaseA, public BaseB
{
public:
    virtual void funcA() override { spdlog::info("call override Derived2 funcA"); }

    virtual void funcB() override { spdlog::info("call override Derived2 funcB"); }

    void func() { spdlog::info("call  Derived2 func"); }

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


class ClassA
{
public:
    ClassA() { cout << "ClassA::ClassA()" << endl; }
    virtual ~ClassA() { cout << "ClassA::~ClassA()" << endl; }

    void func1() { cout << "ClassA::func1()" << endl; }
    void func2() { cout << "ClassA::func2()" << endl; }

    virtual void vfunc1() { cout << "ClassA::vfunc1()" << endl; }
    virtual void vfunc2() { cout << "ClassA::vfunc2()" << endl; }

private:
    int aData;
};

class ClassB : public ClassA
{
public:
    ClassB() { cout << "ClassB::ClassB()" << endl; }
    virtual ~ClassB() { cout << "ClassB::~ClassB()" << endl; }

    void         func1() { cout << "ClassB::func1()" << endl; }
    virtual void vfunc1() { cout << "ClassB::vfunc1()" << endl; }

private:
    int bData;
};

class ClassC : public ClassB
{
public:
    ClassC() { cout << "ClassC::ClassC()" << endl; }
    virtual ~ClassC() { cout << "ClassC::~ClassC()" << endl; }

    void         func2() { cout << "ClassC::func2()" << endl; }
    virtual void vfunc2() { cout << "ClassC::vfunc2()" << endl; }

private:
    int cData;
};



class MyClass
{
public:
    // 默认构造函数
    MyClass()
        : data("MyClass member")
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

    std::string get_value() { return data; }

private:
    std::string data;
};


// 测试this
class TestBaseThis;
class Container
{
public:
    std::vector<TestBaseThis*> m_vec;

    TestBaseThis* get_ptr() { return m_vec[0]; }
};


class TestBaseThis
{
public:
    explicit TestBaseThis(std::shared_ptr<Container> c_ptr)
    {
        spdlog::info(
            "constructor TestBaseThis this type:{} addr:{}", typeid(this).name(), (void*)(this));
        (c_ptr->m_vec).push_back(this);
    }
    virtual void func() { spdlog::info("TestBaseThis call"); }
};

class DerivedTestThis : public TestBaseThis
{
public:
    explicit DerivedTestThis(std::shared_ptr<Container> c_ptr)
        : TestBaseThis(c_ptr)
    {
        spdlog::info(
            "constructor DerivedTestThis this type:{} addr:{}", typeid(this).name(), (void*)(this));
    }
    void func() override { spdlog::info("DerivedTestThis call"); }
};








#endif   // CZH_CLASS_H_
