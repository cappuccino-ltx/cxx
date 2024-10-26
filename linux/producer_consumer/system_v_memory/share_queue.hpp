#pragma once


#include <iostream>

namespace share{

    template <class T>
    class ShareQueue{


    private:
        //因为这个类需要定位new到共享空间上，我们需要将所有的资源都设置成静态的，使他的大小刚好和共享空间一样，就在共享内存初始化的时候，直接吧


    private:
        T _task[];          //T类型的数组，存放任务的数组
        int _front;         //生产的位置
        int _rear;          //消费的位置

        //需要一个进程锁，来控制进程之间锁对下标的访问和修改
        pthread_mutex_t _mtx;

        int _sem;           //system v 的信号量，控制进程之间的同步
    };
}