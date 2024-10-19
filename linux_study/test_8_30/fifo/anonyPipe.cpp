#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
using namespace std;
int main() {
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    assert(n == 0);
    (void)n;
    int fd = fork();
    if (fd < 0) {
        perror("fork");
        return 1;
    }
    if  (fd == 0) {
        //这是子进程，用来接收数据,关闭用以写入数据的端口
        close(pipefd[1]);
        char buff[1024];
        memset(buff, '\0', sizeof(buff));
        while (read(pipefd[0], buff, sizeof(buff)) > 0) {
            
            cout << "子进程接收到的内容：" << buff << endl;
        }
        close(pipefd[0]);
        exit(1);
    }
    //这是父进程，需要关闭读端端口
    close(pipefd[0]);
    for (int i = 0; i < 10; ++i) {
        write(pipefd[1], "hello ltx!", 10);
        sleep(1);
    }
    close(pipefd[1]);
    return 0;

}