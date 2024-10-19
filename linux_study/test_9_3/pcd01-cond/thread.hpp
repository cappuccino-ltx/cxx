#pragma once

#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdio>
//#include <functional>


class Thread{

    //typedef std::function<void* (void*)> function;
    typedef void*(*function)(void*);

public:
    Thread(){}
    Thread(int num):_tid(0)
    {
        char name[64];
        snprintf(name, sizeof(name),"thread%d",num);
        _name = name;
    }

    void create(function fun,void* arg){
        pthread_create(&_tid, nullptr, fun, arg);
    }

    void join() {
        pthread_join(_tid,nullptr);
    }

    std::string name(){
        return _name;
    }

    ~Thread(){}

private:
    pthread_t _tid;
    std::string _name;
};

