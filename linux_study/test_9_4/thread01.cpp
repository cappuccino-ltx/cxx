

#include <iostream>
#include <pthread.h>
#include <unistd.h>


void* fun1(void* arg){
    while(true) {
        sleep(1);
        std::cout << (char*)arg << std::endl;
    }
    return nullptr;
}
void* fun2(void* arg){
     while(true) {
        sleep(1);
        std::cout << (char*)arg << std::endl;
    }
    return nullptr;
}

int main() {
    pthread_t p1,p2;
    //创建并启动线程
    pthread_create(&p1,nullptr,fun1,(void*)"我是线程p1");
    pthread_create(&p2,nullptr,fun2,(void*)"我是线程p2");

    std::cout << "p1:" << p1 << std::endl;
    std::cout << "p2:" << p2 << std::endl;

    while (true);
    //阻塞等待线程结束，并对其进行资源释放
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);

    return 0;
}