#include "ThreadPool.h"


ThreadPool:: ThreadPool(): Queue_Mutex(),
                           queue(), con_task(),con_finished(), pool(), terminate_pool(),busy(0)
{
    int num_of_threads = thread::hardware_concurrency();
    for(int ii = 0; ii < num_of_threads; ii++) {
        pool.push_back(thread(&ThreadPool::process, this));
    }
}

void ThreadPool:: process(){

    while(true)
    {
        unique_lock<mutex> lock(Queue_Mutex);
        con_task.wait(lock, [this](){return terminate_pool || !queue.empty();});
        if (!queue.empty())
        {
            ++busy;
            Task *task = queue.front();
            queue.pop();
            lock.unlock();
            try{
                task->run();
            }catch (std::exception& e){
                cout<<"running task, with exception..."<<e.what()<<endl;
                return;
            }
            lock.lock();
            --busy;
            con_finished.notify_all();
        }
        else if (terminate_pool)
        {
            return;
        }
    }
}

void ThreadPool::Add_Task(Task* task) {
    {
        unique_lock<mutex> lock(Queue_Mutex);
        queue.push(task);
    }
    con_task.notify_one();
}

ThreadPool::~ThreadPool() {}


void ThreadPool::join() {
    for (auto& th: pool){
        th.join();
    }
}

void ThreadPool::shutdown() {
    unique_lock<mutex> lock(Queue_Mutex);
    terminate_pool = true;
    lock.unlock();
    con_task.notify_all(); // wake up all threads.
}




