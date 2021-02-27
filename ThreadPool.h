
#ifndef CPP_PROJECT_THREADPOOL_H
#define CPP_PROJECT_THREADPOOL_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Task.h"
#include <chrono>

using namespace std;

class ThreadPool {
private:
    vector<thread> pool;
    queue<Task*> queue;
    condition_variable con_task;
    condition_variable con_finished;
    mutex Queue_Mutex;
    bool terminate_pool;
    atomic<int> busy;
public:

    explicit ThreadPool();
    void process();
    virtual ~ThreadPool();
    void Add_Task(Task* task);
    void join();
    void shutdown();
    vector<thread> get_pool();

};


#endif //CPP_PROJECT_THREADPOOL_H
