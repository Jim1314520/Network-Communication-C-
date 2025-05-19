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
    explicit ThreadPool(size_t core_threads = 8, size_t max_threads = 32, size_t max_queue_size = 100);
    ~ThreadPool();

    bool enqueue(std::function<void()> task);

private:
    void worker();
    void try_spawn_worker();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> stop_;

    size_t core_threads_;
    size_t max_threads_;
    size_t max_queue_size_;
    std::atomic<size_t> active_threads_;
};
