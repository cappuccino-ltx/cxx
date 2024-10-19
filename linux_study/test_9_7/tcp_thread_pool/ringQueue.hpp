
#include <iostream>
#include <vector>
#include "sem.hpp"
#include "mutex.hpp"

template<class T>
class ringqueue {

public:
    ringqueue(int capacity = 10)
    :_ring_queue(capacity),
    _start(0),
    _tail(0),
    _space_sem(capacity),
    _data_sem(0),
    _mtx()
    {}

    void push(const T &in){

        _space_sem.p();
        _mtx.lock();
        _ring_queue[_start++] = in;
        _start %= _ring_queue.size();
        _data_sem.v();
        _mtx.unlock();
    }

    void pop(T & out){
        _data_sem.p();
        _mtx.lock();
        out = _ring_queue[_tail++];
        _tail %= _ring_queue.size();
        _space_sem.v();
        _mtx.unlock();
    }

    ~ringqueue()
    {
        
    }

private:
    std::vector<T> _ring_queue;
    int _start;
    int _tail;
    sem _space_sem;
    sem _data_sem;
    mutex _mtx;
};