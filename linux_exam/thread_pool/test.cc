

#include "thread_pool.hpp"

int main() {
    Pool::ThreadPool<int>* pool = Pool::ThreadPool<int>::get(5,5);
    pool->start();

    while(1);
    return 0;
}
