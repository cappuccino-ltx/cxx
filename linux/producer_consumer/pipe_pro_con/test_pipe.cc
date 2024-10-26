
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../comm/thread_pool.hpp"
#include "../comm/task.hpp"

using namespace thread;
using namespace taskspace;

static const int slow = 1; // 这个是子进程发送给父进程的信号，让父进程生产的慢一些的标志

//通过，匿名管道进行进程间通信，
// 父进程的线程池作为生产者进行生产，通过匿名管道将生产的任务，给子进程的消费者的线程池，进行消费，
    /***********
        需要注意的是：
            在父子进程间通信的时候，任务不要用new的，任务的传递，不能传递指针，
            不然会引发写实拷贝，父进程会发生严重的内存泄漏问题

        使用互斥锁和条件变量进行资源的互斥和线程之间的同步
    
    
    ****/

void* pipe_producer(void* args){
    //子进程中的生产者，从匿名管道中读取数据，也就是Task的对象
    //取出args中的数据，包括匿名管道的文件描述符
    thread_data<Task>* data = (thread_data<Task>*)args;
    pthread_mutex_t* mtx = data->_mtx;
    pthread_cond_t* pro_cond = data->_pro_cond;
    pthread_cond_t* con_cond = data->_con_cond;
    std::queue<Task>* task = data->_task;
    std::string name = data->_name;
    int fd = *((int*)(data->_other));
    delete data;
    std::cout << "子进程生产者启动成功" << std::endl;

    //注意：这里的生产者是从匿名管道中进行读取的，所以，这里是不需要条件变量等待，
    // 管道中传输了多少任务，就往任务队列中放多少，

    while(true) {
        //从匿名管道中读取任务，插入到队列中，-----------这里不要获取锁之后再堵塞读取，这样的话，消费者线程就拿不到锁了
        Task newtask;
        read(fd,&newtask,sizeof(Task));
        //获取子进程的锁，
        pthread_mutex_lock(mtx);
        task->push(newtask);
        std::cout << "子进程任务队列的长度：" << task->size() << std::endl;
        // 唤醒消费者线程进行消费
        pthread_cond_signal(con_cond);
        // 释放锁
        pthread_mutex_unlock(mtx);
    } 
    return nullptr;
}

void* pipe_consumer(void* args){
    //父进程中的消费者线程，将生产者生产者的任务通过匿名管道给子进程
    //解析args中的数据
    thread_data<Task>* data = (thread_data<Task>*)args;
    pthread_mutex_t* mtx = data->_mtx;
    pthread_cond_t* pro_cond = data->_pro_cond;
    pthread_cond_t* con_cond = data->_con_cond;
    std::string name = data->_name;
    std::queue<Task>* task = data->_task;
    int fd = *((int*)(data->_other));
    delete data;
    std::cout << "父进程消费者启动成功" << std::endl;

    //注意：这里是从生产者的队列中，进行获取任务的，可能会有任务队列为空的情况，所以需要用条件变量进行等待
    while(true) {
        //获取父进程中的锁
        pthread_mutex_lock(mtx);

        while(task->size() == 0) {
            pthread_cond_wait(con_cond,mtx);
        }
        Task newtask = task->front();
        task->pop();

        std::cout << "父进程任务队列的长度：" << task->size() << std::endl;
        //唤醒生产者生产
        pthread_cond_signal(pro_cond);
        //释放锁
        pthread_mutex_unlock(mtx);
        write(fd,&newtask,sizeof(Task));

    }
    return nullptr;
}

//子进程(消费者)监听线程
void* consumer_listener(void*args) {
    int control_fd = *(int*)args;
    //获取线程池对象通过线程池对象获取线程池中的锁，以及任务队列的长度
    ThreadPool<Task>* pool = ThreadPool<Task>::get();
    pthread_mutex_t* mtx = pool->get_mtx();
    pthread_cond_t* con_cond = pool->get_con_cond();
    while(true){    
        pthread_mutex_lock(mtx);
        //如果任务队列中任务的数量超过20，那么通过control的管道，给父进程发送消息，让生产的慢一些
        if(pool->get_task_size() > 20) {
            std::cout << "子进程中的任务队列大于20，发送信号给父进程，" << std::endl;
            write(control_fd,&slow,sizeof(slow));
        }
        //这里注意：抢到锁之后，要唤醒等待的线程，不然别的线程就醒不过来了
        pthread_cond_signal(con_cond);
        pthread_mutex_unlock(mtx);
        //释放锁之后，让监听线程睡眠几秒钟，不然就会一直抢到锁，一直发送
        sleep(1);
    }
    return nullptr;
}

void* producer_listener(void* args) {
    int control_fd = *(int*)args;
    //获取线程池对象，
    ThreadPool<Task>* pool = ThreadPool<Task>::get();
    pthread_mutex_t* mtx = pool->get_mtx();
    pthread_cond_t* pro_cond = pool->get_pro_cond();
    //对control这个管道堵塞读取，如果读到数据，就去抢锁，
    while(true){
        int massage = 0;
        read(control_fd,&massage,sizeof(massage));
        pthread_mutex_lock(mtx);
        std::cout << "父进程收到信号，并且抢到锁，进行休眠" << std::endl;
        //抢到锁之后，睡眠2秒，
        sleep(2);
        //这里也是，需要先唤醒生产者线程，，不然就醒不过来了
        pthread_cond_signal(pro_cond);
        pthread_mutex_unlock(mtx);
    }
    return nullptr;
}

int main() {
    //创建匿名管道
    int fd[2];          //用来传输数据的匿名管道
    int control[2];     //用来控制的匿名管道
    if (pipe(fd) < 0 || pipe(control)) {
        // 匿名管道创建失败
        exit(-1);
    }
    //创建子进程
    pid_t child = fork();
    if (child < 0) {
        // 子进程创建失败
        exit(-2);
    }
    else if (child == 0) {
        //子进程---------子进程作为消费者的角色，应该关闭fd[1]也就是，写端
        //对于控制线程来说，子进程需要写控制信号给父进程，
        // 从而控制父进程的生产速度，所以子进程应该关闭control的读端，也就是control[0],
        close(fd[1]);
        close(control[0]);
        //创建消费者的线程池
        ThreadPool<Task>* pool = ThreadPool<Task>::get(1,5);
        pool->start(pipe_producer,fd,consumer<Task>,nullptr);

        //为子进程设置监听线程
        Thread listener;
        listener.create(consumer_listener,control + 1);

        while(1);
        exit(-3);
    }
    else{
        //父进程---------父进程作为生产者的角色，应该关闭fd[0]也就是，读端
        //对于父进程来说，他需要关注子进程的任务运行情况，来适当减缓自己的任务生产速度
        // 所以父进程应该关闭control的写端，也就是control[1]
        close(fd[0]);
        close(control[1]);
        //创建生产者的线程池
        ThreadPool<Task>* pool = ThreadPool<Task>::get(6,1);
        pool->start(producer<Task>,nullptr,pipe_consumer,fd + 1);

        // 为父进程设置监听
        Thread listener;
        listener.create(producer_listener,control);

        while(1);
        int status = 0;
        waitpid(0,&status,0);
    }
    
    return 0;    
}