

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

using namespace std;

int main() {
    int fd = open("myfifo", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    char buff[1024];
    while (true) {
        memset(buff, '0', sizeof(buff));
        ssize_t n = read(fd, buff, sizeof(buff) - 1);
        if (n > 0){
            buff[n] = 0;
            cout << "接收到的数据：" << buff;
        }
        else if (n == 0) {
            cout << "接收完毕" << endl;
            break;
        }else {
            perror("read");
            break;
        }
    }
    close(fd);
    return 0;
}