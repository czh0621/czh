#if !defined(_SHARED_STATE_H)
#define _SHARED_STATE_H
#include <memory.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>

#include "try.h"

namespace czh {
enum class FutureStatus {
  None,
  Done,
  Timeout,
  Retrived,
};

template <typename T>
class SharedState {
 public:
  using ValueType = typename TryWrapper<T>::type;
  SharedState()
      : m_status(FutureStatus::None), m_retrived(false), m_blocked(false) {}

  void notify_one() {
    if (m_blocked) {
      m_condition.notify_one();
    }
  }

  void wait() {
    std::unique_lock<std::mutex> lck(m_then_lck);
    m_blocked = true;
    m_condition.wait(lck, [this]() { return m_status != FutureStatus::None; });
  }

  template <typename Clock, typename Duration>
  FutureStatus wait_until(
      const std::chrono::duration<Clock, Duration> &timeout_time) {
    std::unique_lock<std::mutex> lck(m_then_lck);
    m_blocked = true;
    bool ret = m_condition.wait_until(
        lck, timeout_time, [this]() { return m_status != FutureStatus::None; });
    if (!ret) {
      m_status = FutureStatus::Timeout;
    }
    return m_status;
  }

  template <typename Rep, typename Duration>
  FutureStatus wait_for(
      const std::chrono::duration<Rep, Duration> &timeout_duration) {
    std::unique_lock<std::mutex> lck(m_then_lck);
    m_blocked = true;
    bool ret = m_condition.wait_for(lck, timeout_duration, [this]() {
      return m_status != FutureStatus::None;
    });
    if (!ret) {
      m_status = FutureStatus::Timeout;
    }
    return m_status;
  }

 public:
  std::mutex m_then_lck;
  std::condition_variable m_condition;
  std::function<void(ValueType &&)> m_then;
  ValueType m_value;
  FutureStatus m_status;
  bool m_blocked;
  std::atomic<bool> m_retrived;
};
}  // namespace czh

#endif  // _SHARED_STATE_H
