#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h>


namespace taskspace{
    template<class T>
    class RingQueue{
    public:
        RingQueue(int num = 30) 
            :_tasks(num)
            ,_front(0)
            ,_rear(0)
        {
            std:: cout << "debug:ringqueue的长度是" << num << std::endl;
            // 对信号量进行初始化
            if (sem_init(&_space_sem, 0, num) != 0) std::cout << "_space_sem初始化失败" <<std::endl;
            if (sem_init(&_data_sem, 0, 0) != 0)std::cout << "_space_sem初始化失败" <<std::endl;
            // 参数2如果是0代表线程间通信，非零代表进程间通信
            //对互斥锁进行初始化
            pthread_mutex_init(&_mtx,nullptr);
        }
        ~RingQueue() {
            // 对信号量进行释放
            sem_destroy(&_space_sem);
            sem_destroy(&_data_sem);
            pthread_mutex_destroy(&_mtx);
        }

        size_t size() {

            return (_front - _rear + _tasks.size()) % _tasks.size();
        }

        //往循环队列中插入数据
        void push(T task) {
            int pro_index = -1;
            //P操作-------空间的p
            sem_wait(&_space_sem);

            //加锁
            pthread_mutex_lock(&_mtx);
            //获取下标，维护下标
            pro_index = _front++;
            _front %= _tasks.size();

            //解锁
            pthread_mutex_unlock(&_mtx);

            //进行资源的插入
            _tasks[pro_index] = task;

            // V操作------数据的v
            sem_post(&_data_sem);
        }

        // 释放循环队列中的数据
        void pop(T& task) {
            int con_index = -1;
            //进行资源的P操作
            sem_wait(&_data_sem);

            //加锁，对下标进行获取
            pthread_mutex_lock(&_mtx);

            //获取下标，维护下标
            con_index = _rear++;
            _rear %= _tasks.size();

            //解锁
            pthread_mutex_unlock(&_mtx);
            // 进行资源的返回
            task = _tasks[con_index];

            //进行空间的V操作
            sem_post(&_space_sem);
        }

    private:
    //     头插尾出
        std::vector<T> _tasks;       //资源队列
        int _front;                  //生产位置
        int _rear;                   //消费位置
        sem_t _space_sem;           //POSIX信号量 ---空间剩余
        sem_t _data_sem;            //POSIX信号量 ---数据剩余
        pthread_mutex_t _mtx;       //互斥锁---------------让循环队列有一个锁，是为了保证在多个线程修改下标的时候，造成的冲突
    };
}