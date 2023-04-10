#if !defined(_PROMISE_H)
#define _PROMISE_H_

#include "shared_state.h"

namespace czh {

    template<typename T>
    class Future;

    template<typename T>
    class Promise {
    public:
        Promise() : m_shared_state(std::make_shared<SharedState<T>>()) {}

        Promise(const Promise &) = default;

        Promise(Promise &&) = default;

        Promise &operator=(const Promise &) = default;

        Promise &operator=(Promise &&) = default;

        // void 无法转发？no matching function for call to ‘forward<>()’
//        template<typename... Args>
//        void set_value(Args &&...args) {
//            set_value_internal(std::forward<Args>(args)...);
//        }

        //这里采用模板+直接函数重载的方式
        template<typename SHIT = T, typename = typename std::enable_if<
                !std::is_void<SHIT>::value, SHIT>::type>
        void set_value(SHIT &&value) {
            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
            if (m_shared_state->m_status != FutureStatus::None ||
                m_shared_state->m_status == FutureStatus::Timeout ||
                m_shared_state->m_status == FutureStatus::Done) {
                return;
            };
            m_shared_state->m_status = FutureStatus::Done;
            m_shared_state->m_value = std::forward<SHIT>(value); //类型转换 在赋值
            m_shared_state->notify_one();
            lck.unlock();
            if (m_shared_state->m_then) {
                m_shared_state->m_then(std::move(m_shared_state->m_value));
            }
        }

        void set_value() {
            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
            if (m_shared_state->m_status != FutureStatus::None ||
                m_shared_state->m_status == FutureStatus::Timeout ||
                m_shared_state->m_status == FutureStatus::Done) {
                return;
            };
            m_shared_state->m_status = FutureStatus::Done;
            m_shared_state->m_value = Try<void>();
            m_shared_state->notify_one();
            lck.unlock();
            if (m_shared_state->m_then) {
                m_shared_state->m_then(std::move(m_shared_state->m_value));
            }
        }

        void set_exception(std::exception_ptr &&e) {
            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
            if (m_shared_state->m_status != FutureStatus::None ||
                m_shared_state->m_status == FutureStatus::Timeout ||
                m_shared_state->m_status == FutureStatus::Done) {
                return;
            };
            m_shared_state->m_status = FutureStatus::Done;
            m_shared_state->m_value = typename TryWrapper<T>::type(std::move(e));
            m_shared_state->notify_one();
            lck.unlock();
            if (m_shared_state->m_then) {
                m_shared_state->m_then(std::move(m_shared_state->m_value));
            }
        }

        // 这里采用函数模板来重载，这种方法可读性更好,根本原因是void类型与T&& 不匹配,void类型没有copy 和 move
//        template<typename SHIT = T, typename = typename std::enable_if<
//                !std::is_void<SHIT>::value, SHIT>::type>
//        void set_value(SHIT &&value) {
//            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
//            if (m_shared_state->m_status != FutureStatus::None ||
//                m_shared_state->m_status == FutureStatus::Timeout ||
//                m_shared_state->m_status == FutureStatus::Done) {
//                return;
//            };
//            m_shared_state->m_status = FutureStatus::Done;
//            m_shared_state->m_value = std::forward<SHIT>(value);
//            m_shared_state->notify_one();
//            lck.unlock();
//            if (m_shared_state->m_then) {
//                m_shared_state->m_then(std::move(m_shared_state->m_value));
//            }
//        }

//        template<typename SHIT = T, typename = typename std::enable_if<
//                std::is_void<SHIT>::value, void>::type>
//        void set_value() {
//            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
//            if (m_shared_state->m_status != FutureStatus::None ||
//                m_shared_state->m_status == FutureStatus::Timeout ||
//                m_shared_state->m_status == FutureStatus::Done) {
//                return;
//            };
//            m_shared_state->m_status = FutureStatus::Done;
//            m_shared_state->m_value = Try<void>();
//            m_shared_state->notify_one();
//            lck.unlock();
//            if (m_shared_state->m_then) {
//                m_shared_state->m_then(std::move(m_shared_state->m_value));
//            }
//        }

        // 这里采用返回值的方式来重载
//        template<typename SHIT = T>
//        typename std::enable_if<!std::is_void<SHIT>::value, void>::type set_value(
//                SHIT &&value) {
//            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
//            if (m_shared_state->m_status != FutureStatus::None ||
//                m_shared_state->m_status == FutureStatus::Timeout ||
//                m_shared_state->m_status == FutureStatus::Done) {
//                return;
//            };
//            m_shared_state->m_status = FutureStatus::Done;
//            m_shared_state->m_value = std::forward<SHIT>(value);
//            m_shared_state->notify_one();
//            lck.unlock();
//            if (m_shared_state->m_then) {
//                m_shared_state->m_then(std::move(m_shared_state->m_value));
//            }
//        }
//
//        template<typename SHIT = T>
//        typename std::enable_if<std::is_void<SHIT>::value, void>::type set_value() {
//            std::unique_lock<std::mutex> lck(m_shared_state->m_then_lck);
//            if (m_shared_state->m_status != FutureStatus::None ||
//                m_shared_state->m_status == FutureStatus::Timeout ||
//                m_shared_state->m_status == FutureStatus::Done) {
//                return;
//            };
//            m_shared_state->m_status = FutureStatus::Done;
//            m_shared_state->m_value = Try<void>();
//            m_shared_state->notify_one();
//            lck.unlock();
//            if (m_shared_state->m_then) {
//                m_shared_state->m_then(std::move(m_shared_state->m_value));
//            }
//        }

        Future<T> get_future() {
            bool expect = false;
            if (!m_shared_state->m_retrived.compare_exchange_strong(expect, true)) {
                std::runtime_error("promise value has been retrieved!");
            }
            return Future<T>(m_shared_state);
        }

    private:
        std::shared_ptr<SharedState<T>> m_shared_state;
    };
}  // namespace czh

#endif  // _PROMISE_H_
