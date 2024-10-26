#pragma once

#include "sock.hpp"
#include "threadpool.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>


namespace ns_tcp{

    using namespace ns_sock;

    
    using tpool = ns_thread::ThreadPool<psock>;           //线程池类型
    using fun_back = std::function<void (const std::string&,std::string*)>;   //上层的回调函数


    class TcpServer{

    public:
        //const size_t buff_size = 4096;                          //缓冲区大小

        TcpServer(uint16_t port) 
            :listensock_(new Sock())
        {
            listensock_->Socket();
            listensock_->ReUse();
            listensock_->Bind(port);
            listensock_->Listen();
        }
        void Start(fun_back collback) {
            std::unique_ptr<tpool> pool(tpool::GetInstance([collback,this](psock conn){
                //基于短链接的服务处理
                std::string buff;
                std::string send;
                conn->Read(&buff);
                //可以在这里插入协议解析，然后返回上层的就是request了，但是这里暂时不做处理
                //?

                collback(buff,&send);
                conn->Write(send);
            }));
            for ( ;; ) {
                psock client = listensock_->Accept();
                //将链接放入线程池中，进行业务处理
                pool->push(client);
            }
        }
        ~TcpServer() {}
    private:
        psock listensock_;
    };
}