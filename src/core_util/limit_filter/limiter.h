/**
 * @file       limiter.h
 * @date       2023/9/11
 * @since      2023-09-11
 * @author     czh
 */

#ifndef CZH_LIMITER_H_
#define CZH_LIMITER_H_
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>
namespace czh { namespace limiter {
    class SimpleLimit
    {
    private:
        uint64_t                                   m_threshold;
        std::chrono::milliseconds                  m_window_time;
        std::atomic<uint64_t>                      count;
        decltype(std::chrono::system_clock::now()) m_current_time{std::chrono::system_clock::now()};

    public:
        bool try_pass()
        {
            auto period    = std::chrono::system_clock::now() - m_current_time;
            auto ms_peroid = std::chrono::duration_cast<std::chrono::milliseconds>(period);
            spdlog::info("count {} time_period:{}", count, ms_peroid.count());
            if (ms_peroid > m_window_time) {
                count.store(0);
                m_current_time = std::chrono::system_clock::now();
                spdlog::info("reset count");
            }
            count++;
            return count <= m_threshold;
        }

        void set_threshold(const std::chrono::milliseconds& time_ms, const uint64_t& threshold)
        {
            m_threshold   = threshold;
            m_window_time = time_ms;
        }
    };

    class SlideWindowLimit
    {
    public:
        struct Window
        {
            int64_t window_start_time_ms{0};
            int64_t count{0};
            void    reset_window(int64_t start_time_ms)
            {
                window_start_time_ms = start_time_ms;
                count                = 0;
            }
            void add(int64_t num) { count = num + count; }
        };

        SlideWindowLimit(int64_t threshold, int64_t total_slide_window_size, int64_t window_num)
            : m_threshold(threshold)
            , m_total_slide_window_size(total_slide_window_size)
            , m_window_num(window_num)
        {
            m_window_size         = total_slide_window_size / window_num;
            m_current_window_time = 0;
            m_slide_window_array.resize(window_num, nullptr);
            for (int64_t i = 0; i < window_num; i++) {
                m_slide_window_array[i] = std::make_shared<Window>();
            }
        }
        // 判断当前时间戳落点
        bool try_pass()
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            auto                         now = std::chrono::system_clock::now().time_since_epoch();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now);
            calculate_windows(now.count() / 1000);
            auto current_count = get_window_success();
            if (current_count < m_threshold) {
                m_slide_window_array[m_current_index]->add(1);
                return true;
            }
            return false;
        }

        void calculate_windows(int64_t time_stamp)
        {
            auto new_window_index         = (time_stamp / m_window_size) % m_window_num;
            auto new_window_start_time_ms = time_stamp - time_stamp % m_window_size;
            auto old_window               = m_slide_window_array[new_window_index];
            if (old_window && (new_window_start_time_ms > (old_window->window_start_time_ms))) {
                m_current_window_time = new_window_start_time_ms;
                old_window->reset_window(new_window_start_time_ms);
                m_current_index = new_window_index;
            }
        }
        int64_t get_window_success()
        {
            int64_t sum = 0;
            auto    first_window_start_time_ms =
                m_current_window_time - m_total_slide_window_size + m_window_size;
            for (const auto& window : m_slide_window_array) {
                if ((window->window_start_time_ms) >= first_window_start_time_ms) {
                    sum += window->count;
                }
            }
            return sum;
        }
        // no lock
        bool try_acqurie()
        {
            auto    time      = std::chrono::steady_clock::now().time_since_epoch();
            auto    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
            auto    current_index = (timestamp % m_total_slide_window_size) / m_window_size;
            int64_t sum           = 0;
            for (int i = 0; i < m_window_num; i++) {
                if ((timestamp - m_slide_window_array[i]->window_start_time_ms) >
                    m_total_slide_window_size) {
                    m_slide_window_array[i]->reset_window(timestamp);
                }
                if (current_index == i) {
                    m_slide_window_array[i]->add(1);
                }
                sum += m_slide_window_array[i]->count;
            }
            return sum <= m_threshold;
        }


    private:
        int64_t                              m_total_slide_window_size;
        int64_t                              m_window_size;
        int64_t                              m_threshold;
        int64_t                              m_window_num;
        int64_t                              m_current_window_time;
        int64_t                              m_current_index{0};
        std::mutex                           m_mutex;
        std::vector<std::shared_ptr<Window>> m_slide_window_array;
    };

}}       // namespace czh::limiter
#endif   // CZH_LIMITER_H_
