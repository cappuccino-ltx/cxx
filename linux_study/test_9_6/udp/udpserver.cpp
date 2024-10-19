

#include "udpserver.hpp"
#include <memory>


int main(int argc, char* args[]) {

    std::string ip;
    int16_t port = 0;
    if (argc == 3) {
        ip = args[1];
        port = atoi(args[2]);
    }
    else if (argc == 2) {
        ip = "0.0.0.0";
        port = atoi(args[1]);
    }
    else{
        perror("输入错误！");
        return 1;
    }

    std::unique_ptr<udpserver> server(new udpserver(ip,port));
    server->initServer();
    server->startServer();
    return 0;
}