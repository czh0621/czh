/**
 * @file       thread_pool.h
 * @date       2023/9/11
 * @since      2023-09-11
 * @author     czh
 */

#ifndef CZH_THREAD_POOL_H_
#define CZH_THREAD_POOL_H_

#include "../../future/future.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <unordered_map>

// 优化 http://www.chunel.cn/archives/cgraph-threadpool-3-introduce
// MT 1.监听线程（自动扩容与回收）2.批量拉取task 3.切片任务，将任务放入不同的任务队列，每个线程各自处理自己的任务队列
// 4 任务偷取（限定范围偷取）
namespace czh { namespace core {

    class ThreadPool
    {
    private:
        // thread_local static bool m_working;
        bool                                             m_stop;
        size_t                                           m_pool_size;
        int                                              m_current_thread_size;
        std::atomic<int>                                 m_wait_thread_size{0};
        std::unordered_map<std::thread::id, std::thread> m_thread_map;
        std::deque<std::function<void()>>                m_task_deque;
        std::mutex                                       m_mutex;
        std::condition_variable                          m_condition_variable;

    private:
        bool need_add_thread();

        void add_thread();

        void worker_consume_task();

    public:
        void print_thread();

        void recycle();

        int get_task_size();

    public:
        explicit ThreadPool(size_t pool_size = std::thread::hardware_concurrency());

        ~ThreadPool() { recycle(); };

        // 有返回值
        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args)
            -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using return_type = decltype(f(args...));
            auto task         = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            /*  std::packaged_task<return_type()> task(std::bind(std::forward<F>(f),
             * std::forward<Args>(args)...)); */
            if (m_stop) {
                throw std::runtime_error("ThreadPool stop do not enqueue task!");
            }
            std::unique_lock<std::mutex> lck(m_mutex);
            if (need_add_thread()) {
                add_thread();
            }
            m_task_deque.push_back([task]() { (*task)(); });

            /*    m_task_deque.push_back([inner_task = std::move(task)]() mutable
                                      { inner_task(); }); */

            m_condition_variable.notify_one();
            return task->get_future();
        }

        template<typename F, typename... Args>
        auto add(F&& f, Args&&... args) -> typename czh::Future<std::result_of_t<F(Args...)>>
        {
            using ReturnType = typename std::result_of_t<F(Args...)>;
            Promise<ReturnType> promise;
            auto                fut  = promise.get_future();
            auto                task = [prom = std::move(promise),
                         func = std::forward<F>(f)](Args&&... args) mutable {
                auto res = func(std::forward<Args>(args)...);
                prom.set_value(res);
            };
            if (m_stop) {
                throw std::runtime_error("ThreadPool stop do not enqueue task!");
            }
            std::unique_lock<std::mutex> lck(m_mutex);
            if (need_add_thread()) {
                add_thread();
            }
            m_task_deque.push_back(task);
            m_condition_variable.notify_one();
            return fut;
        }
    };
}}   // namespace czh::core


#endif   // CZH_THREAD_POOL_H_
