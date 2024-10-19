
#include "tcp_server.hpp"
#include <memory>

void request(connection *conn) {
    conn->_outbuffer += conn->_inbuffer.c_str();
    conn->_inbuffer.erase(0,conn->_inbuffer.size());
    conn->_server->EnableReadWrite(conn, true, true);
}

int main() {
    std::unique_ptr<tcp_server> server(new tcp_server(request));
    server->dispather();
    return 0;
}