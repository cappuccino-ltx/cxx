
#include <iostream>
#include <unistd.h>

int main() {
    alarm(1);
    int count = 1;
    while (count++){
        std::cout << count << std::endl;
    }
    return 0;
}