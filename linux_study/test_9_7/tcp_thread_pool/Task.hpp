#pragma once
#include <iostream>
#include <functional>
#include <string>

using server = std::function<void(int,std::string &,int16_t)>;

class Task{

public:
    Task(int sock, std::string ip, int16_t port, server ser)
    :_sock(sock),
    _ip(ip),
    _port(port),
    _ser(ser)
    {}

    void operator()(){
        _ser(_sock,_ip,_port);
    }

private:
    int _sock;
    std::string _ip; 
    int16_t _port;
    server _ser;
};