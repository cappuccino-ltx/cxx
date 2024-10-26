



#include "fifo.h"
#include "../comm/task.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>


/**********

    用命名管道的方式，进行进程之间的通信，
        pro进程有一个多线程的生产者模型，生产出来的任务通过命名管道传给消费者进程，
        con进程有一个多线程的消费者模型，获取从pro进程获取到的任务，进行执行

    用互斥锁来控制共享资源的互斥，用信号量来控制线程之间的同步

***/

using namespace thread;
using namespace taskspace;

//生产进程的生产者，只负责生产任务
void* producer(void*args) {
    thread_data<Task>* data = (thread_data<Task>*)args;
    RingQueue<Task>* tasks = data->_tasks;
    std::string name = data->_name;
    //other
    delete data;

    //由于使用的是信号量控制的环形队列，所以不需要再插入的时候加锁
    while(true) {
        Task task;
        task.pro();//调用任务的生产函数
        tasks->push(task);
        std::cout << "生产者进程中，任务队列的长度：" << tasks->size() << std::endl;
    }

    return nullptr;
}

//生产进程的消费者，将队列中的任务用命名管道传输给消费者进程
void* consumer(void*args) {
    thread_data<Task>* data = (thread_data<Task>*)args;
    RingQueue<Task>* tasks = data->_tasks;
    std::string name = data->_name;
    int fd = *(int*)(data->_other);
    delete data;


    while(true) {
        //线程间的同步有信号量控制了，只需要将任务取出来，交给消费者进程就可以了
        Task task;
        tasks->pop(task);
        std::cout << "生产者进程中，任务队列的长度：" << tasks->size() << std::endl;
        write(fd,&task,sizeof(task));
    }

    return nullptr;
}


int main() {
    //得先启动生产者进程，创建管道文件
    umask(0);
    if (mkfifo(filename.c_str(), 0666) < 0) {
        std::cout << "创建管道文件失败" << std::endl;
        exit(-1);
    }
    //打开命名管道-------生产者进程，用只写的方式打开文件
    int fd = open(filename.c_str(),O_WRONLY);
    if (fd < 0) {
        std::cout << "打开命名管道失败" << std::endl;
    }
    //创建线程池
    ThreadPool<Task>* pool = ThreadPool<Task>::get(5,1);
    pool->start(producer,nullptr,consumer,&fd);

    //让主线程死循环，维持线程的运行
    while(1);

    return 0;
}