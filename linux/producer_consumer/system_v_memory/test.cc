


#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/***
    用共享内存的方式，进行进程间的通信

*/

const int id = 0x1234;
const char* pathname = "./test";

int main () {
    //获取key值
    key_t key = ftok(pathname,id);
    //获取共享内存
    int size = 4096;//共享内存的大小，单位是bite
    //选项 IPC_CREAT:创建一个共享内存，如果单独使用，那么如果存在的话，就返回这个存在的内存的首地址，如果不存在的话，就创建一个返回，
    //选项 IPC_EXCL:如果这个内存存在的话，返回flse
    int shmid = shmget(key,size,IPC_CREAT | IPC_EXCL | 0666);
    if (shmid < 0) {
        std::cout << "创建共享内存失败" << std::endl;
        exit(-1);
    }
    //第二个参数可以让这个共享内存，映射到你想映射的地址上，配合第三个参数中的SHM_RND,让内存对齐到整数位置上
    //第三个参数，可以设置SHM_RDONLY,只读权限
    void* shmptr = shmat(shmid,nullptr,0);//加载共享内存到当前程序中，（将共享内存映射到当前进程的地址空间）

    shmdt(shmptr);

    //对共享内存进行释放
    sleep(5);
    shmctl(shmid,IPC_RMID,nullptr);
    return 0;
}