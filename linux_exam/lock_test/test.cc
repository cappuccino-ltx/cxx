

#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

void* func1(void* args) {
    pthread_mutex_t* mtxs = (pthread_mutex_t*)args;
    //线程1开始的时候直接获取锁1
    pthread_mutex_lock(mtxs + 0);
    
    //睡眠1秒之后，获取锁2
    sleep(1);
    pthread_mutex_lock(mtxs + 1);

    //睡眠一秒之后对锁2解锁
    sleep(1);
    pthread_mutex_unlock(mtxs + 1);
    
    //对锁1进行解锁
    pthread_mutex_unlock(mtxs + 0);
    return nullptr;
}
void* func2(void* args) {
    pthread_mutex_t* mtxs = (pthread_mutex_t*)args;
    //线程2开始的时候直接获取锁2
    pthread_mutex_lock(mtxs + 1);
    
    //睡眠1秒之后，获取锁1
    sleep(1);
    pthread_mutex_lock(mtxs + 0);

    //睡眠一秒之后对锁1解锁
    sleep(1);
    pthread_mutex_unlock(mtxs + 0);
    
    //对锁2进行解锁
    pthread_mutex_unlock(mtxs + 1);
    return nullptr;   
}


int main() {
    //定义互斥锁
    pthread_mutex_t mtxs[2];;
    //初始化互斥锁
    pthread_mutex_init(mtxs + 0,nullptr);
    pthread_mutex_init(mtxs + 1,nullptr);
    pthread_t id1 = -1;
    pthread_t id2 = -1;
    pthread_create(&id1,nullptr,func1,mtxs);
    pthread_create(&id2,nullptr,func2,mtxs);

    pthread_join(id1,nullptr);
    pthread_join(id2,nullptr);
    //对锁进行释放
    pthread_mutex_destroy(mtxs + 0);
    pthread_mutex_destroy(mtxs + 1);
    return 0;
}
