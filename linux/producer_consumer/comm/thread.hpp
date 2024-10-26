#pragma once 

#include <iostream>
#include <string>

#include <pthread.h>


/*******************************
 *
 *  先实现一个生产者消费者队列，然后然后通过进程通信的方式，将生产者和消费者分开在两个进程中，
 * **/

namespace thread{
    
    //函数的重命名
    typedef void* (*th_function)(void*);
    //线程类
    class Thread{
    public:
        Thread() {}
        ~Thread() {
            pthread_join(_tid,nullptr);
        }
        void create(th_function fun,void* args){
            pthread_create(&_tid,nullptr,fun,args);
        }

    private:
        pthread_t _tid;
    };
}
