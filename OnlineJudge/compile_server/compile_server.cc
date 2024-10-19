

#include <iostream>
#include <jsoncpp/json/json.h>

#include "compile_run.hpp"
#include "../comm/httplib.h"



using namespace compile_run;
using namespace httplib;

int main(int argc, char* args[]) {
    
    if (argc != 2) {
        return 1;
    }
    Server ser;
    //注册post方法
    ser.Post("/compile_run",[](const Request& request,Response& response){
        std::string in_json = request.body;
        std::string out_json;
        //std::cout << in_json << std::endl;
        if (!in_json.empty()) {
            //std::cout << in_json << std::endl;
            Compile_Run::Start(in_json, &out_json);
            //LOG(INFO) << out_json << std::endl;
            response.set_content(out_json, "application/json;charset=utf-8");
        }
    });

    //绑定端口，启动服务器
    ser.listen("0.0.0.0",atoi(args[1]));


    // Json::Value in_json;
    // //Json::Value out_json;
    // in_json["code"] = R"(
    // #include <iostream>

    // int main() {
    //     std::cout << "hello compile_run" << std::endl;
    //     //while(1);
    //     //int* a = new int[1024 * 1024 * 1024];
    //     int* ptrr = NULL;
    //     *ptrr = 10;
    //     return 0;
    // }
    // )";
    // in_json["input"]  = "";
    // in_json["cpu_limit"] = 1;
    // in_json["mem_limit"] = 30 * 1024;
    // //Json::StreamWriterBuilder writer;
    // //std::string in = Json::writeString(writer,in_json);
    // Json::StyledWriter writer;
    // std::string in = writer.write(in_json);
    // std::string out;
    // compile_run::Compile_Run::Start(in,&out);
    // std::cout << in << std::endl;
    // std::cout << out << std::endl;
    return 0;
}
