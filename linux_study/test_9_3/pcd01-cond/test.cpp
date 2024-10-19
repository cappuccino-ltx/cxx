
#include "mypool.hpp"

int main() {
    //std::cout << "hello" << std::endl;
    Pool<int(*)(int,int),int> p(1);
    p.strat();

    return 0;
}