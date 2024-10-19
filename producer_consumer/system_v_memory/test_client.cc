
#include "comm.h"
#include <unistd.h>

int main() {

    //测试一下共享内存通信
    
    //获取key值
    key_t key = ftok(pathname,id);

    //获取共享内存
    int shmid =  shmget(key,size, IPC_CREAT);
    if (shmid < 0) {
        std::cout  << "共享内存创建失败" << std::endl;
    }

    //加载共享内存到当前程序
    char* shmptr = (char*)shmat(shmid, nullptr, 0);

    //进行通信
    for (int i = 0; i < 10; i++) {
        shmptr[i] = 'a' + i;
        sleep(1);
    }


    //共享内存去关联
    shmdt(shmptr);

    //释放共享内存-----------------由服务端进行关闭
    //shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}