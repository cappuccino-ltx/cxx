
#include <iostream>

#include <poll.h>

#include "sock.hpp"

#define FD_NONE -1

class poll_server
{
private:
    int _listenSock;
    struct pollfd *_fds;
    nfds_t _nfds;
    int _timeout;
public:
    poll_server():_nfds(100)
    {
        _listenSock = sock::Socket();
        sock::Bind(_listenSock, "0.0.0.0", 8080);
        sock::Listen(_listenSock);
        _fds = new struct pollfd[_nfds];
        for (int i = 0; i < _nfds; ++i) {
            _fds[i].fd = FD_NONE;
            _fds[i].events = _fds[i].revents = 0;
        }
        _fds[0].fd = _listenSock;
        _fds[0].events = POLLIN;
        _timeout = 1000;
    }
    ~poll_server() {
        if (_listenSock > 0) {
            close(_listenSock);
        }
        if (_fds) delete [] _fds;
    }

    void start() {
        while (true) {
            DebugPrint();
            //调用poll函数
            //int poll(struct pollfd *fds, nfds_t nfds, int timeout);
            //第一个参数，可以传入一个struct pollfd的一个数组，
            //第二个参数nfds代表数组的最大长度，
            //第三个参数代表每过多长时间轮询检查一次，单位是毫秒，1000代表每一秒检测一次
            int n = poll(_fds, _nfds, _timeout);
            if (n == 0) {
                //等待中，
                //logmessage(DEBUG, "time out ...");
            }
            else if (n < 0) {
                //poll error
                logmessage(ERROR, "poll error ...");
            }
            else {
                //成功检测到事件
                handler_event();
            }
        }
    }
private:
    void handler_event() {
        for (int i = 0; i < _nfds; ++i) {
            if (_fds[i].fd == FD_NONE) continue;
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _listenSock) Accept();
                else Recver(i);
            }
            
        }
    }
    void Accept() {
        std::string userip;
        uint16_t userport = 0;
        int serviseSock = sock::Accept(_listenSock, userip, userport);
        if (serviseSock < 0) {
            logmessage(WARNING, "accept error");
            return;
        }

        logmessage(NORMAL, "get a new link success # [%s:%d]: %d", userip.c_str(), userport, serviseSock);

        int pos  = 0;
        for ( ; pos < _nfds; ++pos) {
            if (_fds[pos].fd == FD_NONE) break;
        }
        if (pos == _nfds) {
            //对struct pollfd进行扩容，
            logmessage(WARNING, "poll server already full, close : %d", serviseSock);
            close(serviseSock);
        }
        else{
            _fds[pos].fd = serviseSock;
            _fds[pos].events = POLLIN;
        }
    }

    void Recver(int pos) {
        logmessage(DEBUG, "message in get IO event : %d", _fds[pos].fd);
        std::string inbuffer;
        if (!sock::Recv(_fds[pos].fd, inbuffer)) {
            _fds[pos].fd = FD_NONE;
            _fds[pos].events = 0;
            return ;
        }
        logmessage(DEBUG, "client[%d]#%s", _fds[pos].fd, inbuffer.c_str());
    }

    void DebugPrint() {
        std::cout << "_fds[]:";
        for (int i = 0; i < _nfds; ++i){
            if (_fds[i].fd == FD_NONE) continue;
            std::cout << _fds[i].fd << " ";
        }
        std::cout << std::endl;
    }
};