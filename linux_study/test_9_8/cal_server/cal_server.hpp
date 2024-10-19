

#include "sock.hpp"
#include <functional>

namespace cal
{
    class cal_server;
    struct thread_data
    {
        thread_data(int socket, cal_server *cal) : serviseSock(socket), _cal(cal) {}
        int serviseSock;
        cal_server *_cal;
    };

    typedef std::function<void(int)> function;
    class cal_server
    {
    private:
        static void *threadRoutine(void *arg)
        {
            pthread_detach(pthread_self());
            thread_data *td = (thread_data *)arg;
            td->_cal->Excute(td->serviseSock);
            close(td->serviseSock);
            delete td;
            return nullptr;
        }

    public:
        cal_server(int16_t port, std::string ip = "0.0.0.0")
        {
            _listenSock = _sock.Socket();
            _sock.Bind(_listenSock, ip, port);
            _sock.Listen(_listenSock);
        }
        ~cal_server() {
            if (_listenSock > 0) {
                close(_listenSock);
            }
        }

        void BindServise(function fun)
        {
            _fun = fun;
        }

        void Excute(int s)
        {
            _fun(s);
        }

        static void Send(int socket, std::string str){
            sock s;
            s.Send(socket, str);
        }
        static bool Recv(int socket, std::string &str) {
            sock s;
            return s.Recv(socket, str);
        }

        void start()
        {
            for (;;)
            {
                std::string userip;
                int16_t userport;
                int serviseSock = _sock.Accept(_listenSock, userip, userport);
                if (serviseSock < 0)
                {
                    continue;
                }
                logmassage(NORMAL, "create new link success sock: %d", serviseSock);
                pthread_t tid;
                thread_data *td = new thread_data(serviseSock, this);
                pthread_create(&tid, nullptr, threadRoutine, td);
            }
        }

    private:
        int _listenSock;
        function _fun;
        sock _sock;
    };
}
