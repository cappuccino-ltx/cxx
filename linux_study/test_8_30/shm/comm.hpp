#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdio>

#define PATHNAME "/home/ltx"
#define PROJ_ID 100 
#define SIZE 4096

using namespace std;

static int commshm(int size, int flags){
    key_t _key = ftok(PATHNAME, PROJ_ID);
    if (_key < 0) {
        perror("ftok");
        exit(1);
    }
    int shmid = shmget(_key, size, flags);
    if (shmid < 0) {
        perror("shmget");
        exit(2);
    }
    cout << "共享内存创建/获取成功！" << endl;
    return shmid;
}

int shm_create(){
    return commshm(SIZE, IPC_CREAT | IPC_EXCL | 0666);
}

int shm_get(){
    return commshm(SIZE, IPC_CREAT);
}

int shm_destroy(int shmid){
    if (shmid < 0) {
        return -1;
    }
    return shmctl(shmid,IPC_RMID,nullptr);
}
