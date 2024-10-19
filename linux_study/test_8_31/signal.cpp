

#include <iostream>
#include <signal.h>

using namespace std;

void handler(int signum) {
    cout << "捕获信号：" << signum << endl;
}

int main() {

    signal(SIGINT, handler);

    while (1) {;}
    return 0;
}