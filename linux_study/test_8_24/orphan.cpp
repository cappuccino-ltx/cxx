
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {

    size_t id = fork();
    int tag = 5;
    if (id) {
        //父进程
        while (tag) {
            cout << "I am father!  " << tag-- << endl;
            sleep(1);
        }
    }
    else {
        //子进程
        while (1) {
            cout << "I am child !" << endl;
            sleep(1);
        }
    }


    return 0;

}
