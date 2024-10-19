#pragma once

#include "sock.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>


namespace ns_tcp{

    using namespace ns_sock;

    class TcpServer{

    public:
        typedef std::shared_ptr<vir_sock> psock;

        TcpServer(uint16_t port) 
            :listensock_(new Sock())
        {
            listensock_->Socket();
            listensock_->Bind(port);
            listensock_->Listen();
        }
        void Start() {
            for ( ;; ) {
                psock client = listensock_->Accept();
                //将链接放入线程池中，进行业务处理
            }
        }
        ~TcpServer() {}
    private:
        psock listensock_;
    };
}