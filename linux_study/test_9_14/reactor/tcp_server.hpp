

#include "sock.hpp"
#include "epoll.hpp"

#include <functional>
#include <unordered_map>

class connection;
class tcp_server;

using function = std::function<void(connection *)>;

// 给每个sock都维护一个写入写出缓冲区
class connection
{
public:
    connection(int sock = -1) : _sock(sock), _server(nullptr) {}
    ~connection() {}
    void bind(function recv, function send, function except)
    {
        _recv_cb = recv;
        _send_cb = send;
        _except_cb = except;
    }

public:
    int _sock;
    function _recv_cb;
    function _send_cb;
    function _except_cb;

    std::string _inbuffer;
    std::string _outbuffer;
    // 维护一个回指指针
    tcp_server *_server;
};

static const int gnum = 128;

class tcp_server
{

private:
    int _listenSock;
    int _epfd;
    // 维护每个组织sock的connection
    std::unordered_map<int, connection *> _connections;
    struct epoll_event *_revs;
    int _revs_num;
    //回调函数
    function _cb;
public:
    tcp_server(function cb):_revs_num(gnum),_cb(cb)
    {
        // 创建套接字
        _listenSock = sock::Socket();
        sock::Bind(_listenSock, "0.0.0.0", 8080);
        sock::Listen(_listenSock);

        // 创建epoll模型
        _epfd = epoll::create();
        // 将_listenSock维护起来。
        add_sock_to_epoll(_listenSock, std::bind(&tcp_server::accept, this, std::placeholders::_1),
                          nullptr, nullptr);
        _revs = new struct epoll_event[_revs_num];
    }

    ~tcp_server() {}

    void dispather() {
        while (true) {
            loopOnce();
        }
    }

    void EnableReadWrite(connection *conn, bool readable, bool writeable){
        uint32_t events = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0));
        epoll::ctrl(_epfd, EPOLL_CTL_MOD, conn->_sock, events);
    }

private:
    void accept(connection *conn)
    {
        // 因为这个服务器使用epoll的ET工作模式模式（默认是LT,）设置sock为非阻塞运行，
        // 调用相关的函数要是执行不成功则会返回错误，不会阻塞等待
        while (true)
        {
            std::string userip;
            uint16_t userport = 0;
            int accept_errno = 0;
            int serviseSock = sock::Accept(conn->_sock, userip, userport, accept_errno);
            if (serviseSock < 0)
            {
                if (accept_errno == EAGAIN || accept_errno == EWOULDBLOCK) {
                    //表示没有可用数据
                    break;
                }
                else if (accept_errno == EINTR) {
                    //表示因为系统调用被中断,概率非常低
                    continue;
                }
                else {
                    //调用accept失败
                    logmessage(WARNING, "accept error %d:%s", accept_errno, strerror(accept_errno));
                    break;
                }
            }
            //读取成功后交给connection托管
            add_sock_to_epoll(serviseSock, std::bind(&tcp_server::Recv, this, std::placeholders::_1), std::bind(&tcp_server::Send, this, std::placeholders::_1), std::bind(&tcp_server::Except, this, std::placeholders::_1));
        }
    }
    void add_sock_to_epoll(int sock, function recver, function send, function except)
    {
        // 设置sock为非阻塞
        sock::setNonBlock(sock);

        // 添加sock到维护表中
        connection *conn = new connection(sock);
        conn->_server = this;
        conn->bind(recver, send, except);
        _connections.insert(std::make_pair(sock, conn));

        // 将sock添加到epoll模型中
        epoll::ctrl(_epfd, EPOLL_CTL_ADD, sock, EPOLLIN | EPOLLET);
    }
    

    void Recv(connection *conn) {
        const int buffer_size = 1024;
        int err = false;
        while (true) {
            char buffer[buffer_size];
            ssize_t s = recv(conn->_sock, buffer, sizeof(buffer_size) - 1, 0);
            if (s < 0) {
                //表示数据区没有读到数据
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                //表示被系统调用中断
                if (errno == EINTR) continue;
                //读取失败
                else {
                    logmessage(WARNING, "recv error ,%d:%s", errno, strerror(errno));
                    conn->_except_cb(conn);
                    err = true;
                    break;
                }
            }
            else if (s == 0) {
                //表示断开连接了
                logmessage(DEBUG, "client[%d]:quit,server close [%d]", conn->_sock, conn->_sock);
                conn->_except_cb(conn);
                break;
            }
            else {
                //读取成功
                buffer[s] = '\0';
                conn->_inbuffer += buffer; 
            }
        }
        logmessage(DEBUG, "conn->_inbuffer[sock:%d]:%s", conn->_sock, conn->_inbuffer.c_str());
        if (!err){
            //读取成功
            _cb(conn);
        }
    }
    void Send(connection *conn) {
        while (true) {
            ssize_t s = send(conn->_sock, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
            if (s > 0) {
                conn->_outbuffer.erase(0, s);
                if (conn->_outbuffer.empty()) break;
            }
            else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                else if (errno == EINTR) continue;
                else {  
                    logmessage(ERROR, "send error,%d:%s",errno, strerror(errno));
                    conn->_except_cb(conn);
                    break;
                }
            }
        }
        if (conn->_outbuffer.empty()) EnableReadWrite(conn, true, false);
        else EnableReadWrite(conn, true, true);
    }
    void Except(connection *conn) {
        if (!IsConnectionExists(conn->_sock)) return;
        //从epoll中移除
        epoll::ctrl(_epfd, EPOLL_CTL_DEL, conn->_sock, 0);

        //从connections中移除
        _connections.erase(conn->_sock);
        //关闭sock
        close(conn->_sock);
        //释放conn
        delete conn;
        logmessage(DEBUG, "异常发生，回收资源完毕");
    }

    void loopOnce() {
        int n = epoll::wait(_epfd, _revs, _revs_num);
        for (int i = 0; i < n; ++i) {
            int sock = _revs[i].data.fd;
            uint32_t revents = _revs[i].events;
            if (revents & EPOLLIN) {
                if (IsConnectionExists(sock) && _connections[sock]->_recv_cb != nullptr) {
                    _connections[sock]->_recv_cb(_connections[sock]);
                }
            }
            if (revents & EPOLLOUT) {
                if (IsConnectionExists(sock) && _connections[sock]->_send_cb != nullptr){
                    _connections[sock]->_send_cb(_connections[sock]);
                }
            }
        }
    }
    bool IsConnectionExists(int sock) {
        return _connections.find(sock) != _connections.end();
    }
};