

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main() {
    size_t fd = open("test01.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    dup2(fd, 1);
    cout << "hello ltx!" << endl;
    close(fd);
    return 0;
}