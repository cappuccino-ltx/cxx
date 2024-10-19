#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <jsoncpp/json/json.h>
#include <unistd.h>

#include "../comm/log.hpp"
#include "oj_model1.hpp"
#include "oj_view.hpp"
#include "compile_model.hpp"

namespace control{
    using namespace logger;
    using namespace model;
    using namespace view;
    using namespace compile_server;

    class Control{
    public:
        Control() {
        }
        bool GetAllQuestions(std::string& html) {
            std::vector<Question> ques_list;
            if (!_mod.GetAllQuestions(ques_list)){
                LOG(ERROR) << "获取题目列表失败" << std::endl;
                html = "获取题目列表失败";
                return false;
            }
            //对题目进行排序，
            sort(ques_list.begin(),ques_list.end(),[](const Question& q1,const Question& q2){
                return atoi(q1.number.c_str()) < atoi(q2.number.c_str());
            });
            _view.AllExpandHTML(ques_list,html);
            return true;
        }
        bool GetOneQuestions(const std::string& number, std::string& html) {
            Question q;
            if(!_mod.GetOneQuestion(number,q)) {
                LOG(ERROR) << "未找到题目，题号：" << number << std::endl;
                html = "未找到题目";
                return false;
            }
            _view.OneExpandHTML(q,html);
            return true;
        }

        void Judge(const std::string& number, const std::string & in_json,std::string& out_json) {
            // 根据题号拿到相应的题目细节，
            Question que;
            _mod.GetOneQuestion(number,que);
            // 对提交的代码进行反序列化，拿到用户提交的源码
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json,in_value);
            //重新拼接代码
            Json::Value compile_value;
            std::string code = in_value["code"].asString();
            code += "\n#define COMPILER_ONLINE\n";
            code += que.tail;
            compile_value["code"] = code; 
            compile_value["input"] = in_value["input"].asString();
            compile_value["cpu_limit"] = que.cpu_limit;
            compile_value["mem_limit"] = que.mem_limit;

            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);
            // 选择负载最低的主机，（这里需要差错处理，）需要一直选择，直到选择到合适的主机，否则就是全部都挂掉了
            
            while(true){
                //sleep(1);
                int id = -1;
                Machine* m = nullptr;
                if(!_blance.SmartChoice(&id, &m)) {
                    break;
                }
                //_blance.AllOnline();
                // 然后发起http请求，拿到代码执行的结果，
                httplib::Client cli(m->_ip,m->_port);
                // 增加负载
                m->IncLoad();
                LOG(INFO) << "选择主机id:" << id  << " "<< m->_ip << ":" << m->_port << "，当前负载:" << m->load() << std::endl;
                //进行请求，获取答案
                if (auto res = cli.Post("/compile_run",compile_string,"application/json;charset=utf-8")){
                    //请求成功，将结果返回给out_json;
                    if(res->status == 200) {
                        out_json = res->body;
                        m->DecLoad();
                        //LOG(INFO) << "编译运行成功" << std::endl; 
                        break;
                    }
                    m->DecLoad();
                }
                else{
                    //请求失败
                    LOG(ERROR) << "当前主机请求失败" << m->_ip << ":" << m->_port <<"可能已经离线" << std::endl;;
                    _blance.offMachine(id);
                    _blance.ShowMachines();//显示当前的主机列表
                }
            }
        }

        // 让所有挂掉的主机上线的功能
        void RecoveryMachine() {
            _blance.AllOnline();
        }
    private:
        Model _mod;
        View _view;
        LoadBlance _blance;
    };
}
