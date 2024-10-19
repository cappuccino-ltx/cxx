

#include <iostream>
#include <unistd.h>

int main() {
    while (1) {
        std::cout << "此进程死循环中 ，pid：" << getpid() << std::endl;
        sleep(1);
    }
    return 0;
}