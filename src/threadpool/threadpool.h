#if !defined(_THREAD_POOL_H)
#define _THREAD_POOL_H
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <functional>
#include <deque>
#include <future>
#include <type_traits>
#include <memory>
#include <exception>
#include <atomic>

namespace czh
{
  namespace core
  {
    class threadpool
    {
    private:
      // thread_local static bool m_working;
      bool m_stop;
      size_t m_pool_size;
      int m_current_thread_size;
      std::atomic<int> m_wait_thread_size{0};
      std::unordered_map<std::thread::id, std::thread> m_thread_map;
      std::deque<std::function<void()>> m_task_deque;
      std::mutex m_mutex;
      std::condition_variable m_condition_variable;

    private:
      bool need_add_thread();
      void add_thread();
      void worker_consume_task();

    public:
      void print_thread();
      void recycle();
      int get_task_size();

    public:
      explicit threadpool(size_t pool_size = std::thread::hardware_concurrency());
      ~threadpool()
      {
        recycle();
      };

      // 有返回值
      template <typename F, typename... Args>
      auto enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
      {
        using return_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        /*  std::packaged_task<return_type()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...)); */
        if (m_stop)
        {
          std::runtime_error("threadpool stop do not enqueue task!");
        }
        std::unique_lock<std::mutex> lck(m_mutex);
        if (need_add_thread())
        {
          add_thread();
        }
        m_task_deque.push_back([task]()
                               { (*task)(); });

        /*    m_task_deque.push_back([inner_task = std::move(task)]() mutable
                                  { inner_task(); }); */

        m_condition_variable.notify_one();
        return task->get_future();
      }
    };
  }

}

#endif // _THREAD_POOL_H
