#pragma once

#include "thread_pool.hpp"

#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <signal.h>

const int defalut_sock = -1;
const uint16_t defalut_port = 8080;
const int backlog = 20;

namespace ns_tcp
{

    class ClientConn
    {
    public:
        ClientConn(int fd, std::string ip, uint16_t port)
            : fd_(fd), ip_(ip), port_(port)
        {
        }
        ~ClientConn()
        {
            if (fd_ > 0)
                close(fd_);
        }

        int fd_;
        std::string ip_;
        uint16_t port_;
    };

    class TcpServer
    {

    public:
        TcpServer(std::function<std::string(std::string)> fun) : sock_(defalut_sock), collback_(fun)
        {
            signal(SIGPIPE, SIG_IGN);
        }
        ~TcpServer()
        {
            if (sock_ > 0)
                close(sock_);
        }
        void InitServer(uint16_t port = defalut_port)
        {

            // start a thread pool
            // pool_ = ns_thread::ThreadPool<std::shared_ptr<ClientConn>>::GetInstance(std::bind(&TcpServer::service,this,std::placeholders::_1));
            pool_ = ns_thread::ThreadPool<std::shared_ptr<ClientConn>>::GetInstance([this](const std::shared_ptr<ClientConn> conn) -> void{
                char buffer[4096] = { 0 };
                int n = read(conn->fd_,buffer,sizeof(buffer));
                if (n > 0) {
                    buffer[n] = 0;
                    std::string ret = collback_(buffer);
                    n = write(conn->fd_,ret.c_str(),ret.size());
                    if(n < 0) {
                        std::cout << "failed to write" << std::endl;
                    }
                }
                else if (n == 0) {
                    //user disconnects link
                    std::cout << "user disconnects link from " << conn->ip_ << ":" << conn->port_ << " ." << std::endl;
                }
                else {
                    //connection has an exception
                    std::cout << "the connection is abnormal from " << conn->ip_ << ":" << conn->port_ << " ."  << std::endl;
                } });

            sock_ = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_ < 0)
            {
                std::cout << "failed to create socket!!!" << std::endl;
                exit(2);
            }
            // set up address reuse
            int opt = 1;
            setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = INADDR_ANY;
            local.sin_port = htons(port);

            if (bind(sock_, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                std::cout << "binding socket failed!!!" << std::endl;
                exit(3);
            }

            if (listen(sock_, backlog) < 0)
            {
                std::cout << "listening socket failed!!!" << std::endl;
                exit(4);
            }
            std::cout << "server started successfully!" << std::endl;
        }
        void Start()
        {

            struct sockaddr_in client;
            bzero(&client, sizeof(client));
            socklen_t len = sizeof(client);
            for (;;)
            {
                // 获取新链接
                int client_fd = accept(sock_, (struct sockaddr *)&client, &len);
                if (client_fd < 0)
                {
                    std::cout << "failed to obtain link!!!";
                    exit(5);
                }
                // 解析链接的ip地址和端口号，
                char ip[32] = {0};
                inet_ntop(AF_INET, &(client.sin_addr), ip, sizeof(ip));
                std::string client_ip = ip;
                uint16_t client_port = ntohs(client.sin_port);
                std::cout << "obtain a link from " << client_ip << ":" << client_port << std::endl;

                // version 2
                std::shared_ptr<ClientConn> conn(new ClientConn(client_fd, client_ip, client_port));
                pool_->Push(conn);

                //  version 1
                // service(client_fd,client_ip,client_port);
                // close(client_fd);
            }
        }

        void service(const std::shared_ptr<ClientConn> &conn)
        {
            char buffer[4096] = {0};
            int n = read(conn->fd_, buffer, sizeof(buffer));
            if (n > 0)
            {
                buffer[n] = 0;
                // 接收到用户的信息，进行业务处理
                // 将接受到的内容传递给上层，进行应用层的协议拆分和内容获取
                std::string ret = collback_(buffer);
                n = write(conn->fd_, ret.c_str(), ret.size());
                if (n < 0)
                {
                    std::cout << "failed to write" << std::endl;
                }

                // 简单的进行回显
                // std::cout << conn->ip_ << ":" << conn->port_ << ":>" << buffer << std::endl;
                // write(conn->fd_,buffer,n);
            }
            else if (n == 0)
            {
                // user disconnects link
                std::cout << "user disconnects link from " << conn->ip_ << ":" << conn->port_ << " ." << std::endl;
            }
            else
            {
                // connection has an exception
                std::cout << "the connection is abnormal from " << conn->ip_ << ":" << conn->port_ << " ." << std::endl;
            }
        }

    private:
        int sock_ = -1;
        ns_thread::ThreadPool<std::shared_ptr<ClientConn>> *pool_ = nullptr;

        std::function<std::string(std::string)> collback_;
    };
}
