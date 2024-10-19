
#include "tcp_server.hpp"
#include <memory>


int main(int argc, char* args[]) {
    std::string ip;
    int16_t port;
    if (argc == 2) {
        ip = "";
        port = atoi(args[1]);
    }
    else if (argc == 3) {
        ip = args[1];
        port = atoi(args[2]);
    }
    else {
        std::cerr << "输入错误！" << std::endl;
        exit(1);
    }

    std::unique_ptr<tcpServer> server(new tcpServer(port,ip));
    server->initServer();
    server->start();

    return 0;
}