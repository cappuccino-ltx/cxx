#pragma once

#include <iostream>
#include <pthread.h>


class mutex{

public:
    mutex(){
        pthread_mutex_init(&_mtx,nullptr);
    }

    void lock(){
        pthread_mutex_lock(&_mtx);
    }

    void unlock(){
        pthread_mutex_unlock(&_mtx);
    }

    ~mutex(){
        pthread_mutex_destroy(&_mtx);
    }


private:
    pthread_mutex_t _mtx;
};