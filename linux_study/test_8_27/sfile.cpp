
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    //int fd = open("log2.txt", O_WRONLY | O_CREAT, 0666);
    int fd = open("log2.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    //const char* msg = "hello linuxIO!\n";
    //write(fd, msg, strlen(msg));

    char str[128] = {0};
    read(fd, str, sizeof(str));
    printf("%s\n", str);
    close(fd);
    return 0; 
}
