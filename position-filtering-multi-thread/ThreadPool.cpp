#include "ThreadPool.h"

ThreadPool::ThreadPool()
	: stopFlag(false) {
}

ThreadPool::~ThreadPool() {
	Stop();
}

void ThreadPool::Init(size_t numThreads)
{
	for (size_t i = 0; i < numThreads; ++i) {
		workerThreads.emplace_back(&ThreadPool::WorkerThread, this);
	}
}

void ThreadPool::EnqueueTask(const std::function<void()>& task) {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tasks.push(task);
	}
	condition.notify_one();
}

void ThreadPool::Start() {
	stopFlag.store(false);
}

void ThreadPool::Stop() {
	stopFlag.store(true);
	condition.notify_all();
}

void ThreadPool::WorkerThread() {
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [this] { return stopFlag.load() || !tasks.empty(); });
			if (stopFlag.load() && tasks.empty()) {
				return;
			}
			task = std::move(tasks.front());
			tasks.pop();
		}
		task();
	}
}