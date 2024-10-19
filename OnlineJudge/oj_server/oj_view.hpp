#pragma once 


#include <iostream>
#include <string>
#include <vector>

#include <ctemplate/template.h>

#include "oj_model1.hpp"
//#include "oj_model2.hpp"
#include "../comm/log.hpp"
#include "../comm/info.hpp"


namespace view{
    using namespace model;
    using namespace logger;

    const std::string template_path = "./template_html/";

    class View{
    public:

        View() {
            info(2,"View()") << "创建View对象" << std::endl;
        }
        void AllExpandHTML(const std::vector<Question>& ques, std::string& html){
            //在网页上显示题目的列表的时候，只需要显示
            //题目的编号，题目的标题，题目的难度
            
            // 1.形成路径
            const std::string src_html = template_path + "all_questions.html";
            // 形成数字典
            ctemplate::TemplateDictionary root("all_questions");
            for (const auto& q : ques) {
                ctemplate::TemplateDictionary* sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number",q.number);
                sub->SetValue("title",q.title);
                sub->SetValue("star",q.star);
            }
            //获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html,ctemplate::DO_NOT_STRIP);
            //开始渲染，
            tpl->Expand(&html,&root);
        }
        bool OneExpandHTML(const Question& q, std::string& html){
            // 1.形成路径
            const std::string src_html = template_path + "one_questions.html";
            // 2.形成数字典
            ctemplate::TemplateDictionary root("ont_questions");
            root.SetValue("number",q.number);
            root.SetValue("title",q.title);
            root.SetValue("star",q.star);
            root.SetValue("desc",q.desc);
            root.SetValue("pre_code",q.header);
            // 3.获取被渲染的html
            ctemplate::Template* tql = ctemplate::Template::GetTemplate(src_html,ctemplate::DO_NOT_STRIP);
            // 4.开始渲染
            tql->Expand(&html,&root);
            return true;
        }
    private:

    };
}
