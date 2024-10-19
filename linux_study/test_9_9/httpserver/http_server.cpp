

#include "http_server.hpp"
#include <cstdlib>
#include <fstream>

#define WEBROOT "./wwwroot"
#define INDEX "/index.html"

void httpServerRequest(int serviseSock)
{
    std::string inbuffer;

    if (!HttpServer::Recv(serviseSock, inbuffer))
    {
        exit(10);
    }
    std::cout << inbuffer << std::endl;
    //截取请求的文件名
    std::size_t pos = inbuffer.find("\n");
    std::string requestLine = inbuffer.substr(0,pos);
    //std::cout << requestLine << std::endl;
    std::size_t left = requestLine.find(" ") + 1;
    std::size_t right = requestLine.find(" ", left);
    std::string filename = requestLine.substr(left, right - left);
    std::cout << filename << std::endl;

    std::string requestFile = WEBROOT;
    if (filename == "/") requestFile += INDEX;
    else requestFile += filename;
    std::ifstream in(requestFile);
    std::string responseFile;
    std::string line;
    while (std::getline(in,line)) {
        responseFile += line;
    }
    
    std::string sendStr;
    if (responseFile.empty()) sendStr += "http/1.1 404 NotFound\r\n";
    else sendStr += "http/1.1 200 OK\r\n";
    sendStr += "\r\n";
    sendStr += responseFile;
    HttpServer::Send(serviseSock,sendStr);
}

int main(int argc, char *args[])
{
    if (argc != 2)
    {
        exit(1);
    }

    int16_t port = atoi(args[1]);

    std::unique_ptr<HttpServer> server(new HttpServer(port, httpServerRequest));
    server->strat();
}
