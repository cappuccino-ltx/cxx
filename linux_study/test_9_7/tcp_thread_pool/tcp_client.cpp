
#include <iostream>
#include <string>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* args[]) {
    if (argc != 3) {
        std::cout << "输入错误！" << std::endl;
        exit(1);
    }
    std::string serverip = args[1];
    int16_t serverport = atoi(args[2]);

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        std::cerr << "获取描述符失败" << std::endl;
    }

    //获取链接
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverip.c_str());
    server.sin_port = htons(serverport);

    if (connect(serverSock, (struct sockaddr*)&server, sizeof(server)) < 0){
        std::cerr << "链接错误！" << std::endl;
        exit(3);
    }
    std::string sendbuffer;
    char recvbuffer[1024];
    while (1) {
        sendbuffer.clear();
        memset(recvbuffer,0,sizeof(recvbuffer));
        std::cout << "请输入#";
        std::getline(std::cin, sendbuffer);
        send(serverSock,sendbuffer.c_str(),sendbuffer.size(),0);
        ssize_t recv_size = recv(serverSock,recvbuffer,sizeof(recvbuffer), 0);

        if (recv_size > 0) {
            recvbuffer[recv_size] = 0;
            std::cout << "server 回显#" << recvbuffer << std::endl;
        }
        else if (recv_size == 0) break;
        else break;
    }
    close(serverSock);
    return 0;
}