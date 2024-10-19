
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <string>

#define THREADNO 2

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int residue = 100;

void *loot(void* arg){
    while (true) {
        pthread_mutex_lock(&mtx);
        if (residue > 0) {
            std::cout << "线程" << (long long)arg + 1 << ":" << residue << " 余票：" << residue - 1 << std::endl;
            residue--;
            usleep(5000);
            //sleep(1);
        }
        else {
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
        usleep(2000);
    }
}

int main() {
    //创建线程
    pthread_t tids[THREADNO];
    for (long long i = 0; i < THREADNO; ++i) {
        if (pthread_create(tids + i, NULL, loot, (void*)i) != 0){
            perror("pthread_create");
            exit(1);
        }
    }
    std::cout << "进程创建完毕" << std::endl;


    for (int i = 0; i < THREADNO; ++i) {
        pthread_join(tids[i],nullptr);
    }

    return 0;
}