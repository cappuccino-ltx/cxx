
#include "mypool.hpp"

int main() {
    Pool<int(*)(int,int),int> p(5);
    p.strat();
    return 0;
}