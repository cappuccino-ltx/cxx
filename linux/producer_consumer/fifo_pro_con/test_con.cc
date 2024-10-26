

#include "fifo.h"
#include "../comm/task.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

using namespace taskspace;
using namespace thread;

//消费者进程，负责从生产者进程中获取任务数据，进行任务的执行

//作为消费者进程的生产者，任务就是将生产者线程传来的数据，放到队列中，
void* producer(void* args) {

    thread_data<Task>* data = (thread_data<Task>*)args;
    std::string name  = data->_name;
    RingQueue<Task>* tasks = data->_tasks;
    int fd = *(int*) (data->_other);

    delete data;
    while(true) {
        //从命名管道中读取任务，放到任务队列中
        //由于进程之间的同步是由信号量控制的，所以不需要加锁
        Task task;
        read(fd,&task,sizeof(task));
        tasks->push(task);
        std::cout << "消费者进程的任务队列长度"  << tasks->size() << std::endl;
    }
    return nullptr;
}

//作为消费者进程的消费者，任务就是执行任务，进行消费
void* consumer(void*args) {

    thread_data<Task>* data = (thread_data<Task>*)args;
    std::string name  = data->_name;
    RingQueue<Task>* tasks = data->_tasks;

    while(true) {
        //从队列中获取任务，进行执行
        Task task;
        tasks->pop(task);
        task.run();
        std::cout << "消费者进程的任务队列长度"  << tasks->size() << std::endl;
    }


    return nullptr;    
}

int main() {

    //由于消费者线程是之后启动的，所以管道应该已经创建好了
    //打开管道文件
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cout << "消费者进程打开管道文件失败" << std::endl;
        exit(-1);
    }
    //获取线程池
    ThreadPool<Task>* pool = ThreadPool<Task>::get(1,5);
    pool->start(producer,&fd, consumer, nullptr);

    //让主程序进入死循环
    while(1);

    return 0;
}