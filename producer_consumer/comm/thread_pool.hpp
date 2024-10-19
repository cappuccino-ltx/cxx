#pragma once 

#include <iostream>
#include <vector>
#include <queue>

#include <unistd.h>

#include "thread.hpp"

namespace thread{
    /****
        创建一个生产者消费者模型，
    
    **/
    template<class T>
    struct thread_data{
        thread_data(std::queue<T>* task,pthread_mutex_t* mtx,pthread_cond_t*pro,pthread_cond_t* con,void*other, const std::string& name) 
            :_task(task)
            ,_mtx(mtx)
            ,_name(name)
            ,_pro_cond(pro)
            ,_con_cond(con)
            ,_other(other)
        {}
        std::queue<T>* _task;
        pthread_mutex_t* _mtx;
        std::string _name;
        pthread_cond_t* _pro_cond;
        pthread_cond_t* _con_cond;
        void* _other;
    };

    //生产者
    template<class T>
    void* producer(void* args) {
        thread_data<T>* data = (thread_data<T>*)args;
        //取出data中的数据
        std::queue<T>* task = data->_task;
        pthread_mutex_t* mtx = data->_mtx;
        std::string name = data->_name;
        pthread_cond_t* pro_cond = data->_pro_cond;
        pthread_cond_t* con_cond = data->_con_cond;
        delete data;

        //生产一个数据之后，加锁，将这个数据插入到任务队列中，
        std::cout << name << "启动成功!" << std::endl;

        while(true){
            T newtask;
            newtask.pro();
            pthread_mutex_lock(mtx);
            if(task->size() >= 20) {
                std::cout << name << " :任务队列中任务超过20,延缓生产" << std::endl;
                sleep(1);
                pthread_cond_wait(pro_cond,mtx);
            }
            task->push(newtask);
            //通知消费者，有了新的任务
            //std::cout << "任务队列长度：" << task->size() << std::endl;
            pthread_cond_signal(con_cond);
            pthread_mutex_unlock(mtx);
        }
        return nullptr;
    }
    //消费者
    template<class T>
    void* consumer(void * args) {
        thread_data<T>* data = (thread_data<T>*)args;
        //取出data中的元素
        std::queue<T>* task = data->_task;
        pthread_mutex_t* mtx = data->_mtx;
        std::string name = data->_name;
        pthread_cond_t* pro_cond = data->_pro_cond;
        pthread_cond_t* con_cond = data->_con_cond;
        delete data;                    //释放data

        std::cout << name << "启动成功!" << std::endl;
    
        //加上锁，从任务队列中拿到数据之后，释放锁，执行任务
        while(true){
            pthread_mutex_lock(mtx);
            //消费者拿到锁之后，需要检查队列中是否有元素，没有元素的话，用条件变量释放锁
            while(task->size() == 0) {
                std::cout << name << ":任务队列以空，进入等待" << std::endl;
                pthread_cond_wait(con_cond,mtx);
            }
            T newtask = task->front();
            task->pop();
            //通知生产者进行生产
            pthread_cond_signal(pro_cond);
            pthread_mutex_unlock(mtx);
            //执行任务
            newtask.run();
            //释放任务
        }
        return nullptr;
    }

    template<class T>
    class ThreadPool{
    public:

        //获取线程池对象
        static ThreadPool<T>* get(int pro = 10,int con = 10){
            if (nullptr == _pool){
                pthread_mutex_lock(&_mtx);
                if (nullptr == _pool) {
                    _pool = new ThreadPool<T>(pro,con);
                }
                pthread_mutex_unlock(&_mtx);
            }
            return _pool;
        }

        ~ThreadPool(){
            for (int i = 0; i < _producer.size(); i++) {
                delete _producer[i];
            }
            for (int i = 0; i < _consumer.size(); i++) {
                delete _consumer[i];
            }
        }

        // 线程池的开始函数
        void start(th_function producer_fun,void*pro, th_function consumer_fun,void*con) {
            for (int i = 0; i < _producer.size(); i++) {
                thread_data<T>* data1 = new thread_data<T>(&_task, &_mtx, &_pro_cond, &_con_cond, pro, std::string("生产者线程") + std::to_string(i));
                _producer[i] = new Thread();
                _producer[i]->create(producer_fun,data1);
            }
            for(int i = 0; i < _consumer.size(); i++){
                thread_data<T>* data2 = new thread_data<T>(&_task,&_mtx, &_pro_cond, &_con_cond, con, std::string("消费者线程") + std::to_string(i));
                _consumer[i] = new Thread();
                _consumer[i]->create(consumer_fun,data2);
            }
        }

        static pthread_mutex_t* get_mtx(){return &_mtx;}
        static pthread_cond_t* get_pro_cond(){return &_pro_cond;}
        static pthread_cond_t* get_con_cond() {return &_con_cond;}
        size_t get_task_size() {
            return _task.size();
        }
    
    private:
        ThreadPool(int pro,int con)
            :_producer(pro)
            ,_consumer(con)
        {}

    private:
        std::queue<T> _task;               //任务队列
        std::vector<Thread*> _producer;     //生产者队列
        std::vector<Thread*> _consumer;     //消费者队列
        static ThreadPool<T>* _pool;        //线程池对象指针
        static pthread_mutex_t _mtx;        //锁
        static pthread_cond_t _pro_cond;    //生产者条件变量
        static pthread_cond_t _con_cond;    //消费者条件变量
    };

    //线程池变量的初始化，
    template<class T>
    ThreadPool<T>* ThreadPool<T>::_pool = nullptr;
    //锁的初始化
    template<class T>
    pthread_mutex_t ThreadPool<T>::_mtx = PTHREAD_MUTEX_INITIALIZER;
    //初始化条件变量
    template<class T>
    pthread_cond_t ThreadPool<T>::_pro_cond = PTHREAD_COND_INITIALIZER;
    template<class T>
    pthread_cond_t ThreadPool<T>::_con_cond = PTHREAD_COND_INITIALIZER;
}