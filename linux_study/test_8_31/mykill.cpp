
#include <iostream>
#include <string>
#include <sys/types.h>
#include <signal.h>
using namespace std;

int main(int argc,char* args[]) {
    if (argc != 3) {
        cout << "./mykill signo pid" << endl;
        exit(1);
    }
    kill(stoi(args[2]), stoi(args[1]));
    return 0;
}