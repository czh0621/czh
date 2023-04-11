//#include "../../src/threadpool/threadpool.h"

#include "threadpool.h" //简写形式 需要cmake中指定头文件搜索路径

int add(int x, int y) {
    return x + y;
}

void test_thread_pool() {
    czh::core::threadpool pool(4);
    for (int i = 0; i < 50; i++) {
        pool.enqueue(std::bind(&add, 1, 2));
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "size" << pool.get_task_size() << std::endl;
    pool.print_thread();
}

void test_join() {
    // 调用join时 会等待子线程处理完任务，才会回收子线程，这里子线程while循环未推出 无法回收，一直等待！
    std::thread t([]() {
        while (true) {};
        std::cout << "while exit" << std::endl;
    });
    if (t.joinable()) {
        t.join();
        std::cout << "thread join" << std::endl;
    }
}

void test_move_thread() {
    std::thread t([]() { std::cout << 11 << std::endl; });
    // std::thread t2(t);
    std::thread t2(std::move(t));
}

int main() {
    test_thread_pool();
    // test_join();
    return 0;
}