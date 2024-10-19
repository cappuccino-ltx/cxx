

#include "comm.hpp"


int main() {
    //创建共享内存
    int shmid = shm_create();
    
    //将共享内存挂到进程中
    char* addr = (char*)shmat(shmid, NULL, 0);
    //cout << addr << endl;
    sleep(2);
    
    //char a = 'a';
    //addr[0] = '\0';
    int i = 26;
    while (i--) {
        cout << "循环读取：";
        printf("client# %s\n", addr);
        sleep(1);
    }
    cout << "读取完毕" << endl;
    shmdt((void*)addr);
    sleep(2);
    int n = shm_destroy(shmid);
    if (n < 0) {
        perror("shm_destroy");
        exit(1);
    }
    
    return 0;
}