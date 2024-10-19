#pragma once

#include "thread.hpp"
//#include "Task.hpp"
#include "ringQueue.hpp"
//#include <vector>
//#include <queue>
//#include <cstdlib>
#include <ctime>
#include <unistd.h>



template <class T>
struct poolData
{
    Thread* _self;
    ringqueue<T>* _rq;
};

template <class T>
class Pool
{

public:
    static Pool<T>* getpool(int num = 10){
        if (nullptr == _pool) {
            pthread_mutex_lock(&mtx);
            if (nullptr == _pool) {
                _pool = new Pool<T>(num);
            }
            pthread_mutex_unlock(&mtx);
        }
        return _pool;
    }

private:
    Pool(int num) 
    :_consumer(num),
    _rq(10)
    {}
    Pool(const Pool& pool) = delete;
    Pool& operator=(const Pool& pool) = delete;
public:
    void strat()
    {
        poolData<T> condata[_consumer.size()];
        for (int i = 0; i < _consumer.size(); ++i) {
            _consumer[i] = new Thread(i);
            condata[i]._self = _consumer[i];
            condata[i]._rq = &_rq;
            _consumer[i]->create(consumer,&condata[i]);
        }
    }

    // 生产者
    void pushTask(T task)
    {
        _rq.push(task);
    }
    // 消费者
    static void *consumer(void *args)
    {
        poolData<T> *pd = (poolData<T>*)args;
        Thread *self = pd->_self;
        ringqueue<T> *rq = pd->_rq;
        std::cout << self->name() << " Successfully started!" << std::endl;
        while (true) {
            T t;
            rq->pop(t);
            (*t)();
            delete t;
        }
    }

    ~Pool()
    {
        for (int i = 0; i < _consumer.size(); ++i) {
            _consumer[i]->join();
            delete _consumer[i];
        }
        
    }

private:
    
    ringqueue<T> _rq;
    std::vector<Thread*> _consumer;
    static pthread_mutex_t mtx;
    static Pool<T>* _pool;
};

template<class T>
Pool<T>* Pool<T>::_pool = nullptr;

template<class T>
pthread_mutex_t Pool<T>::mtx = PTHREAD_MUTEX_INITIALIZER;
