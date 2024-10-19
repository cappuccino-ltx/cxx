
#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void servise(int serviseSock, std::string userip, int16_t userport) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        size_t s = read(serviseSock,buffer,sizeof(buffer));    

        if (s > 0) {
            buffer[s] = '\0';
            std::cout << userip.c_str() << ":" << userport << "#" << buffer << std::endl;
        }
        else if (s == 0) {
            //表示对方关闭了连接
            std::cerr << userip << ":" << userport << " shutdowm,me too!" << std::endl;
            break;
        }
        else {
            std::cerr << "read socket error," << errno << strerror(errno) << std::endl;
            break;
        }

        write(serviseSock, buffer, strlen(buffer));
    }
}


class tcpServer{
public:
    tcpServer(int16_t port, std::string ip = "")
    :_ip(ip),
    _port(port),
    _listenSock(-1)
    {}
    
    ~tcpServer(){
        if (_listenSock > 0) {
            close(_listenSock);
        }
    }

    void initServer(){
        //backlog不能太大也不能太小
        static int gbacklog = 20;
        //申请描述符
        _listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listenSock < 0) {
            std::cerr << "注册socket失败" << std::endl;
            exit(2);
        }
        //绑定端口号和IP地址
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str());
        local.sin_port = htons(_port);

        if (bind(_listenSock, (struct sockaddr*)&local, sizeof(local)) < 0) {
            std::cerr << "绑定ip和端口号失败" << std::endl;
            exit(3);
        }

        //设置监听状态
        if (listen(_listenSock, gbacklog) < 0) {
            std::cerr << "设置监听失败" << std::endl;
            exit(4);
        }
    }

    void start() {

        //将子进程的信号改为忽略
        signal(SIGCHLD, SIG_IGN);
        
        while (1) {

            struct sockaddr_in user;
            socklen_t len = sizeof(user);
            int serviseSock = accept(_listenSock, (struct sockaddr*)&user, &len);
            std::string userip = inet_ntoa(user.sin_addr);
            int16_t userport = ntohs(user.sin_port);
            
            //servise(serviseSock,userip,userport);
            int pid = fork();
            if (pid == 0) {
                close(_listenSock);
                servise(serviseSock,userip,userport);
                close(serviseSock);
                exit(0);
            }
            close(serviseSock);
        }
    }

private:
    std::string _ip;
    int16_t _port;
    int _listenSock;
};