

#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void printpending(sigset_t* s){
    int i = 31;
    for (; i > 0; --i) {
        cout << sigismember(s, i);
    }
    cout << endl;
}

int main() {

    sigset_t set, pending;
    //清空初始化
    sigemptyset(&set);
    //将ctrl + c 2号信号添加入阻塞信号集
    sigaddset(&set, SIGINT);
    //设置信号集
    sigprocmask(SIG_BLOCK, &set, nullptr);

    while (1) {
        sigpending(&pending);
        printpending(&pending);
        sleep(1);
    }


    return 0;
}