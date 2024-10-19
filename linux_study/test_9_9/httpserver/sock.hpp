
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

#include "Log.hpp"

class sock
{
private:
    // backlog不能太大也不能太小
    static const int gbacklog = 20;

public:
    sock()
    {
    }

    ~sock() {}

    int Socket()
    {
        int listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSock < 0)
        {
            logmassage(FATAL, "创建socket失败");
            exit(2);
        }
        int opt = 1;
        setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        return listenSock;
    }

    void Bind(int listenSock, std::string ip, int16_t port)
    {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
        local.sin_port = htons(port);

        if (bind(listenSock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logmassage(FATAL, "绑定ip和端口号失败");
            exit(3);
        }
    }

    void Listen(int listenSock)
    {
        if (listen(listenSock, gbacklog) < 0)
        {
            logmassage(FATAL, "设置监听失败");
            exit(4);
        }
    }

    int Accept(int listenSock, std::string &ip, int16_t &port)
    {
        struct sockaddr_in user;
        socklen_t len = sizeof(user);
        int serviseSock = accept(listenSock, (struct sockaddr *)&user, &len);
        ip = inet_ntoa(user.sin_addr);
        port = ntohs(user.sin_port);
        logmassage(NORMAL, "%s:%d 已成功连接访问",ip.c_str(),port);
        return serviseSock;
    }

    void Connect(int serverSock,std::string serverip, int16_t serverport) {
        //获取链接
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(serverip.c_str());
        server.sin_port = htons(serverport);

        if (connect(serverSock, (struct sockaddr*)&server, sizeof(server)) < 0){
            logmassage(FATAL,"链接错误！");
            exit(3);
        }
    }

    void Send(int sock, const std::string str)
    {
        if (send(sock, str.c_str(), str.size(), 0) < 0)
        {
            logmassage(ERROR, "向socket：%d发送失败，发送内容：%s", sock, str.c_str());
        }
    }
    bool Recv(int sock, std::string &out)
    {
        char buffer[1024];
        int s = recv(sock, buffer, sizeof(buffer), 0);
        if (s > 0)
        {
            buffer[s] = '\0';
            out += buffer;
            return true;
        }
        else if (s == 0)
        {
            logmassage(NORMAL, "连接%d已断开连接！", sock);
            close(sock);
            return false;
        }
        else
        {
            close(sock);
            logmassage(ERROR, "recv 错误");
            return false;
        }
    }
};
