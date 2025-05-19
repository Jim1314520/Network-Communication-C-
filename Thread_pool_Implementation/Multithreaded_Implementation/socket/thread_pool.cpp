#include "thread_pool.h"
#include <iostream>
#include <chrono>

ThreadPool::ThreadPool(size_t core_threads, size_t max_threads, size_t max_queue_size)
    : stop_(false), core_threads_(core_threads), max_threads_(max_threads), max_queue_size_(max_queue_size), active_threads_(0)
{
    for (size_t i = 0; i < core_threads_; ++i) {
        workers_.emplace_back([this] { this->worker(); });
        ++active_threads_;
    }
}

ThreadPool::~ThreadPool()
{
    stop_ = true;
    cond_.notify_all();
    for (auto& t : workers_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

bool ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.size() >= max_queue_size_) {
            std::cerr << "[ThreadPool] ❌ Task queue full. Rejecting task." << std::endl;
            return false;
        }
        tasks_.push(std::move(task));
        std::cout << "[ThreadPool] Task enqueued, current queue size: " << tasks_.size() << std::endl;
    }

    cond_.notify_one();
    try_spawn_worker();
    return true;
}

void ThreadPool::try_spawn_worker()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (active_threads_ < max_threads_ && tasks_.size() > active_threads_) {
        workers_.emplace_back([this] { this->worker(); });
        ++active_threads_;
        std::cout << "[ThreadPool] ⚙️ Spawning additional worker. Total active: " << active_threads_ << std::endl;
    }
}

void ThreadPool::worker()
{
    while (!stop_) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (!cond_.wait_for(lock, std::chrono::seconds(10), [this] { return stop_ || !tasks_.empty(); })) {
                if (active_threads_ > core_threads_) {
                    --active_threads_;
                    std::cout << "[ThreadPool] 💤 Extra worker exiting due to idleness. Active threads: " << active_threads_ << std::endl;
                    return;
                }
                continue;
            }

            if (stop_ && tasks_.empty()) return;

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}
