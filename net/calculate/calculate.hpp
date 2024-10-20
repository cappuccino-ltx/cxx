#pragma once

#include "tcpserver.hpp"

#include <iostream>
#include <string>
#include <vector>


namespace ns_calculate{
    
    using tcpser = ns_tcp::TcpServer;
    using ptcpser = std::unique_ptr<tcpser>;

    class Calculate{

    public:
        
        Calculate(uint16_t port)
            :tcp_(new tcpser(port))
        {
            tcp_->Start([](const std::string& mass){
                //读取上来的完整报文，
                
                //1,解析报文，

                //2,进行计算

                //3,进行反序列化

                return mass;
            });
        }
        ~Calculate(){}
    
    private:
        ptcpser tcp_;
    };

}