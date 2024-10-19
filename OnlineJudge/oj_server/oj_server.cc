
#include <cstdlib>

#include <iostream>

#include <signal.h>

#include "../comm/httplib.h"
#include "oj_control.hpp"
#include "../comm/info.hpp"

using namespace httplib;
using namespace control;

static Control* ctl_ptr = nullptr;

void header(int signo) {
    info(1,"header()") << "执行信号捕捉函数" << std::endl;
    ctl_ptr->RecoveryMachine();
}

int main(int argc, char* args[]) {

    // 绑定让挂掉的主机上线的功能
    info(1,"main()") << "绑定SIGQUIT信号自定义函数，让下线编译服务可以重新上线" << std::endl;
    signal(SIGQUIT, header);

    //对用户处理的请求进行路由
    info(1,"main()") << "创建httplib::Server对象" << std::endl;
    Server ser;
    info(1,"main()") << "创建Control对象" << std::endl;
    Control ctr;
    ctl_ptr = &ctr;

    // 获取全部的题目列表、
    info(1,"main()") << "注册Get::/all_questions服务处理函数" << std::endl;
    ser.Get("/all_questions",[&ctr](const Request& request, Response& response){
        //获取全部的题目
        std::string html;
        ctr.GetAllQuestions(html);
        response.set_content(html,"text/html;charset=utf-8");
    });

    // 根据用户的请求，获取相应的题目内容
    info(1,"main()") << "注册Get::/question/n服务处理函数" << std::endl;
    ser.Get(R"(/question/(\d+))", [&ctr](const Request& request, Response& response){
        std::string number = request.matches[1];
        //response.set_content("这是指定的一道题" + number, "text/plain;charset=utf-8");
        std::string html;
        ctr.GetOneQuestions(number,html);
        response.set_content(html, "text/html;charset=utf-8");
    }) ;

    // 根据用户提交的代码进行判体，并返回执行的结果
    info(1,"main()") << "注册Post::/judge/n服务处理函数" << std::endl;
    ser.Post(R"(/judge/(\d+))", [&ctr](const Request& request, Response& response){
        std::string number = request.matches[1];
        //LOG(DEBUG) << "收到判题请求" << std::endl;
        //response.set_content("这是要判定的题目" + number, "text/plain;charset=utf-8");
        //std::cout << request.body << std::endl;
        //std::string number = request.matches[1];
        //获取题目之后需要在获取请求体，也就是提交的代码
        std::string _out;
        ctr.Judge(number,request.body,_out);
        response.set_content(_out,"application/json;charset=utf-8");
    });

    //设置项目根目录
    info(1,"main()") << "设置项目根路径./wwwroot" << std::endl;
    ser.set_base_dir("./wwwroot");
    LOG(INFO) << "服务器启动成功" << std::endl;
    //设置监听，启动服务器
    info(1,"main()") << "开始监听服务listen..." << std::endl;
    ser.listen("0.0.0.0",atoi(args[1]));

    return 0;
}
