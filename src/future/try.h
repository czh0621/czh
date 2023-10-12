/**
 * @file       try.h
 * @date       2023/9/11
 * @since      2023-09-11
 * @author     czh
 */

#ifndef CZH_TRY_H_
#define CZH_TRY_H_
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace czh {
class TryException : public std::logic_error
{
public:
    using std::logic_error::logic_error;
};

class Uninitialization : public TryException
{
public:
    Uninitialization()
        : TryException("Uninitialization Try"){};
};

inline void get_exception_what(const std::exception_ptr& e, std::string& out)
{
    try {
        std::rethrow_exception(e);
    }
    catch (const std::exception& exp) {
        out = exp.what();
    }
}

template<typename T>
class Try
{
    enum class State
    {
        None,
        Value,
        Exception,
    };

public:
    Try()
        : m_state(State::None)
    {}

    // 不加explict 需要隐式转换
    Try(const T& t)
        : m_state(State::Value)
        , m_value(t)
    {
        std::cout << "const T &t" << std::endl;
    }

    // 此处不能显示实例化void 没有void&的写法，因此需要模板特化void版本
    Try(T&& t)
        : m_state(State::Value)
        , m_value(std::move(t))
    {
        std::cout << "T&&" << std::endl;
    }

    explicit Try(std::exception_ptr&& e)
        : m_state(State::Exception)
        , m_exception(e)
    {}

    Try(const Try<T>& t)
        : m_state(t.m_state)
    {
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(t.m_exception);
        }
        else if (t.m_state == State::Value) {
            new (&m_value) T(t.m_value);
        }
    }

    Try<T>& operator=(const Try<T>& t)
    {
        if (&t == this) {
            return *this;
        }
        this->destroy();
        if (t.m_state == State::Value) {
            new (&m_value) T(t.m_value);
        }
        else if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(t.m_exception);
        }
        this->m_state = t.m_state;
        return *this;
    }

    Try(Try<T>&& t) noexcept
        : m_state(std::move(t.m_state))
    {
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(std::move(t.m_exception));
        }
        else if (t.m_state == State::Value) {
            new (&m_value) T(std::move(t.m_value));
        }
    };

    Try<T>& operator=(Try<T>&& t) noexcept
    {
        if (&t == this) {
            return *this;
        }
        this->destroy();
        if (t.m_state == State::Value) {
            new (&m_value) T(std::move(t.m_value));
        }
        else if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(std::move(t.m_exception));
        }
        this->m_state = std::move(t.m_state);
        return *this;
    }

    bool has_value() const { return m_state == State::Value; }

    bool has_exception() const { return m_state == State::Exception; }

    const T& value() const&
    {
        check();
        return m_value;
    }

    T& value() &
    {
        check();
        return m_value;
    }

    T&& value() &&
    {
        check();
        return std::move(m_value);
    }

    void check() const
    {
        if (m_state == State::None) {
            throw Uninitialization();
        }
        else if (m_state == State::Exception) {
            std::rethrow_exception(m_exception);
        }
    }

    operator const T&() const& { return value(); }

    operator T&() & { return value(); }

    operator T&&() && { return std::move(value()); }

    const T& operator*() const { return value(); }

    T& operator*() { return value(); }

    const T* operator->() const { return &value(); }

    T* operator->() { return &value(); }

    const std::exception_ptr& exception() const&
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return m_exception;
    }

    std::exception_ptr&& exception() &&
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return std::move(m_exception);
    }

    std::exception_ptr& exception() &
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return m_exception;
    }

    std::string get_exception()
    {
        std::string exp_str;
        get_exception_what(m_exception, exp_str);
        return exp_str;
    }

    ~Try() { this->destroy(); }

    template<typename R>
    R get()
    {
        // std::forward 会自动去除引用
        return std::forward<R>(*this);
    }

private:
    void destroy()
    {
        auto old_state = std::exchange(this->m_state, State::None);
        if (old_state == State::Exception) {
            m_exception.~exception_ptr();
        }
        else if (old_state == State::Value) {
            m_value.~T();
        }
    }

private:
    State m_state;
    union
    {
        T m_value;   // 模板推到时不能将T推导为&类型！！！！上层需使用decay进行类型退化！
        std::exception_ptr m_exception;
    };
};

template<>
class Try<void>
{
    enum class State
    {
        Value,
        Exception,
    };

public:
    Try()
        : m_state(State::Value)
    {}

    Try(std::exception_ptr&& e)
        : m_exception(e)
        , m_state(State::Exception)
    {}

