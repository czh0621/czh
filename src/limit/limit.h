#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
namespace czh {
namespace rpc {
class SimpleLimit {
 private:
  uint64_t m_threshold;
  std::chrono::milliseconds m_window_time;
  std::atomic<uint64_t> count;

 public:
  bool try_pass() {
    count++;
    if (count <= m_threshold) {
      return true;
    }
    return false;
  }

  void start() {
    std::thread t([this]() {
      while (true) {
        count = 0;
        std::this_thread::sleep_for(m_window_time);
      }
    });
    t.detach();
  }

  void set_threshold(const std::chrono::milliseconds& time_ms,
                     const uint64_t& threshold) {
    m_threshold = threshold;
    m_window_time = time_ms;
  }
};

class SlideWindowLimit {
 public:
  struct Window {
    int64_t window_start_time_ms{0};
    int64_t count{0};
    void reset_window(int64_t start_time_ms) {
      window_start_time_ms = start_time_ms;
      count = 0;
    }
    void add(int64_t num) { count = num + count; }
  };

  SlideWindowLimit(int64_t threshold, int64_t total_slide_window_size,
                   int64_t window_num)
      : m_threshold(threshold),
        m_total_slide_window_size(total_slide_window_size),
        m_window_num(window_num) {
    m_window_size = total_slide_window_size / window_num;
    m_current_window_time = 0;
    m_slide_window_array.resize(window_num, nullptr);
    for (int64_t i = 0; i < window_num; i++) {
      m_slide_window_array[i] = std::make_shared<Window>();
    }
  }

  bool try_pass() {
    std::unique_lock<std::mutex> lck(m_mutex);
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now);
    // why now.count return nano time?
    calculate_windows(now.count() / 1000);
    auto current_count = get_window_success();
    if (current_count < m_threshold) {
      m_slide_window_array[m_current_index]->add(1);
      return true;
    }
    return false;
  }

  void calculate_windows(int64_t time_stamp) {
    auto new_window_index = (time_stamp / m_window_size) % m_window_num;
    auto new_window_start_time_ms = time_stamp - time_stamp % m_window_size;
    auto old_window = m_slide_window_array[new_window_index];
    if (old_window &&
        (new_window_start_time_ms > (old_window->window_start_time_ms))) {
      m_current_window_time = new_window_start_time_ms;
      old_window->reset_window(new_window_start_time_ms);
      m_current_index = new_window_index;
    }
  }
  int64_t get_window_success() {
    int64_t sum = 0;
    auto first_window_start_time_ms =
        m_current_window_time - m_total_slide_window_size + m_window_size;
    for (auto window : m_slide_window_array) {
      if ((window->window_start_time_ms) >= first_window_start_time_ms) {
        sum += window->count;
      }
    }
    return sum;
  }

 private:
 private:
  int64_t m_total_slide_window_size;
  int64_t m_window_size;
  int64_t m_threshold;
  int64_t m_window_num;
  int64_t m_current_window_time;
  int64_t m_current_index;
  std::mutex m_mutex;
  std::vector<std::shared_ptr<Window>> m_slide_window_array;
};

}  // namespace rpc
}  // namespace czh
