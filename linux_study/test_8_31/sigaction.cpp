
#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void sigcd(int signum){
    cout << "捕获了信号：" << signum << endl;
}

int main() {

    cout << "进程：" << getpid() << endl;

    struct sigaction act, oact;
    act.sa_handler = sigcd;

    sigaction(2, &act, &oact);

    while (1) {;}

    return 0;
}