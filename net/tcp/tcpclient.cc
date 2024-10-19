

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        std::cout << "usage " << argv[0] << " + ip + port " << std::endl;
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip.c_str(), &(server.sin_addr));
    server.sin_port = htons(server_port);

    for (;;)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            std::cout << "failed to create socket!!!" << std::endl;
            exit(2);
        }

        int ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
        if (ret < 0)
        {
            std::cout << "failed to connect to " << server_ip << ":" << server_port << std::endl;
            exit(3);
        }
        std::cout << "connect successfully to " << server_ip << ":" << server_port << std::endl;
        std::string input;
        char buffer[4096];
        std::cout << "echo #> ";
        std::getline(std::cin, input);
        int n = write(sock, input.c_str(), input.size());
        if (n < 0)
        {
            std::cout << "failed to write" << std::endl;
        }
        n = read(sock, buffer, sizeof(buffer));
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "server echo #: " << buffer << std::endl;
        }
        else
        {
            std::cout << "this connection is abnormal !!!" << std::endl;
            break;
        }
    }
    return 0;
}
