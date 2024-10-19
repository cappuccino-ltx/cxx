

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct sendData{
    int _sock;
    struct sockaddr_in *server;
};


void* sending(void *arg) {
    struct sendData* data = (struct sendData*)arg;
    int sock = data->_sock;
    struct sockaddr_in server = *(data->server);

    while (1) {
        std::string massage;
        std::cerr << "请输入内容：" ;
        std::getline(std::cin, massage);
        //发送数据
        sendto(sock, massage.c_str(), massage.size(), 0, (struct sockaddr*)&server, sizeof(server));
    }
}

void* receive(void *arg) {
    struct sendData* data = (struct sendData*)arg;
    int sock = data->_sock;
    char buffer[1024];
    while (1) {
        memset(buffer, '\0', sizeof(buffer));
        struct sockaddr_in from;
        socklen_t len = sizeof(from);
        ssize_t recv_size = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &len);
        if(recv_size < 1) {
            continue;
        }
        buffer[recv_size] = '\0';
        //printf("[%s:%u]#%s\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port),buffer);
        std::cout << buffer << std::endl;
    }
}

//客户端，负责给服务端发送消息
int main(int argc, char* args[]) {

    if (argc != 3) {
        std::cerr << "请正确输入参数！" << std::endl;
        exit(1);
    }
    std::string ip = args[1];
    int16_t port = atoi(args[2]);

    //创建套接字
    int _sock = socket(AF_INET, SOCK_DGRAM, 0);
    //这里依然会绑定，但是不需要手动绑定，回自动绑定，在第一次send的时候自动绑定，  
    if (_sock < 0) {
        std::cerr << "创建套接字失败!" << std::endl;
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    socklen_t len = sizeof(server);

    sendData data;
    data._sock = _sock;
    data.server = &server;

    //创建线程，让线程1负责发送，线程2负责接收


    pthread_t send,recv;
    pthread_create(&send,nullptr,sending,(void*)&data);
    pthread_create(&send,nullptr,receive,(void*)&data);

    pthread_join(send,nullptr);
    pthread_join(recv,nullptr);
    
    // std::string massage;
    // char buffer[1024];
    // while (1) {
    //     std::cout << "请输入你的信息：";
    //     std::getline(std::cin, massage);
    //     sendto(_sock, massage.c_str(), massage.size(), 0, (struct sockaddr*)&server, len);
    //     //std::cout << "发送成功！" << std::endl;
    //     //读取服务器返回的消息
    //     memset(buffer, '\0', sizeof(buffer));
    //     struct sockaddr_in from;
    //     socklen_t fromLen = sizeof(from);
    //     ssize_t recv_size = recvfrom(_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromLen);
        
    //     //打印数据
    //     if (recv_size < 1) {
    //         std::cout << "接收失败！" << std::endl;
    //         exit(3);
    //     }
    //     buffer[recv_size] = '\0';
    //     std::string fromIp = inet_ntoa(from.sin_addr);
    //     int16_t fromPort = ntohs(from.sin_port);
    //     std::cout << "server echo#[" << fromIp << ":" << fromPort << "]" << buffer << std::endl;
    // }

    close(_sock);
    return 0;
}