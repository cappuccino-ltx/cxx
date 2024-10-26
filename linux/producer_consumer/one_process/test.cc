
#include <signal.h>

#include "../comm/thread_pool.hpp"
#include "../comm/task.hpp"

//多线程
using namespace taskspace;

void sighandler(int signum) {
    std::cout << "收到了信号：" << signum << std::endl;
    exit(signum);
}

int main() {
    struct sigaction act;
    act.sa_handler = sighandler;
    for (int i = 1; i < 32; i++) {
        sigaction(i, &act, nullptr);
    }
    thread::ThreadPool<Task>* pool = thread::ThreadPool<Task>::get(5,3);
    pool->start(thread::producer<Task>,nullptr, thread::consumer<Task>,nullptr);
    while(1);
    return 0;
}
