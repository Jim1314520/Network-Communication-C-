#include "thread_pool.h"
#include <iostream>
ThreadPool::ThreadPool(size_t thread_count) : stop_(false)
{
    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back([this] { this->worker(); });
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

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
        std::cout << "[ThreadPool] Task enqueued, current queue size: " << tasks_.size() << std::endl;
    }
    cond_.notify_one();
}

void ThreadPool::worker()
{
    while (!stop_) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty()) return;

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();  // 执行任务
    }
}
