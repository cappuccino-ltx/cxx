#pragma once 


#include <iostream>
#include <string>
#include <memory>

#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


namespace ns_sock{

    class vir_sock{
    public:
        virtual void                        Socket() = 0;
        virtual void                        ReUse() = 0;
        virtual void                        Bind(uint16_t)= 0;
        virtual void                        Listen(int logback_  = 50)= 0;
        virtual std::shared_ptr<vir_sock>   Accept()= 0;
        virtual bool                        Read(std::string*)= 0;
        virtual bool                        Write(const std::string&)= 0;
        virtual void                        Close()= 0;

        virtual void                        Connect(std::string,uint16_t)= 0;

    public:
        int             sock_ = -1;
        std::string     ip_ = "0.0.0.0";
        uint16_t        port_ = 0;
    };

    

    class Sock : public vir_sock{

    public:
        Sock(){}
        ~Sock(){
            if (sock_ >= 0) {
                Close();
            }
        }
        virtual void Socket() override{
            //int BLOCK = block ? SOCK_NONBLOCK : 0;
            sock_ = socket(AF_INET, SOCK_STREAM , 0);
            if (sock_ < 0) {
                std::cerr << "failed to conncet !!!" << std::endl;
                exit(-1);
            }
        }
        virtual void ReUse(){
            int opt = 1;
            if(setsockopt(sock_,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt)) < 0) {
                std::cerr << "feiled to reuse !!!" << std::endl;
            }
        }
        virtual void Bind(uint16_t port) override{
            struct sockaddr_in local;
            bzero(&local,sizeof(local));
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = INADDR_ANY;
            // inet_pton(AF_INET,"0.0.0.0",&(local.sin_addr));
            local.sin_port = htons(port);

            if (bind(sock_,(struct sockaddr*)&local,sizeof(local))) {
                std::cerr << "failed to binding !!!" << std::endl;
                exit(-1);
            }
        }
        virtual void Listen(int logback_  = 50)override {
            if(listen(sock_,logback_) < 0) {
                std::cerr << "failed to listening" << std::endl;
                exit(-1);
            }
        }
        virtual std::shared_ptr<vir_sock> Accept() {
            std::shared_ptr<vir_sock> ret(new Sock);
            struct sockaddr_in client;
            bzero(&client,sizeof(client));
            socklen_t len = 0;
            ret->sock_ = accept(sock_,(struct sockaddr*)&client, &len);
            char buff[16] = "";
            inet_ntop(AF_INET,&(client.sin_addr),buff,sizeof(buff));
            ret->ip_ = buff;
            ret->port_ = ntohs(client.sin_port);
            return ret;
        }
        virtual bool Read(std::string* mas) override{
            char buff[4096] = "";
            int n = read(sock_,buff,sizeof(buff));
            if (n > 0) {
                buff[n] = 0;
                *mas = buff;
                return true;
            }
            else if (n == 0) {
                std::cerr << "failed to read" << std::endl;
            }
            else{
                std::cerr << "failed to read" << std::endl;
            }
            return false;
        }
        virtual bool Write(const std::string& mas)override {
            int n = write(sock_,mas.c_str(),mas.size());
            if (n < 0) {
                std::cerr << "failed to write !!!" << std::endl;
                return false;
            }
            return true;
        }

        virtual void Close()override{
            close(sock_);
            sock_ = -1;
        }

        virtual void Connect(std::string ip,uint16_t port)override{
            struct sockaddr_in target;
            bzero(&target,sizeof(target));
            target.sin_family = AF_INET;
            inet_pton(AF_INET,ip.c_str(),&(target.sin_addr));
            target.sin_port = htons(port);

            if (connect(sock_,(struct sockaddr*)&target,sizeof(target)) != 0) {
                std::cerr << "failed to conncet !!!" << std::endl;
                exit(-1);
            }
        }
    };




    using psock =  std::shared_ptr<vir_sock>;                   //sock 智能指针类型
}