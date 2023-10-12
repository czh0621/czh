/**
 * @file       future.h
 * @date       2023/9/11
 * @since      2023-09-11
 * @author     czh
 */

#ifndef CZH_FUTURE_H_
#define CZH_FUTURE_H_
#include "helper.h"
#include "promise.h"

namespace czh {
template<typename T>
class Future
{
public:
    Future() = default;

    explicit Future(std::shared_ptr<SharedState<T>> ptr)
        : m_shared_state(ptr)
    {}

    Future(const Future&) = delete;

    Future(Future&& fut) = default;

    Future& operator=(const Future&) = delete;

    Future& operator=(Future&& fut) = default;

    typename SharedState<T>::ValueType wait(
        const std::chrono::milliseconds& time = std::chrono::milliseconds(24 * 3600 * 1000))
    {
        try {
            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
            switch (m_shared_state->m_status) {
            case FutureStatus::None:
                /* code */
                break;
            case FutureStatus::Timeout: throw std::runtime_error("FutureStatus Timeout！");

            case FutureStatus::Done:
                m_shared_state->m_status = FutureStatus::Retrived;
                return std::move(m_shared_state->m_value);

            default: throw std::runtime_error("Future already retrieved !");
            }
        }
        catch (const std::exception& e) {
            (void)e;
            return typename SharedState<T>::ValueType(std::current_exception());
        }
        if (m_shared_state->wait_for(time) == FutureStatus::Timeout) {
            return typename SharedState<T>::ValueType(
                std::make_exception_ptr(std::runtime_error("Future wait_for timeout")));
        }
        else {
            return typename SharedState<T>::ValueType(std::move(m_shared_state->m_value));
        }
    }

    template<typename F, typename R = CallableResult<F, T>>
    auto then(F&& f) -> typename CallableResult<F, T>::ReturnFutureType
    {
        using Args = typename CallableResult<F, T>::Return_type;
        return then_impl<F, R>(nullptr, std::forward<F>(f), Args{});
    }

    bool valid() const { return m_shared_state != nullptr; }

private:
    template<typename F, typename R, typename... Args>
    auto then_impl(void*, F&& f, ResultOfWrapper<F, Args...>) ->
        typename std::enable_if_t<!R::IsFutureReturnType::value, typename R::ReturnFutureType>
    {
        static_assert(sizeof...(Args) <= 1, "Then must take zero/one argument");
        using FuncReturnType = typename R::Return_type::type;
        using FuncType       = std::decay_t<F>;
        Promise<FuncReturnType>      promise;
        auto                         fut = promise.get_future();
        std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
        if (m_shared_state->m_status == FutureStatus::Timeout) {}
        else {
            set_callback([prom = std::move(promise), func = std::forward<FuncType>(f)](
                             typename TryWrapper<T>::type&& t) mutable {
                auto res = invoke_with_TryWrapper(func, std::move(t));
                prom.set_value(res);
            });

            if (m_shared_state->m_status == FutureStatus::Done) {
                if (m_shared_state->m_then) {
                    try {
                        m_shared_state->m_then(std::move(m_shared_state->m_value));
                    }
                    catch (const std::exception& exp) {
                        m_shared_state->m_then(
                            typename TryWrapper<T>::type(std::current_exception()));
                    }
                }
            }
        }
        return fut;
    }

    template<typename F, typename R, typename... Args>
    auto then_impl(void*, F&& f, ResultOfWrapper<F, Args...>) ->
        typename std::enable_if_t<R::IsFutureReturnType::value, typename R::ReturnFutureType>
    {
        static_assert(sizeof...(Args) <= 1, "Args must be not more than one!");
        using FuncReturnType = typename R::IsFutureReturnType::inner_type;
        using FuncType       = std::decay_t<F>;
        Promise<FuncReturnType>      prom;
        auto                         next_fut = prom.get_future();
        std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
        if (m_shared_state->m_status == FutureStatus::Timeout) {}
        else {
            set_callback([prom = std::move(prom), func = std::forward<FuncType>(f)](
                             typename TryWrapper<T>::type&& t) mutable {
                auto cb = [prom, func, res = std::move(t)]() mutable {
                    decltype(func(res.template get<Args>()...)) inner_fut;
                    try {
                        if (res.has_exception()) {
                            inner_fut =
                                func(typename TryWrapper<typename std::decay<Args...>::type>::type(
                                    (std::move(res.exception()))));
                        }
                        else {
                            inner_fut = func(res.template get<Args>()...);
                        }
                    }
                    catch (const std::exception& exp) {
                        (void)exp;
                        Promise<FuncReturnType> inner_promise;
                        inner_fut = std::move(inner_promise.get_future());
                        inner_promise.set_exception(std::current_exception());
                    }

                    if (!inner_fut.valid()) {
                        return;
                    }
                    std::unique_lock<std::mutex> lck(inner_fut.m_shared_state->m_then_lck);
                    if (inner_fut.m_shared_state->m_status == FutureStatus::Timeout) {}
                    else {
                        inner_fut.set_callback(
                            [prom](typename TryWrapper<FuncReturnType>::type&& t) mutable {
                                prom.set_value(std::move(t));
                            });
                        if (inner_fut.m_shared_state->m_status == FutureStatus::Done) {
                            try {
                                inner_fut.m_shared_state->m_then(
                                    std::move(inner_fut.m_shared_state->m_value));
                            }
                            catch (const std::exception& exp) {
                                (void)exp;
                                typename TryWrapper<FuncReturnType>::type t(
                                    std::current_exception());
                                inner_fut.m_shared_state->m_then(std::move(t));
                            }
                        }
                    }
                };
                cb();
            });
            if (m_shared_state->m_status == FutureStatus::Done) {
                if (m_shared_state->m_then) {
                    try {
                        m_shared_state->m_then(std::move(m_shared_state->m_value));
                    }
                    catch (const std::exception& exp) {
                        (void)exp;
                        typename TryWrapper<T>::type t(std::current_exception());
                        m_shared_state->m_then(std::move(t));
                    }
                }
            }
        }

        return next_fut;
    }


    void set_callback(std::function<void(typename TryWrapper<T>::type&&)>&& func)
    {
        if (!m_shared_state->m_then) {
            m_shared_state->m_then = std::move(func);
        }
    }

private:
    std::shared_ptr<SharedState<T>> m_shared_state;
};

// 此场景需要类型退化！防止T实例化为&类型,导致 try中含有T类型的成员变量(T
// m_value)，无法实例化m_value！
template<typename T2>
inline Future<std::decay_t<T2>> MakeReadyFuture(T2&& value)
{
    Promise<std::decay_t<T2>> prom;
    prom.set_value(std::forward<T2>(value));
    return prom.get_future();
};

//    编译无法通过
//    template<typename T>
//    Future<T> MakeReadyFuture(T &&value) {
//        Promise<T> prom;
//        prom.set_value(std::forward<T>(value));
//        return prom.get_future();
//    };

inline Future<void> MakeReadyFuture()
{
    Promise<void> prom;
    prom.set_value();
    return prom.get_future();
}

template<typename T2, typename E>
inline Future<T2> MakeExceptionFuture(E&& exp)
{
    Promise<T2> prom;
    prom.set_exception(std::make_exception_ptr(std::forward<E>(exp)));
    return prom.get_future();
}

template<typename T2>
inline Future<T2> MakeExceptionFuture(std::exception_ptr&& exp_ptr)
{
    Promise<T2> prom;
    prom.set_exception(std::move(exp_ptr));
    return prom.get_future();
}

}   // namespace czh
#endif   // CZH_FUTURE_H_
