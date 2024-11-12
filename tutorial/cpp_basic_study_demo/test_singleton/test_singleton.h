//
// Created by viruser on 2024/11/8.
//

#ifndef CZH_TEST_SINGLETON_H
#define CZH_TEST_SINGLETON_H
#include <spdlog/spdlog.h>
template<typename T>
class Singleton
{
public:
    template<typename... Args>
    static T& instance(Args... args)
    {
        static T p_instance(args...);
        return p_instance;
    }

private:
    Singleton()                             = default;
    ~Singleton()                            = default;
    Singleton(const Singleton&)             = delete;
    Singleton(Singleton&)                   = delete;
    Singleton&  operator=(const Singleton&) = delete;
    Singleton&& operator=(Singleton&&)      = delete;
};

class TestSingletonRef
{
public:
    TestSingletonRef(int value)
        : m_num(value)
    {
        spdlog::info("TestSingletonRef construct!");
    }
    void increase() { m_num++; }
    int  value() { return m_num; }

private:
    int m_num{0};
};

#endif   // CZH_TEST_SINGLETON_H
