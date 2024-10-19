
#include <iostream>
#include <map>

#include <cerrno>

#include "sock.hpp"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


#define NUM (sizeof(fd_set)*8)
#define FD_NONE -1

class select_server
{

private:
    uint16_t _port;
    int _listenSock;
    int _arr_fd[NUM];
    std::map<std::string, std::string> _dict;
    std::map<int, std::string> _response;
public:
    select_server(uint16_t port = 8080):_port(port) 
    {
        _listenSock = sock::Socket();
        sock::Bind(_listenSock, "0.0.0.0", _port);
        sock::Listen(_listenSock);
        _arr_fd[0] = _listenSock;
        for(int i = 1; i < NUM; ++i) {
            _arr_fd[i] = FD_NONE;
        }
        _dict.insert({"hello", "你好"});
        _dict.insert({"apple", "苹果"});
        _dict.insert({"phone", "手机"});
    }
    
    ~select_server() {
        if (_listenSock > 0) {
            close(_listenSock);
        }
    }

    void start() {
        fd_set rfds;
        fd_set wfds;
        while (true) {
            //select的第五个参数，代表每五秒进行一次轮询检查，
            //设置为{0，0}代表非阻塞等待，设置为nullptr代表阻塞等待
            //struct timeval timeout = { 5,0 };
            //
            //logmessage(DEBUG, "server start seccess...");
            DebugPrint();
            int maxfd = _listenSock;
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            for(int i = 0; i < NUM; ++i){
                if(_arr_fd[i] == FD_NONE) continue;
                if (maxfd < _arr_fd[i]) maxfd = _arr_fd[i];
                FD_SET(_arr_fd[i], &rfds);
                //if(i > 0)
                //    FD_SET(_arr_fd[i], &wfds);
            }
            // int select(int nfds, fd_set *readfds, fd_set *writefds,
            //      fd_set *exceptfds, struct timeval *timeout);
            //其中readfds，writefds，exceptfds都是输入输出型参数,每次在调用select之后都会被设置为
            //检测到新事件的文件描述符，所以在每次调用前需要重新设置。
            
            int s = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            
            if(s == 0) {
                //等待中
                logmessage(NORMAL, "time out...");
            }
            else if (s == -1) {
                logmessage(ERROR, "select error# %d : %s", errno, strerror(errno));
            }
            else {
                //成功检测到事件
                logmessage(NORMAL, "get a new event...");
                handlerEvent(rfds);
            }
        }
    }
private:
    void handlerEvent(fd_set &rfds){//, fd_set &wfds){
        //就绪的可能时多个io接口
        for (int i = 0; i < NUM; ++i) 
        {
            //如果文件描述符不合法，那就跳过
            if (_arr_fd[i] == FD_NONE) continue;
            //合法的也不一定就绪，检测就绪位图中是否包含某个文件描述符，如果包含，那就处理
            if (FD_ISSET(_arr_fd[i], &rfds))
            {
                //合法且就绪的文件描述符接口，分两种情况
                //第一种，是请求被连接的
                if (_arr_fd[i] == _listenSock) 
                    Accepter();
                //第二种情况，普通请求
                else if (FD_ISSET(_arr_fd[i], &rfds))
                    Recver(i);
                //else if (FD_ISSET(_arr_fd[i], &wfds))
                //    Send(i);
            }
        }
    }
    void Accepter() {
        std::string userip;
        uint16_t userport;
        int serviseSock = sock::Accept(_listenSock, userip, userport);
        logmessage(DEBUG, "get a new link success # [%s:%d]:%d", userip.c_str(), userport, serviseSock);
        int pos = 1;
        for (; pos < NUM; ++pos) {
            if (_arr_fd[pos] == FD_NONE) break;
        }
        if (pos == NUM) {
            logmessage(DEBUG, "select server already full.close...");
            close(serviseSock);
        }
        else{
            _arr_fd[pos] = serviseSock;
        }
    }
    void Recver(int pos) {
        logmessage(DEBUG, "message in get IO event: %d", _arr_fd[pos]);
        //对消息进行读取
        std::string inbuffer;
        if (!sock::Recv(_arr_fd[pos],inbuffer)) {
            _arr_fd[pos] = FD_NONE;
            return ;
        }
        inbuffer[inbuffer.size() - 2] = '\0';
        logmessage(DEBUG, "client[%d]:[%s]",_arr_fd[pos], inbuffer.c_str());
        auto it = _dict.find(inbuffer.c_str());
        std::string res;
        if (it == _dict.end())
            res = "词典词库并没未更新，所以未能翻译！";
        else res = it->second;
        _response.insert({_arr_fd[pos], res});
        Send(pos);
    }
    void Send(int pos) {
        auto it = _response.find(_arr_fd[pos]);
        if (it == _response.end()) return ;
        sock::Send(_arr_fd[pos],it->second);
        _response.erase(_arr_fd[pos]);
        logmessage(DEBUG, "send success...");
    }
    void DebugPrint() {
        std::cout << "_arr_fd[]:";
        for (int i = 0; i < NUM; ++i) {
            if (_arr_fd[i] == FD_NONE) continue;
            std::cout << _arr_fd[i] << " ";
        }
        std::cout << std::endl;
    }

};