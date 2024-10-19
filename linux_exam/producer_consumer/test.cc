

#include <iostream>
#include <queue>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

struct thread_data{
    thread_data(pthread_mutex_t* mtx, pthread_cond_t* con_pro, pthread_cond_t* con_con, std::queue<int>* tasks)
        :_mtx(mtx),_con_pro(con_pro), _con_con(con_con),_tasks(tasks)
     {}
    pthread_mutex_t* _mtx;
    pthread_cond_t* _con_pro;
    pthread_cond_t* _con_con;
    std::queue<int>* _tasks;
};

void* pro_fun(void* args) {
    thread_data* data = (thread_data*)args;
    pthread_mutex_t* mtx = data->_mtx;
    pthread_cond_t* con_pro = data->_con_pro;
    pthread_cond_t* con_con = data->_con_con;
    std::queue<int>* tasks = data->_tasks;
    delete data;
    srand(time(nullptr));    
    while(1) {
        //生产者每一秒生产1个任务
        sleep(1);
        int num = rand();
        //获取锁
        pthread_mutex_lock(mtx);
        while(tasks->size() > 20){
            pthread_cond_wait(con_pro,mtx);
        }
        std::cout << "生产者生产任务：" << num << std::endl;
        tasks->push(num);
        //发信号让消费者消费
        pthread_cond_signal(con_con);
        //解锁
        pthread_mutex_unlock(mtx);
    }
    return nullptr;
}
void* con_fun(void* args) {
    thread_data* data = (thread_data*)args;
    pthread_mutex_t* mtx = data->_mtx;
    pthread_cond_t* con_pro = data->_con_pro;
    pthread_cond_t* con_con = data->_con_con;
    std::queue<int>* tasks = data->_tasks;
    delete data;
    while(1) {
        //消费者消费，
        //获取锁
        pthread_mutex_lock(mtx);
        while(tasks->size() == 0){
            pthread_cond_wait(con_con,mtx);
        }
        int num = tasks->front();
        tasks->pop();
        std::cout << "消费者消费任务：" << num << std::endl;

        //发信号让生产者生产
        pthread_cond_signal(con_pro);
        //解锁
        pthread_mutex_unlock(mtx);
    }
    return nullptr;
}

int main() {
    //创建互斥锁，条件变量
    pthread_mutex_t mtx;
    pthread_cond_t con_pro;
    pthread_cond_t con_con;
    //对互斥锁和条件变量进行初始化
    pthread_mutex_init(&mtx,nullptr);
    pthread_cond_init(&con_pro,nullptr);
    pthread_cond_init(&con_con,nullptr);

    //创建任务队列
    std::queue<int> tasks;

    pthread_t produceres[3];
    pthread_t consumer;
    //创建消费者线程s
    for (int i = 0; i < 3; i++) {
        pthread_create(produceres + i,nullptr,pro_fun,new thread_data(&mtx,&con_pro,&con_con,&tasks));
    }
    pthread_create(&consumer,nullptr,con_fun,new thread_data(&mtx,&con_pro,&con_con,&tasks));

    //对线程进行等待
    for(int i = 0; i < 3; i++) {
        pthread_join(produceres[i],nullptr);
    }
    pthread_join(consumer,nullptr);

    //释放互斥锁和条件变量
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&con_pro);
    pthread_cond_destroy(&con_con);
    return 0;
}