
#pragma once

#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include "ringqueue.hpp"


namespace thread{

    using namespace taskspace;

    template<class T>
    struct thread_data{
        thread_data(const std::string& name, RingQueue<T>* tasks, void* other) 
            :_name(name),_tasks(tasks),_other(other)
        {}
        RingQueue<T>* _tasks;
        std::string _name;
        void* _other;
    };

    template<class T>//参数代表要生产消费的任务
    class ThreadPool{

    public:

        typedef void*(*function)(void*);

        static ThreadPool<T>* get(int pro = 10, int con = 10) {
            if (nullptr == _pool) {
                //加锁
                pthread_mutex_lock(&_mtx);
                if (nullptr == _pool) {
                    _pool = new ThreadPool<T>(pro,con);
                }
                pthread_mutex_unlock(&_mtx);
            }
            return _pool;
        }

        void start(function pro,void* pro_arg,function con, void* con_arg) {
            //创建生产者线程
            for (int i = 0; i < _producer.size(); i++) {
                thread_data<T>* data = new thread_data<T>(std::string("生产者线程") + std::to_string(i),&_tasks, pro_arg);
                pthread_create(&(_producer[i]), nullptr, pro, data);
            }
            //消费者线程
            for (int i = 0; i < _consumer.size(); i++) {
                thread_data<T>* data = new thread_data<T>(std::string("消费者线程") + std::to_string(i),&_tasks, con_arg);
                pthread_create(&(_consumer[i]), nullptr, con,data);
            }
        }

    private:
        ThreadPool(int pro,int con)
            :_producer(pro,-1)
            ,_consumer(con,-1)
            ,_tasks((pro + con) * 2)
        {}

    private:
        RingQueue<T> _tasks;                         //任务队列
        std::vector<pthread_t> _producer;         //生产者队列
        std::vector<pthread_t> _consumer;         //消费者队列    
        static ThreadPool<T>* _pool;        //线程池对象
        static pthread_mutex_t _mtx;        //互斥锁   
    };

    template<class T>
    ThreadPool<T>* ThreadPool<T>::_pool = nullptr;
    template<class T>
    pthread_mutex_t ThreadPool<T>::_mtx = PTHREAD_MUTEX_INITIALIZER;
}