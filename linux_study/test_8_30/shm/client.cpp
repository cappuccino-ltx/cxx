

#include "comm.hpp"


int main() {
    //创建共享内存
    int shmid = shm_get();
    //将共享内存挂到进程中
    char* addr = (char*)shmat(shmid, nullptr, 0);
    sleep(2);

    char a = 'a';
    while (a <= 'z') {
        addr[a - 'a'] = a;
        addr[a - 'a' + 1] = '\0';
        a++;
        sleep(1);
    }
    
    shmdt((void*)addr);
    sleep(2);
    return 0;
}