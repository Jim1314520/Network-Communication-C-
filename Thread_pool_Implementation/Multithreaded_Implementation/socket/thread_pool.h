#pragma once
#pragma message(">>> included thread_pool.h <<<")
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = 8);
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    void worker();  // 每个工作线程执行的函数

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> stop_;
};
