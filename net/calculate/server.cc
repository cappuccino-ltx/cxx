
#include "calculate.hpp"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2){
        std::cout << "usage " << argv[0] << " + port" << std::endl;
        exit(1);
    }

    uint16_t port = std::stoi(argv[1]);

    ns_calculate::Calculation ser(port);
    ser.listen();

    return 0;
}