

#include "epoll_server.hpp"

#include <memory>

void response(std::string str) {
    std::cout << str << std::endl;
}

int main() {
    std::unique_ptr<epoll_server> server(new epoll_server(response));
    server->start();
    return 0;
}