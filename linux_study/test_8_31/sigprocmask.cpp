
#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main() {

    sigset_t set, oset;
    sigemptyset(&set);
    sigemptyset(&oset);

    //使用sigprocmask函数阻塞2号信号和40号信号
    sigaddset(&set, 2);
    sigaddset(&set, 40);

    //SIG_BLOCK表示添加对set中的信号的屏蔽 ，相当于mask = mask | set，
    //SIG_UNBLOCK表示解除对set中的信号进行的屏蔽 mask = mask &~ set
    //SIG_SETMASK表示直接使用set中的信号屏蔽 mask = set
    sigprocmask(SIG_BLOCK, &set, &oset);

    while (1) {
        cout << "进程正在运行！" << getpid() << endl;
        sleep(1);
    }
    return 0;
}