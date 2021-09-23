//
// Created by antonio_vespa on 23/09/21.
//

#ifndef GRAPHCOLORING_THREADPOOL_H
#define GRAPHCOLORING_THREADPOOL_H

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "MyPackagedTask.h"

using namespace  std;

class ThreadPool {
    vector<thread> pool;
    mutex queue_mutex;
    condition_variable cv;
    bool terminate_pool, stopped;
    vector< std::function<void()> > queue;
public:
    ThreadPool();
    ~ThreadPool();
    void shutdown();
    void wait();
    void addJob(const function<void()>& new_Job);
};


#endif //GRAPHCOLORING_THREADPOOL_H
