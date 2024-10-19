#pragma once

#include <iostream>
#include <string>

#include "./util.hpp"

namespace logger{
    using namespace util;
    enum{
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    inline std::ostream& loger(const std::string & level, const std::string & filename, int line){
        //添加错误等级
        std::string  massage = "[";
        massage += level;
        massage += "]";

        //添加报错文件
        massage += "[";
        massage += filename;
        massage += "]";

        //添加报错行
        massage += "[";
        massage += std::to_string(line);
        massage += "]";

        //添加时间戳
        massage +="[";
        massage += TimeUtil::getDateTime();
        massage += "]";

        std::cout << massage;//这里不用用endl进行刷新
        return std::cout;
    }

    #define LOG(level) loger(#level,__FILE__,__LINE__)
}