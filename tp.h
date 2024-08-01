#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "pch.h"

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    void enqueueTask(std::function<void()> task);
    void stop();
    
private:
    void workerThread();  // Worker thread function

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
};

#endif