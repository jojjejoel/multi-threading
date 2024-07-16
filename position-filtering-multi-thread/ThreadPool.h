#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();
    void Init(size_t numThreads);

    void EnqueueTask(const std::function<void()>& task);
    void Start();
    void Stop();

public:
    const size_t GetNumThreads() const { return workerThreads.size(); }


private:
    void WorkerThread();

private:
    std::vector<std::jthread> workerThreads;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
};
