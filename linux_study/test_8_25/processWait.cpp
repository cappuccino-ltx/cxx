

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
    size_t id = fork();
    if (id) {
        //父进程
        int status = 0;
        size_t ret = 0;
        while (!ret) {
            ret = waitpid(id, &status, WNOHANG);
            if (ret == 0) {
                cout << "子进程还未结束！" << endl;
            }
            else if (ret > 0) {
                cout << "子进程已经结束！结束码：" << ((status >> 8) & 0xFF);
                cout << endl;
                ret = 1;
            }else {
                cout << "返回错误！" << endl;
            }
            sleep(1);
        }
    }else {
        //子进程
        int tag = 5;
        while (tag--) {
            cout << "我是子进程！" << endl;
            sleep(1);
        }
        exit(12);
    }

    return 0;
}
