

#include "tcpserver.hpp"

#include <iostream>
#include <string>
#include <memory>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void daemon(const std::string path = "") {

    signal(SIGCLD,SIG_IGN);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGHUP,SIG_IGN);

    if(fork() > 0) exit(0);
    setsid();

    if (!path.empty()) chdir(path.c_str());

    int fd = open("/dev/null",O_RDWR);
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    close(fd);

}


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage " << argv[0] << " + port" << std::endl;
        exit(1);
    }
    int16_t port = atoi(argv[1]);
    daemon();
    std::shared_ptr<ns_tcp::TcpServer> server(new ns_tcp::TcpServer([](std::string str){
        //sleep(3);
        return str;
    }));
    server->InitServer(port);
    server->Start();
    return 0;
}