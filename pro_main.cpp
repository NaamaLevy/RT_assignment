
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <dirent.h>
#include <mutex>
#include <cstring>
#include <chrono>
#include "Task.h"
#include "ThreadPool.h"
#include "boost/interprocess/managed_shared_memory.hpp"
#include "boost/interprocess/containers/vector.hpp"
#include "boost/interprocess/allocators/allocator.hpp"
#include <string>
#include <cstdlib> //std::system

using boost::container::vector;
using namespace std;
using namespace boost::interprocess;

typedef boost::interprocess::allocator<Task, managed_shared_memory::segment_manager>  ShmemAllocator;
typedef boost::interprocess::vector<Task, ShmemAllocator> TaskVector;//shm_tasks_vector
struct shm_obj { //struct for shm_objects
    boost::interprocess::interprocess_mutex mutex;
    bool last_one;
};
static const std::size_t process_count = 4; //# of processes to run

int main(){
    auto t1 = std::chrono::high_resolution_clock::now(); //set timer for process
    for(size_t i = 0; i<process_count; i++){
        pid_t  pid = fork(); //create child processes
        if (pid > 0) {  //Parent process
            std::cout << "In parent" << std::endl;
            //Remove shared memory on construction and destruction
            struct shm_remove
            {
                shm_remove() { shared_memory_object::remove("MutexMemory"); }
                ~shm_remove() { shared_memory_object::remove("MutexMemory"); }
            } remover;
            shared_memory_object::remove("VectorMemory");
            //Create a managed shared memory segment
            managed_shared_memory segment
                    (create_only
                            , "VectorMemory" //segment name
                            , 65536);          //segment size in bytes


            const ShmemAllocator alloc_inst(segment.get_segment_manager());

            shared_memory_object shm(create_only, "MutexMemory", read_write);


            TaskVector* taskvector =
                    segment.construct<TaskVector>("TaskVector") //object name
                            (alloc_inst);//first ctor parameter

            shm.truncate(1000);

            mapped_region region(shm, read_write);
            void* addr = region.get_address();
            shm_obj* mu = new (addr) shm_obj();
            mu->last_one = false;


            //create tasks and add to pool tasks_queue
            DIR *dir = opendir("C:\\Users\\Naama Levy\\Desktop\\personal\\jobs\\greeneye\\cpp_project\\cmake-build-debug\\images");
            int index = 1;
            struct dirent *ent;
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    //            cout << ent->d_name << "\n";
                    scoped_lock <interprocess_mutex> lock(mu->mutex);
                    char path[] = "images\\";
                    char *name = ent->d_name;
                    strcat(path, name);
                    Task task = Task(ent->d_name, index);
                    taskvector->push_back(task);
                    if (index == 10) {
                        mu->last_one = true;
                    }
                    index++;
                }
            }
        }
        else if (pid == 0){
            std::cout << "In child" << std::endl;
            //Open already created shared memory object.
            shared_memory_object shm(open_only, "MutexMemory", read_write);

            //Map the whole shared memory in this process
            mapped_region region(shm, read_write);
            shm_obj *mu = (shm_obj *) region.get_address();
            managed_shared_memory segment
                    (open_only, "VectorMemory");  //segment name

            TaskVector *taskvector = segment.find<TaskVector>("TaskVector").first;
            condition_variable con_task;

            while (true) { //child run tasks in loop
                unique_lock <interprocess_mutex> lock(mu->mutex);
                if (!taskvector->empty()) {
                    Task task = taskvector->front();
                    try {
                        task.run();
                    } catch (std::exception &e) {
                        cout << "running task, with exception..." << e.what() << endl;
                        return 1;
                    }

                    taskvector->erase(taskvector->begin());
                }
                else if (mu->last_one = true) { //no tasks in queue
                    return(0);
                }
            }
        }
        else{ //failed to fork
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();//calculate time for process
    std::cout << duration;
    return 0;
}

//int main() {


//    auto t1 = std::chrono::high_resolution_clock::now();
//    //create a threads_pool
//    ThreadPool pool;
//
//    //create tasks and add to pool tasks_queue
//    DIR *dir = opendir("C:\\Users\\Naama Levy\\Desktop\\personal\\jobs\\greeneye\\cpp_project\\cmake-build-debug\\images");
//    int index = 1;
//    struct dirent *ent;
//    while ((ent = readdir(dir)) != NULL) {
//        if (ent->d_type == DT_REG) {
////            cout << ent->d_name << "\n";
//            char path[] = "images\\";
//            char *name = ent->d_name;
//            strcat(path, name);
//            Task *task = new Task(ent->d_name, index);
//            pool.Add_Task(task);
//            index++;
//        }
//    }
//    pool.join();
//    pool.shutdown();
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
//    std::cout << duration;
////    pool.waitFinished();
//    exit(0);

//}





