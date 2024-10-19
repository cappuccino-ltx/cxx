

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <string>

struct data{
    char name;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
};



void* print(void *arg){
    data *d = (data*)arg;
    char name[64];
    snprintf(name, sizeof(name),"线程%c打印--我是线程%c！",d->name,d->name);
    std::string threadname = name;
    
    while (true) {
        pthread_cond_wait(d->cond,d->mtx);
        std::cout << threadname <<std::endl;
        sleep(1);
        pthread_cond_signal(d->cond);
    }
}


int main() {
    pthread_mutex_t mtx;
    pthread_cond_t cond;

    pthread_mutex_init(&mtx,nullptr);
    pthread_cond_init(&cond,nullptr);

    data d1,d2;
    d1.name = 'A';
    d1.mtx = &mtx;
    d1.cond = &cond;
    d2.name = 'B';
    d2.mtx = &mtx;
    d2.cond = &cond;

    pthread_t p1,p2;
    pthread_create(&p1,nullptr,print,(void*)&d1);
    pthread_create(&p2,nullptr,print,(void*)&d2);
    sleep(1);
    pthread_cond_signal(&cond);
    
    pthread_join(p1,nullptr);
    pthread_join(p2,nullptr);
    while (1);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    return 0;
}