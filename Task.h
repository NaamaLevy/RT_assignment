

#ifndef CPP_PROJECT_TASK_H
#define CPP_PROJECT_TASK_H
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <dirent.h>
#include <mutex>
#include <vector>



class Task {
public:
    Task( char *img, int index);
    virtual ~Task();
    std::thread run();
    bool last_one();
private:
    int _index;
    char * _img;
};


#endif //CPP_PROJECT_TASK_H
