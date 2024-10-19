#pragma once

#include "thread.hpp"
#include "Task.hpp"
#include <vector>
#include <queue>
//#include <cstdlib>
#include <ctime>
#include <unistd.h>
#define CAPACITY 10

int add(int x, int y)
{
    return x + y;
}

template <class T, class D>
struct poolData
{
    int _capacity;
    // 生产着队列
    //std::vector<Thread *> *_productor;
    // 消费者队列
    //std::vector<Thread *> *_consumer;
    // 阻塞队列
    //线程自身
    Thread* _self;
    std::queue<Task<T, D> *> *_task;
    // 锁
    pthread_mutex_t *_mtx;
    // 条件变量
    pthread_cond_t *_full;  // 满了
    pthread_cond_t *_empty; // 空了
};

template <class T, class D>
class Pool
{
public:
    Pool(int num) : _capacity(10)
    {
        _productor = new std::vector<Thread *>(num);
        _consumer = new std::vector<Thread *>(num);
        _task = new std::queue<Task<T, D> *>;
        pthread_mutex_init(&_mtx, nullptr);
        pthread_cond_init(&_full, nullptr);
        pthread_cond_init(&_empty, nullptr);
    }

    std::vector<Thread *> *getpro()
    {
        return _productor;
    }
    std::vector<Thread *> *getcon()
    {
        return _consumer;
    }
    std::queue<Task<T, D> *> *gettask()
    {
        return _task;
    }

    void strat()
    {
        //poolData<T, D> prodata[_productor->size()];
        //poolData<T, D> condata[_productor->size()];
        std::vector<poolData<T, D>> prodata(_productor->size());
        std::vector<poolData<T, D>> condata(_productor->size());
        for (int i = 0; i < _productor->size(); ++i)
        {
            //data[i]._consumer = _consumer;
            //data[i]._productor = _productor;
            prodata[i]._task = _task;
            prodata[i]._full = &_full;
            prodata[i]._empty = &_empty;
            prodata[i]._mtx = &_mtx;
            prodata[i]._capacity = _capacity;
            (*_productor)[i] = new Thread(i);
            prodata[i]._self = (*_productor)[i];
            (*_productor)[i]->create(productor, &prodata[i]);

            condata[i]._task = _task;
            condata[i]._full = &_full;
            condata[i]._empty = &_empty;
            condata[i]._mtx = &_mtx;
            condata[i]._capacity = _capacity;
            (*_consumer)[i] = new Thread(i);
            condata[i]._self = (*_consumer)[i];
            (*_consumer)[i]->create(consumer, &condata[i]);
        }
        for (int i = 0; i < _productor->size(); ++i)
        {
            (*_productor)[i]->join();
            (*_consumer)[i]->join();
        }
        for (int i = 0; i < _productor->size(); ++i)
        {
            delete (*_productor)[i];
            delete (*_consumer)[i];
        }
    }

    // 生产者
    static void *productor(void *args)
    {
        poolData<T, D> *pd = (poolData<T, D> *)args;
        //std::vector<Thread *> *pro = pd->_productor;
        // 消费者队列
        // std::vector<Thread*> *con = pd->_consumer;
        Thread* self = pd->_self;
        // 阻塞队列
        std::queue<Task<T, D> *> *task = pd->_task;
        // 锁
        pthread_mutex_t *mtx = pd->_mtx;
        // 条件变量
        pthread_cond_t *full = pd->_full;   // 满了
        pthread_cond_t *empty = pd->_empty; // 空了
        int capacity = pd->_capacity;
        srand((uint64_t)time(nullptr) ^ 0x6666);
        while (true)
        {
            int x = rand() % 1000 + 10;
            int y = rand() % 1000 + 10;

            pthread_mutex_lock(mtx);
            // 如果满了，就挂起
            while (task->size() == capacity){
                std::cout << "生产者挂起！" << std::endl;
                pthread_cond_wait(full, mtx);
            }
                
            
            std::cout << "生产者拿到锁，开始生产"<< std::endl;
            // 添加任务
            task->push(new Task<int (*)(int, int), int>(add, x, y));
            std::cout << "生产者" << self->name() << " 生产：" << x << "+" << y << "=?" << std::endl;
            usleep(rand()%3000);
            std::cout << "生产者生产完毕，释放锁，唤醒线程"<< std::endl;
            pthread_cond_signal(empty);
            pthread_mutex_unlock(mtx);
            

            
            sleep(1);
        }
    }
    // 消费者
    static void *consumer(void *args)
    {
        poolData<T, D> *pd = (poolData<T, D> *)args;
        // std::vector<Thread*>* pro = pd->_productor;
        // 消费者队列
        //std::vector<Thread *> *con = pd->_consumer;
        Thread* self = pd->_self;
        // 阻塞队列
        std::queue<Task<T, D> *> *task = pd->_task;
        // 锁
        pthread_mutex_t *mtx = pd->_mtx;
        // 条件变量
        pthread_cond_t *full = pd->_full;   // 满了
        pthread_cond_t *empty = pd->_empty; // 空了

        while (true)
        {

            pthread_mutex_lock(mtx);
            while (task->empty()) {
                std::cout << "消费者挂起！" << std::endl;
                pthread_cond_wait(empty, mtx);
            }
            std::cout << "消费者拿到锁，开始消费"<< std::endl;
            // 执行任务
            // task->push(Task(add,x,y));
            Task<T, D> *t = task->front();
            task->pop();
            D num = t->fun()(t->arg0(), t->arg1());
            std::cout << "消费者" << self->name() << " 消费：" << t->arg0() << "+" << t->arg1() << "=" << num << std::endl;
            delete t;
            usleep(rand()%3000);
            std::cout << "消费者消费完毕，释放锁，唤醒线程"<< std::endl;

            pthread_cond_signal(full);
            pthread_mutex_unlock(mtx);
            

            //usleep(rand() % 2000);
            sleep(2);
        }
    }

    ~Pool()
    {
        if (!_productor->empty())
        {
            for (int i = 0; i < _productor->size(); ++i)
            {
                if ((*_productor)[i] != nullptr)
                {
                    delete (*_productor)[i];
                }
            }
        }
        if (!_consumer->empty())
        {
            for (int i = 0; i < _consumer->size(); ++i)
            {
                if ((*_consumer)[i] != nullptr)
                {
                    delete (*_consumer)[i];
                }
            }
        }

        while (!_task->empty())
        {
            Task<T,D> *t = _task->front();
            _task->pop();
            delete t;
        }

        delete _productor;
        delete _consumer;
        delete _task;
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_full);
        pthread_cond_destroy(&_empty);
    }

private:
    int _capacity;
    // 生产着队列
    std::vector<Thread *> *_productor;
    // 消费者队列
    std::vector<Thread *> *_consumer;
    // 阻塞队列
    std::queue<Task<T, D> *> *_task;
    // 锁
    pthread_mutex_t _mtx;
    // 条件变量
    pthread_cond_t _full;  // 满了
    pthread_cond_t _empty; // 空了
};
