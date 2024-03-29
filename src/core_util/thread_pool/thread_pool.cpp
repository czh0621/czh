#include "thread_pool.h"

// thread_local bool ThreadPool::m_working = true;

using namespace czh::core;

ThreadPool::ThreadPool(size_t pool_size)
{
    m_pool_size           = pool_size;
    m_current_thread_size = 0;
    m_stop                = false;
}

void ThreadPool::recycle()
{
    m_stop = true;
    m_condition_variable.notify_all();
    for (auto& item : m_thread_map) {
        if (item.second.joinable()) {
            std::cout << "recycle thread id :" << item.first << std::endl;
            item.second.join();
        }
    }
    std::cout << "recycle finish" << std::endl;
}

int ThreadPool::get_task_size()
{
    return m_task_deque.size();
}

bool ThreadPool::need_add_thread()
{
    if (m_current_thread_size == 0 ||
        (m_wait_thread_size == 0) && (m_current_thread_size < m_pool_size)) {
        return true;
    }
    return false;
}

void ThreadPool::add_thread()
{
    m_current_thread_size++;
    /* std::thread t([this]()
                  { this->worker_consume_task(); }); */
    std::thread t(&ThreadPool::worker_consume_task, this);
    /*  m_thread_map[t.get_id()] = std::move(t); */
    m_thread_map.emplace(std::make_pair(t.get_id(), std::move(t)));
}

void ThreadPool::worker_consume_task()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            m_wait_thread_size++;
            m_condition_variable.wait(
                lck, [this]() { return !this->m_task_deque.empty() || this->m_stop; });
            m_wait_thread_size--;
            if (this->m_stop) {
                // break;
                std::cout << "exit thread" << std::endl;
                return;
            }
            task = m_task_deque.front();
            m_task_deque.pop_front();
        }
        task();
    }
}

void ThreadPool::print_thread()
{
    for (auto& item : m_thread_map) {
        std::cout << "print thread id:" << item.first << std::endl;
    }
}