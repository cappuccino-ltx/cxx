#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <memory>
enum opentype{
    null,
    tofile,
    tostd
};

const char* de_path = "./debug.log";
opentype toout = tostd;

namespace ns_info{
    class info_log{
        
    public:
        
        info_log(const opentype& ty,const char* path)
            :out(path),type(ty)
        {}

        ~info_log(){
            out.close();
        }

        static info_log& get_info(const opentype& ty = toout,const char* path = de_path) {
            if (nullptr == info) {
                mtx.lock();
                if (nullptr == info) {
                    info = new info_log(ty,path);
                }
                mtx.unlock();
            }
            return *info;
        }

        std::ostream& operator()(int level,const std::string& mass,const std::string& file,int line){
            std::string buff;
            // 用两个空格来区分层次标识
            while(--level) buff += "|   ";
            // 指向函数名：
            //buff += "|---";
            buff += "<";
            buff += file;
            buff += " : ";
            buff += std::to_string(line);
            buff += "> ";
            buff += mass;
            buff += "~> ";
            
            // 返回流，继续插入具体的逻辑描述
            if (type == tofile) {
                out << buff;
                return out;
            }
            std::cout << buff;
            return std::cout;
        }
    private:
        std::ofstream out;
        opentype type;
        static info_log* info;
        static std::mutex mtx;
    };
    info_log* info_log::info = nullptr;
    std::mutex info_log::mtx;
}


#define info(level,mass) ns_info::info_log::get_info()(level,mass,__FILE__,__LINE__)