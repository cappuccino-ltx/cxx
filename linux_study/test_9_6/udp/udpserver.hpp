

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



class udpserver{

public:
    udpserver(std::string ip, int16_t port):_fd(-1), _ip(ip), _port(port),_users(0)
    {}

    ~udpserver(){
        if (_fd > 0) {
            close(_fd);
        }
    }

    void initServer() {
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_fd < 0) {
            perror("注册socket失败");
            exit(2);
        }
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = inet_addr(_ip.c_str());

        if (bind(_fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            perror("绑定失败！");
            exit(3);
        }
        //std::cout << "绑定成功！"<< std::endl;
    }

    void startServer(){

        //准备用来接收客户端发送的消息的缓冲区
        char buffer[1024];
        while (1) {
            
            //准备用来接收发送消息的客户端信息
            memset(buffer, '\0', sizeof(buffer));
            struct sockaddr_in peer;
            memset(&peer, 0, sizeof(peer));
            socklen_t len = sizeof(peer);
            //接收数据，以及接收发送数据的客户端信息
            //std::cout << "正在接收！" << std::endl;
            ssize_t recv_size = recvfrom(_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &len);
            
            //打印客户端发送来的数据
            //std::cout << "接收成功！正在打印：" << std::endl;
            // if (recv_size > 0) {
            //     buffer[recv_size] = 0;
            //     std::string ip = inet_ntoa(peer.sin_addr);
            //     int16_t port = ntohs(peer.sin_port);
            //     std::cout << "[" << ip << ":" << port << "]:";
            //     std::cout << buffer << std::endl;
            // }
            //处理数据
            buffer[recv_size] = 0;
            std::string massage;
            massage += inet_ntoa(peer.sin_addr);
            massage += ":";
            massage += ntohs(peer.sin_port);

            //_users.insert(make_pair<std::string,struct sockaddr_in>(massage, peer);
            _users.insert({massage, peer});

            massage += "#";
            massage += buffer;
            //_users.insert(makepair(, peer);

            //回写数据
            //sendto(_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&peer, len);
            for (auto &s : _users) {
                sendto(_fd, massage.c_str(), massage.size(),0 ,(struct sockaddr*)&(s.second), sizeof(s.second));
            }
        }
    }

private:
    int _fd;
    std::string _ip;
    int16_t _port;
    std::unordered_map<std::string,struct sockaddr_in> _users;
};