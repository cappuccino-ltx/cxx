

#include <iostream>
#include <functional>
#include <memory>
#include "sock.hpp"

using function = std::function<void(int)>;
class HttpServer;
class serverData{
public:
    serverData(int serviseSock,HttpServer *http):_serviseSock(serviseSock),_http(http) {}
    int _serviseSock;
    HttpServer *_http;
};
class HttpServer{
public:
    int _listenSock;
    function _fun;
    sock _sock;
public:
    HttpServer(int16_t port,function fun, std::string ip = "0.0.0.0"):_fun(fun){
        _listenSock = _sock.Socket();
        _sock.Bind(_listenSock, ip, port);
        _sock.Listen(_listenSock);
    }
    ~HttpServer(){
        if (_listenSock > 0) {
            close(_listenSock);
        }
    }

    static void* execute(void *arg) {
        pthread_detach(pthread_self());
        std::unique_ptr<serverData> data((serverData*)arg);
        data->_http->_fun(data->_serviseSock);
        close(data->_serviseSock);
    }

    void strat() {
        while (1) {
            std::string userip;
            int16_t userport;
            int serviseSock = _sock.Accept(_listenSock, userip, userport);
            pthread_t thr;
            serverData *data = new serverData(serviseSock, this);
            pthread_create(&thr, nullptr, execute, data);
        }
    }
    static bool Recv(int socket, std::string &str) {
        sock s;
        return s.Recv(socket, str);
    }
    static void Send(int socket, std::string &str){
        sock s;
        s.Send(socket,str);
    }
};
