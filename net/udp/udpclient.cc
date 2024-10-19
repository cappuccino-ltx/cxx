


#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>




int main(int argc, char ** argv) {

    if (argc != 3) exit(1);
    int16_t port = std::stoi(argv[2]);
    std::string ip = argv[1];

    //SOCK_DGRAM
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    std::string buffer;
    char recvbuff[4096] = { 0 };
    for (;;) {
        std::cout << "enter#";
        std::getline(std::cin,buffer);
        if (buffer == "quit")
            break;
        sendto(sock,buffer.c_str(), buffer.size(),0,(const struct sockaddr*)&addr,sizeof(addr));
        int n = recv(sock,recvbuff,sizeof(recvbuff) - 1, 0);
        if(n <=  0) break;
        std::cout << recvbuff << std::endl;
    }

    return 0;
}
