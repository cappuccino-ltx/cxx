



#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class udpserver{
public:
    udpserver(int16_t port)
        :port_(port)
    {
        initserver();
        start();
    }
    ~udpserver() {}

private:
    void initserver() {
        //创建udp套接字
        socket_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_ < 0) {
            std::cout << "创建套接字失败" << std::endl;
            exit(1);
        }
        //绑定ip地址和端口，
        struct sockaddr_in addr;
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = inet_addr("0.0.0.0");
        
        if(bind(socket_,(const struct sockaddr*)&addr,sizeof(addr))) {
            std::cout << "端口绑定失败" << std::endl;
            exit(2);
        }

        std::cout << "服务器启动成功 sock："  << socket_ << std::endl;
    }

    void start() {
        char buff[4096] = { 0 };
        for (;;){
            struct sockaddr_in addr;
            socklen_t len;
            int n = recvfrom(socket_, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&addr, &len);
            if (n <= 0) {
                continue;
            }

            std::cout << "来自 " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << "收到消息：" << buff << std::endl;
            buff[n] = 0;
            std::string s = std::string("server echo# ") + buff;
            sendto(socket_, s.c_str(), s.size(), 0, (const struct sockaddr*)&addr, len);
        }
    }

private:
    int16_t port_;
    int socket_;
};