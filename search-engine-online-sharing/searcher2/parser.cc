#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "util.hpp"

//是一个目录，下面放的是所有的html网页
const std::string src_path = "data/input";
const std::string output = "data/raw_html/raw.txt";

typedef struct DocInfo{
    std::string title;   //文档的标题
    std::string content; //文档内容
    std::string url;     //该文档在官网中的url
}DocInfo_t;

//const &: 输入
//*: 输出
//&：输入输出
bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list);
bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results);
bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output);

int main()
{
    std::vector<std::string> files_list;
    //第一步: 递归式的把每个html文件名带路径，保存到files_list中，方便后期进行一个一个的文件进行读取
    if(!EnumFile(src_path, &files_list)){
        std::cerr << "enum file name error!" << std::endl;
        return 1;

    }
    //第二步: 按照files_list读取每个文件的内容，并进行解析
    std::vector<DocInfo_t> results;
    if(!ParseHtml(files_list, &results)){
        std::cerr << "parse html error" << std::endl;
        return 2;
    }

    //第三步: 把解析完毕的各个文件内容，写入到output,按照\3作为每个文档的分割符
    if(!SaveHtml(results, output)){
        std::cerr << "sava html error" << std::endl;
        return 3;
    }

    return 0;
}

bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list)
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);

    //判断路径是否存在，不存在，就没有必要再往后走了
    if(!fs::exists(root_path)){
        std::cerr << src_path << " not exists" << std::endl;
        return false;
    }

    //定义一个空的迭代器，用来进行判断递归结束
    fs::recursive_directory_iterator end;
    for(fs::recursive_directory_iterator iter(root_path); iter != end; iter++){
        //判断文件是否是普通文件，html都是普通文件
        if(!fs::is_regular_file(*iter)){ 
            continue;
        }
        if(iter->path().extension() != ".html"){ //判断文件路径名的后缀是否符合要求
            continue;
        }
        //std::cout << "debug: " << iter->path().string() << std::endl;
        //当前的路径一定是一个合法的，以.html结束的普通网页文件
        files_list->push_back(iter->path().string()); //将所有带路径的html保存在files_list,方便后续进行文本分析
    }

    return true;
}

static bool ParseTitle(const std::string &file, std::string *title)
{
    std::size_t begin = file.find("<title>");
    if(begin == std::string::npos){
        return false;
    }
    std::size_t end = file.find("</title>");
    if(end == std::string::npos){
        return false;
    }

    begin += std::string("<title>").size();

    if(begin > end){
        return false;
    }
    *title = file.substr(begin, end - begin);
    return true;
}

static bool ParseContent(const std::string &file, std::string *content)
{
    //去标签,基于一个简易的状态机
    enum status{
        LABLE,
        CONTENT
    };

    enum status s = LABLE;
    for( char c : file){
        switch(s){
            case LABLE:
                if(c == '>') s = CONTENT;
                break;
            case CONTENT:
                if(c == '<') s = LABLE;
                else {
                    //我们不想保留原始文件中的\n,因为我们想用\n作为html解析之后文本的分隔符
                    if(c == '\n') c = ' ';
                    content->push_back(c);
                }
                break;
            default:
                break;
        }
    }

    return true;
}

static bool ParseUrl(const std::string &file_path, std::string *url)
{
    std::string url_head = "https://www.boost.org/doc/libs/1_85_0/doc/html";
    std::string url_tail = file_path.substr(src_path.size());

    *url = url_head + url_tail;
    return true;
}

//for debug
static void ShowDoc( const DocInfo_t &doc)
{
    std::cout << "title: " << doc.title << std::endl;
    std::cout << "content: " << doc.content << std::endl;
    std::cout << "url: " << doc.url << std::endl;
}

bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results)
{
    for(const std::string &file : files_list){
        //1. 读取文件，Read();
        std::string result;
        if(!ns_util::FileUtil::ReadFile(file, &result)){
            continue;
        }
        DocInfo_t doc;
        //2. 解析指定的文件，提取title
        if(!ParseTitle(result, &doc.title)){
            continue;
        }
        //3. 解析指定的文件，提取content,就是去标签
        if(!ParseContent(result, &doc.content)){
            continue;
        }
        //4. 解析指定的文件路径，构建url
        if(!ParseUrl(file, &doc.url)){
            continue;
        }

        //done,一定是完成了解析任务，当前文档的相关结果都保存在了doc里面
        results->push_back(std::move(doc)); //bug:todo;细节，本质会发生拷贝，效率可能会比较低

        //for debug
        //ShowDoc(doc);
        //break;
    }
    return true;
}

bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output)
{
#define SEP '\3'
    //按照二进制方式进行写入
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if(!out.is_open()){
        std::cerr << "open " << output << " failed!" << std::endl;
        return false;
    }

    //就可以进行文件内容的写入了
    for(auto &item : results){
        std::string out_string;
        out_string = item.title;
        out_string += SEP;
        out_string += item.content;
        out_string += SEP;
        out_string += item.url;
        out_string += '\n';

        out.write(out_string.c_str(), out_string.size());
    }

    out.close();

    return true;
}


