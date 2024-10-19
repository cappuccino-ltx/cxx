

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

using namespace std;


int main() {
    if (mkfifo("myfifo", 0666) < 0) {
        perror("mkfifo");
        exit(1);
    }
    int fd = open("myfifo", O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    char buff[1024];
    while (1) {
        memset(buff, '\0', sizeof(buff));
        fflush(stdout);
        ssize_t n = read(0, buff, sizeof(buff) - 1);
        if (n > 0){
            buff[n] = 0;
            write(fd, buff, strlen(buff));
        }
        else  {
            perror("read");
            break;
        }
    }
    close(fd);
    return 0;
}