
#include "cal_server.hpp"
#include "protocol.hpp"
#include "deamon.hpp"
#include <memory>

response calculate(request req)
{
    response res;
    switch (req._op)
    {
    case '+':
        res._code = 0;
        res._result = (req._x + req._y);
        break;
    case '-':
        res._code = 0;
        res._result = (req._x - req._y);
        break;
    case '*':
        res._code = 0;
        res._result = (req._x * req._y);
        break;
    case '/':
        if (req._y == 0)
        {
            res._code = 1; // 除0错误;
            res._result = 0;
            break;
        }
        res._code = 0;
        res._result = (req._x / req._y);
        break;
    case '%':
        if (req._y == 0)
        {
            res._code = 2; // 除余0错误;
            res._result = 0;
            break;
        }
        res._code = 0;
        res._result = (req._x % req._y);
        break;
    default:
        res._code = 3;
        res._result = 0;
        break;
    }
    return res;
}

void calcullator(int serviseSock)
{
    std::string inbuffer;
    while (1)
    {
        // 接收字符流
        if (!cal::cal_server::Recv(serviseSock, inbuffer))
        {
            break;
        }
        // 分析字符流，
        std::string package = Decode(inbuffer);
        if (package.empty())
        {
            continue;
        }
        // 拿到了一个完整的报文
        // 进行反序列化
        request req;
        req.Deserialize(package);
        // 进行运算
        response res = calculate(req);
        // 进行序列化
        std::string res_string = res.Serialize();
        res_string = Encode(res_string);
        cal::cal_server::Send(serviseSock, res_string);
    }
}

int main(int argc, char *args[])
{
    if (argc != 2)
    {
        std::cout << "请正确输入！" << std::endl;
        exit(1);
    }
    mydeamon();
    std::string ip = "0.0.0.0";
    int16_t port = atoi(args[1]);
    std::unique_ptr<cal::cal_server> server(new cal::cal_server(port, ip));
    server->BindServise(calcullator);
    server->start();
    return 0;
}
