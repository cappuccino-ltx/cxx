#pragma once

#include "tcpserver.hpp"
#include "protocol.hpp"

#include <iostream>
#include <string>
#include <vector>


namespace ns_calculate{

    using namespace ns_protocol;

    using tcpser = ns_tcp::TcpServer;
    using ptcpser = std::unique_ptr<tcpser>;
    //using handler = std::function<void(Request&, Response&)>;
    

    class vir_calculator{
    public:

        virtual void handler(Request&, Response&) = 0;
    };

    class Calculator : public vir_calculator{
    public:
 
        virtual void handler(Request& request, Response& response) {
            switch (request.opt_) {
                case '+': 
                    response.result_ = request.x_ + request.y_;
                    response.code_ = 0;
                    break;
                case '-': 
                    response.result_ = request.x_ - request.y_;
                    response.code_ = 0;
                    break;
                case '*': 
                    response.result_ = request.x_ * request.y_;
                    response.code_ = 0;
                    break;
                case '/': 
                    if(request.y_ == 0) {
                        response.code_ = 1;
                        break;
                    }
                    response.result_ = request.x_ / request.y_;
                    response.code_ = 0;
                    break;
            }
        }
    };

    using calcu = std::unique_ptr<vir_calculator>;

    class Calculation{

    public:
        
        Calculation(uint16_t port)
            :tcp_(new tcpser(port))
            ,cal_(new Calculator())
        {}
        ~Calculation(){}

        void listen() {
            tcp_->Start([this](const std::string& mass,std::string* send){
                //读取上来的完整报文，
                
                //1,解析报文，
                Request req(mass);
                //std::cout << req.x_ << " " << req.opt_ << " " << req.y_ << std::endl;
                Response res;
                //2,进行计算
                cal_->handler(req,res);
                //3,进行反序列化
                *send = res.Encode();
                std::cout << req.Encode() << std::endl;
                std::cout << res.Encode() << std::endl;
            });
        }
    
    private:
        ptcpser tcp_;
        calcu cal_;
    };

}