#pragma once

#include <iostream>
#include <semaphore.h>

class sem{

public:
    sem(int value)
    {
        sem_init(&_sem, 0, value);
    }

    void p(){
        sem_wait(&_sem);
    }

    void v(){
        sem_post(&_sem);
    }

    ~sem(){
        sem_destroy(&_sem);
    }


private:
    sem_t _sem;
};