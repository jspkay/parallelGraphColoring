//
// Created by antonio_vespa on 23/09/21.
//

#include <iostream>
#include "ThreadPool.h"

using namespace  std;

ThreadPool::ThreadPool() {
    unsigned int num_threads = thread::hardware_concurrency();
    terminate_pool = false;
    stopped = false;
    for (int i = 0; i < num_threads; i++)
        pool.emplace_back(thread([this](){
            /*** endless loop ***/
            while (true)
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this](){
                    return !queue.empty() || terminate_pool;
                });
                if(terminate_pool)
                    return;
                std::cout << "thread in coda: " << queue.size()<<std::endl;
                function<void()> Job = queue.back();
                queue.pop_back();
                lock.unlock();
                Job(); // function<void()> type
            }
        }));
};

void ThreadPool::shutdown() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    terminate_pool = true; // use this flag in condition.wait
    cv.notify_all(); // wake up all threads.
    lock.unlock();
    wait(); //for stopped
    // Join all threads.
    for(std::thread &th : pool)
        th.join();
    pool.clear();
    stopped = true;
}

void ThreadPool::addJob(const function<void()>& new_Job) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue.push_back(new_Job);
    cv.notify_all();
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    cv.wait(lock,[this](){ return stopped; });
};