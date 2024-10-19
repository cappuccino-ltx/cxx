#pragma once

#include <iostream>
#include <functional>
#include "sock.hpp"
#include "epoll.hpp"

class epoll_server{

private:
    static const int max_events = 64;
    using function = std::function<void(std::string)>;
private:
    int _listenSock;
    int _epfd;
    struct epoll_event *_revents;
    int _maxevents;
    function _responsefun;
public:
    epoll_server(function fun):_responsefun(fun), _maxevents(max_events) {
        //申请相应的空间
        _revents = new struct epoll_event[_maxevents];
        //创建套接字
        _listenSock = sock::Socket();
        //绑定端口
        sock::Bind(_listenSock, "0.0.0.0", 8080);
        //设置监听
        sock::Listen(_listenSock);
        //创建epoll模型
        _epfd = epoll::create();
        //将——listenSock添加到epoll模型中
        if (!epoll::ctrl(_epfd, EPOLL_CTL_ADD, _listenSock, EPOLLIN)) exit(6);
        logmessage(DEBUG, "add listenSock to epoll success...");
        logmessage(DEBUG, "listenSock: %d,_epfd:%d", _listenSock, _epfd);

    }
    ~epoll_server() {
        if (_listenSock > 0) close(_listenSock);
        if (_epfd > 0) close(_epfd);
        if (_revents) delete []_revents;
    }

    void start() {
        while (true) {
            loopOnce(-1);
        }
    }
private:
    void loopOnce(int timeout) {
        
        int n = epoll::wait(_epfd, _revents, _maxevents, timeout);
        if (n == 0) {
            logmessage(DEBUG, "timeout..."); 
        }
        else if (n < 0) {
            logmessage(WARNING, "epoll wait error...:%s", strerror(errno));
        }
        else {
            //等待成功
            handlerEvent(n);
        }
    }
    void handlerEvent(int size) {
        for (int i = 0; i < size; ++i) {
            int sock = _revents[i].data.fd;
            uint32_t revents = _revents[i].events;
            if (revents & EPOLLIN) {
                //读事件就绪
                if (sock == _listenSock) Acceptor();
                else Recver(sock);
            }
        }
    }
    void Acceptor() {
        std::string userip; 
        uint16_t userport = 0;
        int serviseSock = sock::Accept(_listenSock,userip, userport);
        if (serviseSock < 0) {
            logmessage(WARNING, "get a new link fatal...");
        }
        else {
            logmessage(DEBUG, "get a new link success...");
            epoll::ctrl(_epfd, EPOLL_CTL_ADD, serviseSock, EPOLLIN);
        }
    }
    void Recver(int sock) {
        std::string inbuffer;
        if(!sock::Recv(sock, inbuffer)) {
            //读取失败
            epoll::ctrl(_epfd, EPOLL_CTL_DEL, sock, 0);
            logmessage(WARNING, "client[%d]:recver error", sock);
            close(sock);
        }
        else {
            //读取成功
            _responsefun(inbuffer);
        }

    }
};