

#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void mydeamon() {
    //忽略信号
    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);

    //不要让自己成为组长
    if (fork() != 0) exit(0);

    //调用setsid();
    setsid();
    //标准输入、标准输出、标准错误全部关闭
    int devnull= open("/dev/null", O_RDONLY | O_WRONLY);
    if (devnull > 0) {
        dup2(0, devnull);
        dup2(1, devnull);
        dup2(2, devnull);
        close(devnull);
    }
}