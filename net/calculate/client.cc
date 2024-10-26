


#include "protocol.hpp"
#include "sock.hpp"

#include <iostream>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace ns_sock;
using namespace ns_protocol;

int main(int argc, char** argv) {
    if (argc < 3){
        std::cout << "usage " << argv[0] << " + ip + port" << std::endl;
        exit(1);
    }
    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);
    char opts[] = "+-*/";

    srand(time(nullptr));

    psock sock(new Sock());
    
    
    int count = 10000;
    while (count--) {
        int x = rand() % 1000;
        int y = rand() % 1000;
        int opt = rand() % 4;
        Request req(x,y,opts[opt]);
        sock->Socket();
        sock->Connect(ip,port);
        sock->Write(req.Encode());

        std::string buff;
        sock->Read(&buff);
        sock->Close();

        Response res(buff);
        std::cout << req.x_ << " " << req.opt_ << " " << req.y_ << " = " << res.result_ << "      code: " << res.code_ << std::endl;
        //std::cout << req.Encode() << "  " << res.Encode() << std::endl;
    }

    return 0;
}