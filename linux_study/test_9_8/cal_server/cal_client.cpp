
#include "cal_server.hpp"

#include "protocol.hpp"

int main(int argc, char *args[])
{
    if (argc != 3)
    {
        std::cout << "请正确输入" << std::endl;
        exit(1);
    }
    std::string ip = args[1];
    int16_t port = atoi(args[2]);

    sock s;
    int serverSock = s.Socket();
    // 获取连接
    s.Connect(serverSock, ip, port);

    while (1)
    {
        request req;
        std::cout << "请输入要计算的式子：" ;
        std::cin >> req._x >> req._op >> req._y;
        std::cout << req._x << " " << req._y << std::endl;
        //序列化数据
        std::string req_str = req.Serialize();
        //添加数据信息
        req_str = Encode(req_str);
        s.Send(serverSock,req_str);
        std::string buffer;
        while (1) {
            s.Recv(serverSock,buffer);
            std::string package = Decode(buffer);
            if (package.empty()){
                continue;
            }
            response res;
            res.Deserialize(package);
            if(res._code == 1) {
                std::cout << "除0错误！计算失败" <<  std::endl;
                break;
            }
            else if(res._code == 2) {
                std::cout << "除余0错误！计算失败" <<  std::endl;
                break;
            }
            else if(res._code == 3) {
                std::cout << "输入错误！计算失败" <<  std::endl;
                break;
            }
            else if (res._code == 0) {
                std::cout << "计算状态：" << res._code << std::endl;
                std::cout << "计算结果：" << res._result << std::endl;
                break;
            }
        }
    }
    close(serverSock);
    return 0;
}