    Try(const Try<void>& t)
        : m_state(t.m_state)
    {
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(t.m_exception);
        }
    }

    Try(Try<void>&& t) noexcept
        : m_state(t.m_state)
    {
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(std::move(t.m_exception));
        }
    }

    Try<void>& operator=(const Try<void>& t)
    {
        if (&t == this) {
            return *this;
        }
        this->destroy();
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(t.m_exception);
        }
        this->m_state = t.m_state;
        return *this;
    }

    Try<void>& operator=(Try<void>&& t) noexcept
    {
        if (&t == this) {
            return *this;
        }
        this->destroy();
        if (t.m_state == State::Exception) {
            new (&m_exception) std::exception_ptr(std::move(t.m_exception));
        }
        this->m_state = t.m_state;
        return *this;
    }

    ~Try() { this->destroy(); }

    bool has_value() const { return m_state == State::Value; }

    bool has_exception() const { return m_state == State::Exception; }

    void check() const
    {
        if (m_state == State::Exception) {
            std::rethrow_exception(m_exception);
        }
    }

    const std::exception_ptr& exception() const&
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return m_exception;
    }

    std::exception_ptr&& exception() &&
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return std::move(m_exception);
    }

    std::exception_ptr& exception() &
    {
        if (!has_exception()) {
            throw std::runtime_error("Not has exception! State error!");
        }
        return m_exception;
    }

    std::string get_exception()
    {
        std::string exp_str;
        get_exception_what(m_exception, exp_str);
        return exp_str;
    }

    template<typename R>
    R get()
    {
        return std::forward<R>(*this);
    }

private:
    void destroy()
    {
        if (m_state == State::Exception) {
            m_exception.~exception_ptr();
        }
    }

private:
    State              m_state;
    std::exception_ptr m_exception;
};

template<typename T>
struct TryWrapper
{
    using type = Try<T>;
};

template<typename T>
struct TryWrapper<Try<T>>
{
    using type = Try<T>;
};

// return Try<T> invoke by F(Args...)
template<typename F, typename... Args>
auto invoke_with_TryWrapper(F&& f, Args&&... args) -> typename std::enable_if<
    !std::is_same<typename std::result_of<F(Args...)>::type, void>::value,
    typename TryWrapper<typename std::result_of<F(Args...)>::type>::type>::type
{
    try {
        using return_type = typename std::result_of<F(Args...)>::type;
        return
            typename TryWrapper<return_type>::type(std::forward<F>(f)(std::forward<Args>(args)...));
    }
    catch (const std::exception& e) {
        (void)e;
        return typename TryWrapper<typename std::result_of<F(Args...)>::type>::type(
            std::current_exception());
    }
}

// return Try<void> invoke by F(Args...)
template<typename F, typename... Args>
auto invoke_with_TryWrapper(F&& f, Args&&... args) ->
    typename std::enable_if<std::is_same<typename std::result_of<F(Args...)>::type, void>::value,
                            Try<void>>::type
{
    try {
        //        return Try<void>(std::forward<F>(f)(std::forward<Args...>(args...)));
        // 无构造函数
        std::forward<F>(f)(std::forward<Args>(args)...);
        return Try<void>();
    }
    catch (const std::exception& e) {
        (void)e;
        return Try<void>(std::current_exception());
    }
}

// return Try<T> invoke by F(void)
template<typename F>
auto invoke_with_TryWrapper(F&& f, Try<void>&& arg) ->
    typename std::enable_if<!std::is_same<typename std::result_of<F()>::type, void>::value,
                            typename TryWrapper<typename std::result_of<F()>::type>::type>::type
{
    try {
        using return_type = typename std::result_of<F()>::type;
        return typename TryWrapper<return_type>::type(std::forward<F>(f)());
    }
    catch (const std::exception& e) {
        (void)e;
        return
            typename TryWrapper<typename std::result_of<F()>::type>::type(std::current_exception());
    }
}

// return Try<void> invoke by F(void)
template<typename F>
auto invoke_with_TryWrapper(F&& f, Try<void>&& arg) ->
    typename std::enable_if<std::is_same<typename std::result_of<F()>::type, void>::value,
                            Try<void>>::type
{
    try {
        std::forward<F>(f)();
        return Try<void>();
    }
    catch (const std::exception& e) {
        (void)e;
        return Try<void>(std::current_exception());
    }
}

}   // namespace czh
#endif   // CZH_TRY_H_
