#pragma once

#include "thread.hpp"
#include "Task.hpp"
#include "ringQueue.hpp"
//#include <vector>
//#include <queue>
//#include <cstdlib>
#include <ctime>
#include <unistd.h>
//#define CAPACITY 10

int add(int x, int y)
{
    return x + y;
}

template <class T, class D>
struct poolData
{
    Thread* _self;
    ringqueue<Task<T,D>*>* _rq;
};

template <class T, class D>
class Pool
{
public:
    Pool(int num) 
    :_productor(num),
    _consumer(num)
    {
        
    }

    

    void strat()
    {
        poolData<T,D> prodata[_productor.size()];
        poolData<T,D> condata[_consumer.size()];
        for (int i = 0; i < _productor.size(); ++i) {
            _productor[i] = new Thread(i);
            prodata[i]._self = _productor[i];
            prodata[i]._rq = &_rq;
            _productor[i]->create(productor,&prodata[i]);
            _consumer[i] = new Thread(i);
            condata[i]._self = _consumer[i];
            condata[i]._rq = &_rq;
            _consumer[i]->create(consumer,&condata[i]);
        }

        for (int i = 0; i < _productor.size(); ++i) {
            _productor[i]->join();
            delete _productor[i];
            _consumer[i]->join();
            delete _consumer[i];
        }
    }

    // 生产者
    static void *productor(void *args)
    {
        poolData<T,D> *pd = (poolData<T,D>*)args;
        Thread *self = pd->_self;
        ringqueue<Task<T,D>*> *rq = pd->_rq;
        srand(time(nullptr) ^ 0x6666);
        while (true) {
            int x = rand() % 1000 + 10;
            int y = rand() % 1000 + 10;
            rq->push(new Task<T,D>(add,x,y));
            std::cout << "生产线程" << self->name() << "生产任务：" << x << "+" << y << "=?" << std::endl;
            usleep(rand() %5000 + 3000);
            //sleep(1);
        }
    }
    // 消费者
    static void *consumer(void *args)
    {
        poolData<T,D> *pd = (poolData<T,D>*)args;
        Thread *self = pd->_self;
        ringqueue<Task<T,D>*> *rq = pd->_rq;
        srand(time(nullptr) ^ 0x6666);
        while (true) {
            Task<T,D> *t;
            rq->pop(t);
            int z = t->fun()(t->arg0(),t->arg1());
            std::cout << "消费者" << self->name() << "消费任务：" << t->arg0() << "+" << t->arg1() << "=" << z << std::endl;

            delete t;
            usleep(rand() % 5000 + 3000);
        }
    }

    ~Pool()
    {
        
    }

private:
    ringqueue<Task<T,D>*> _rq;
    std::vector<Thread*> _productor;
    std::vector<Thread*> _consumer;
};
