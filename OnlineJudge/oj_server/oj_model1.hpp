#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>

#include <cassert>
#include <cstdlib>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "../comm/info.hpp"

namespace model{

    using namespace logger;
    using namespace util;

    //1 判断回文数 简单 1 30000
    class Question{
    public:
        std::string number;                 //题目的题号
        std::string title;          //题目的标题
        std::string star;           //题目的难度
        int cpu_limit;              //题目的时间限制
        int mem_limit;              //题目的内存限制
        std::string desc;           //题目的描述
        std::string header;         //题目的预设的编辑区的代码
        std::string tail;           //题目的测试用例，需要和提交的代码拼接，形成完整的代码

        //这个是预设的，如果某些题目是通过标准输入的形式进行提交的话，需要标准输入和正确的答案输出，
        
        std::string std_in;
        std::string std_out;
        
    };

    const std::string questions_list = "./questions/questions.list";
    const std::string questions_path = "./questions/";

    class Model{
        bool LoadQuestions() {
            info(3,"LoadQuestions()") << "加载题目调用LoadQuestions()" << std::endl;
            // 打开题目列表文件
            info(4,"LoadQuestions()") << "打开题目列表文件" << questions_list << std::endl;
            std::ifstream ques_list(questions_list);
            if (!ques_list.is_open()) {
                info(4,"LoadQuestions()") << "打开题目列表文件失败" << std::endl;
                return false;
            }
            info(4,"LoadQuestions()") << "循环构建题目_questions" << std::endl;
            std::string line;
            while(getline(ques_list, line)) {
                //对读取到的字符串进行分割，
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, tokens," ");
                if (tokens.size() < 5){
                    info(4,"LoadQuestions()") << "加载部分题目出错，请检查格式" << std::endl;
                    continue;
                }
                Question ques;
                ques.number = tokens[0];
                ques.title = tokens[1];
                ques.star = tokens[2];
                ques.cpu_limit = atoi(tokens[3].c_str());
                ques.mem_limit = atoi(tokens[4].c_str());

                //读取这个题目的描述，预设代码，以及测试代码
                FileUtil::ReadFile(questions_path + ques.number + "/" + "desc.txt",&ques.desc,true);
                FileUtil::ReadFile(questions_path + ques.number + "/" + "header.cpp", &ques.header, true);
                FileUtil::ReadFile(questions_path + ques.number + "/" + "tail.cpp", &ques.tail, true);

                _questions.insert({ques.number,ques});
            }
            info(4,"LoadQuestions()") << "加载题目成功" << std::endl;
            return true;
        }

    public:
        Model(){
            info(2,"Control()") << "创建Model对象" << std::endl;
            //在构造函数中去加载函数
            
            assert(LoadQuestions());
        }
        bool GetAllQuestions(std::vector<Question>& ques) {

            if (_questions.size() == 0) {
                LOG(FATAL) << "题目列表为空" << std::endl;
                return false;
            }
            for (auto q : _questions) {
                ques.push_back(q.second);
            }
            return true;
        }

        bool GetOneQuestion(const std::string& num,Question& ques) {
            const auto& q =  _questions.find(num);
            if (q == _questions.end()) {
                LOG(ERROR) << "用户获取题目失败,题目编号为： " << num << std::endl;
                return false;
            }
            ques = q->second;
            return true;
        }
    
    private:
        std::unordered_map<std::string,Question> _questions;
        //std::map<std::string,Question> _questions;
    };
}
