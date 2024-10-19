
#include "poll_server.hpp"
#include <memory>

int main() {
    std::unique_ptr<poll_server> server(new poll_server());
    server->start();
    return 0;
}