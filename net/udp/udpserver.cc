


#include "udp.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {

        std::cout << "请输入端口号" << std::endl;
        exit(1);
    }

    udpserver ser(std::stoi(argv[1]));

    return 0;
}

