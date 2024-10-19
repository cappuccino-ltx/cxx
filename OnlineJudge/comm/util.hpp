#pragma once

#include <iostream>
#include <string>
#include <atomic>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <boost/algorithm/string.hpp>

//格式化要用的头文件 
#include <ctime>    //localtime
#include <iomanip>  //put_time
#include <sstream>

namespace util{
    //时间操作的相关类
    class TimeUtil{
    public:
        //获取时间戳
        static time_t _getTimeStamp(){
            struct timeval t;
            gettimeofday(&t,nullptr);
            return t.tv_sec;
        }
        //获取字符串形式的时间戳
        static std::string getTimeStamp() {
            return std::to_string(_getTimeStamp());
        }
        static std::string getDateTime() {
            //对时间戳进行格式化
            //std::stringstream ss;
            char buff[32];
            time_t t = _getTimeStamp();
            struct tm* timer = localtime(&t);
            std::strftime(buff,sizeof(buff),R"(%Y-%m-%d/%H:%M:%S)",timer);
            //ss << std::put_time(timer,R"(%Y-%m-%d/%H:%M:%S)");//put_time
            return buff;
        }
    };
    //进行路径拼接的工具类
    class Splice{
        static std::string PathSuffix(const std::string& filename, const std::string& suf) {
            std::string ret = "./temp/";
            ret += filename;
            ret += suf;
            return ret;
        }
    public:
        static std::string Src(const std::string & filename) {
            return PathSuffix(filename,".cc");
        }
        static std::string Err_com(const std::string & filename) {
            return PathSuffix(filename,".err_com");
        }
        static std::string Exe(const std::string& filename) {
            return PathSuffix(filename,".exe");
        }
        static std::string Err_run(const std::string & filename) {
            return PathSuffix(filename,".err_run");
        }
        static std::string Out(const std::string& filename) {
            return PathSuffix(filename,".out");
        }
        static std::string In(const std::string & filename){
            return PathSuffix(filename,".in");
        }
    };
    //文件操作的相关类
    class FileUtil{
    public:
        static bool FileExists(const std::string& file) {
            struct stat st;
            if (stat(file.c_str(), &st) == 0) {
                //获取文件信息成功，那就代表，这个文件是存在的， -1代表失败
                return true;
            }
            return false;
        }

        static bool WriteFile(const std::string& filename,std::string& code) {
            std::ofstream out(filename);
            if (!out.is_open()) {
                return false;
            }
            out.write(code.c_str(),code.size());
            out.close();
            return true;
        }

        static bool ReadFile(const std::string& filename, std::string* target, bool keep = true) {
            std::ifstream in(filename);
            if (!in.is_open()) return false;
            std::string line;
            while(std::getline(in, line)) {
                (*target) += line;
                (*target) += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }

        //生成一个具有唯一标识的文件名
        static std::string UniqFileName() {
            //定义一个原子自增的变量
            static std::atomic_uint id(0);
            //获取时间戳
            std::string time_stamp = util::TimeUtil::getTimeStamp();
            return time_stamp + "_" + std::to_string(id);
        }
    };

    class StringUtil{
    public:
        static void SplitString(const std::string& src,std::vector<std::string>& tar,const std::string& sep) {
            boost::split(tar,src,boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}
