
#include "select_server.hpp"
#include <memory>

int main() {
    std::unique_ptr<select_server> server(new select_server());
    server->start();
}