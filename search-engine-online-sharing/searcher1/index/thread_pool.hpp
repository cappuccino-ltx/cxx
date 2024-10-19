#pragma once 

#include <iostream>
#include <vector>
#include <string>

#include <pthread.h>


namespace ns_thread{

    class thread_pool{
            static pthread_mutex_t      _mtx;
            std::vector<pthread_t>      _threads;
            static thread_pool*         _pool;
            

            thread_pool(int n)
                :_threads(n)
            {}
            ~thread_pool() {
                for (int i = 0; i < _threads.size(); i++) {
                    pthread_join(_threads[i],nullptr);
                }
            }
        public:
            static thread_pool* get_pool(int n = 5) {
                if(nullptr == _pool) {
                    pthread_mutex_lock(&_mtx);
                    if (nullptr == _pool) {
                        _pool = new thread_pool(n);
                    }
                    pthread_mutex_unlock(&_mtx);
                }

                return _pool;
            }
            void start(void*(*fun)(void*),void* args) {
                for (int i = 0; i < _threads.size(); i++) {
                    pthread_create(&_threads[i],nullptr,fun,args);
                }
            }
            
    };
    pthread_mutex_t      thread_pool::_mtx = PTHREAD_MUTEX_INITIALIZER;
    thread_pool*         thread_pool::_pool = nullptr;
}