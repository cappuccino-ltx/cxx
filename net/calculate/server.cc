
#include "tcpserver.hpp"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2){
        std::cout << "usage " << argv[0] << " + port" << std::endl;
        exit(1);
    }

    uint16_t port = std::stoi(argv[1]);

    ns_tcp::TcpServer ser(port);

    return 0;
}