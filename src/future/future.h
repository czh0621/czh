#if !defined(_FUTURE_H)
#define _FUTURE_H

#include "helper.h"
#include "promise.h"

namespace czh {
    template<typename T>
    class Future {
    public:
        Future(std::shared_ptr<SharedState<T>> ptr) : m_shared_state(ptr) {}

        Future(const Future &) = delete;

        Future(Future &&) = default;

        Future &operator=(const Future &) = delete;

        Future &operator=(Future &&) = default;

        typename SharedState<T>::ValueType wait(
                const std::chrono::milliseconds &time = std::chrono::milliseconds(24 *
                                                                                  3600 *
                                                                                  1000)) {
            try {
                std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
                switch (m_shared_state->m_status) {
                    case FutureStatus::None:
                        /* code */
                        break;
                    case FutureStatus::Timeout:
                        throw std::runtime_error("FutureStatus Timeout！");

                    case FutureStatus::Done:
                        m_shared_state->m_status = FutureStatus::Retrived;
                        return std::move(m_shared_state->m_value);

                    default:
                        throw std::runtime_error("Future already retrieved !");
                }
            } catch (const std::exception &e) {
                (void) e;
                return typename SharedState<T>::ValueType(std::current_exception());
            }
            if (m_shared_state->wait_for(time) == FutureStatus::Timeout) {
                return typename SharedState<T>::ValueType(std::make_exception_ptr(
                        std::runtime_error("Future wait_for timeout")));
            } else {
                return typename SharedState<T>::ValueType(
                        std::move(m_shared_state->m_value));
            }
        }

        template<typename F, typename R = CallableResult<F, T>>
        auto then(F &&f) -> typename CallableResult<F, T>::ReturnFutureType {
            using Args = typename CallableResult<F, T>::Return_type;
            return then_impl<F, R>(nullptr, std::forward<F>(f), Args{});
        }

    private:
        template<typename F, typename R, typename... Args>
        auto then_impl(void *, F &&f, ResultOfWrapper<F, Args...>) ->
        typename std::enable_if_t<!R::IsFutureReturnType::value,
                typename R::ReturnFutureType> {
            using real_return_type = typename R::Return_type::type;
            Promise<real_return_type> promise;
            auto fut = promise.get_future();
            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
            if (m_shared_state->m_status == FutureStatus::Timeout) {
            } else {
                set_callback([prom = std::move(promise), func = std::forward<F>(f)](
                        typename TryWrapper<T>::type &&t) mutable {
                    auto res = invoke_with_TryWrapper(func, std::move(t));
                    prom.set_value(res);
                });

                if (m_shared_state->m_status == FutureStatus::Done) {
                    if (m_shared_state->m_then) {
                        try {
                            m_shared_state->m_then(std::move(m_shared_state->m_value));
                        } catch (const std::exception &e) {
                            m_shared_state->m_then(
                                    typename TryWrapper<T>::type(std::current_exception()));
                        }
                    }
                }
            }
            return fut;
        }

//  template <typename F, typename R, typename... Args>
//  auto then_impl(void *, F &&f, ResultOfWrapper<F, Args...>) ->
//      typename std::enable_if_t<R::IsFutureReturnType::value,
//                                typename R::ReturnFutureType> {
//    using real_return_type = typename R::IsFutureReturnType::inner_type;
//    Promise<real_return_type> prom;
//    auto fut = prom.get_future();
//    std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
//    if (m_shared_state->m_status == FutureStatus::Timeout) {
//    } else {
//      set_callback(
//          prom = std::move(promise),
//          func = std::forward<F>(f)(typename TryWrapper<T>::type && t) {
//            auto cb = [func = std::move(func), prom = std::move(prom),
//                       t = std::move(t)]() mutable {
//              decltype(func(t.template get<Args>()...)) inner_fut;
//              if (t.has_exception()) {
//                inner_fut = func(t.excpetion());
//              } else {
//                inner_fut = func(t.template get<Args>()...);
//              }
//              std::unique_lock<std::mutex> lck(
//                  inner_fut.m_shared_state->m_then_lck);
//              if (inner_fut.m_shared_state->m_status == FutureStatus::Timeout) {
//              } else {
//                inner_fut.set_callback(
//                    [prom = std::move(prom)](
//                        typename TryWrapper<real_return_type>::type &&t) {
//                      prom.set_value(std::move(t));
//                    });
//                if (inner_fut.m_shared_state->m_status == FutureStatus::Done) {
//                  try {
//                    inner_fut.m_shared_state->m_then(
//                        std::move(inner_fut.m_shared_state->m_value));
//                  } catch (const std::exception &exp) {
//                    inner_fut.m_shared_state->m_then(
//                        typename TryWrapper<real_return_type>::type t(
//                            std::current_exception()););
//                  }
//                }
//              }
//            };
//            cb();
//          });
//    }
//    return fut;
//  }
        void set_callback(
                std::function<void(typename TryWrapper<T>::type &&)> &&func) {
            if (!m_shared_state->m_then) {
                m_shared_state->m_then = std::move(func);
            }
        }

    private:
        std::shared_ptr<SharedState<T>> m_shared_state;
    };

// 此场景需要类型退化！防止T实例化为&类型 try 中含有T 类型的成员变量(T &
// m_value)！
    template<typename T>
    Future<std::decay_t<T>> MakeReadyFuture(T &&value) {
        Promise<std::decay_t<T>> prom;
        prom.set_value(std::forward<T>(value));
        return prom.get_future();
    };

    template<>
    Future<void> MakeReadyFuture() {
        Promise<void> prom;
        prom.set_value();
        return prom.get_future();
    }

    template<typename T, typename E>
    Future<T> MakeExceptionFuture(E &&exp) {
        Promise<T> prom;
        prom.set_exception(std::make_exception_ptr(std::forward<E>(exp)));
        return prom.get_future();
    }

    template<typename T>
    Future<T> MakeExceptionFuture(std::exception_ptr &&exp_ptr) {
        Promise<T> prom;
        prom.set_exception(std::move(exp_ptr));
        return prom.get_future();
    }

}  // namespace czh

#endif  // _FUTURE_H
