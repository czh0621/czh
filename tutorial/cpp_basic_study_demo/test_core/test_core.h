//
// Created by viruser on 2024/11/8.
//

#ifndef CZH_TEST_CORE_H_
#    define CZH_TEST_CORE_H_

#endif   // CZH_TEST_CORE_H_

template<typename T>
class Singleton
{
public:
    template<typename... Args>
    static T& instance(Args... args)
    {
        static T instance(args...);
        return instance;
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
    {}
    void increase() { m_num++; }
    int  value() { return m_num; }

private:
    int m_num{0};
};