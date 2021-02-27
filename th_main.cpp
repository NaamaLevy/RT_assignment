
#include <iostream>
#include <dirent.h>
#include <mutex>
#include <cstring>
#include <chrono>
#include "Task.h"
#include "ThreadPool.h"


using namespace std;

int main() {




    DIR *dir = opendir("C:/Users/Naama Levy/Desktop/personal/jobs/greeneye/cpp_project/cmake-build-debug/images");
    int index = 1;
    struct dirent *ent;
    auto t1 = std::chrono::high_resolution_clock::now();
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            char path[] = "images\\";
            char *name = ent->d_name;
            strcat(path, name);
            Task *task = new Task(ent->d_name, index);
            task->run();
            index++;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << duration1 << " milliseconds for Native implementation \n";
    //create a threads_pool
    ThreadPool pool;
    index = 1;
    rewinddir(dir);
    auto t3 = std::chrono::high_resolution_clock::now();
    while ((ent = readdir(dir)) != NULL) {
        //create tasks and add to pool tasks_queue
        if (ent->d_type == DT_REG) {
            char path[] = "images\\";
            char *name = ent->d_name;
            strcat(path, name);
            Task *task = new Task(ent->d_name, index);
            pool.Add_Task(task);
            index++;
        }
    }
    pool.shutdown();
    pool.join();
    auto t4 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>( t4 - t3 ).count();
    std::cout << duration2 << " milliseconds for Multithreading implementation \n";


}





