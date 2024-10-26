#pragma once

#include <iostream>
#include <string>

using namespace std::literals;

namespace ns_protocol{

    const std::string endf = "...";
    const std::string segf = " ";

    //    5_1_+_1\n\n
    class Request{
    public:
        Request(long long x = 0, long long y = 0,char opt = ' ') 
            :x_(x),y_(y),opt_(opt)
        {}
        Request(const std::string& mass) {
            Decode(mass);
        }
        ~Request() {}
        
        std::string Encode() {
            
            std::string ret = segf;
            ret += std::to_string(x_);
            ret += segf;
            ret += opt_;
            ret += segf;
            ret += std::to_string(y_);
            ret += endf;

            int len = ret.size();
            ret = std::to_string(len) + ret;

            return ret;
        }

        bool Decode(std::string mass) {
            size_t begin = 0;
            size_t end = mass.find(segf,begin);
            if(end == std::string::npos) return false;
            int size = std::stoi(mass.substr(begin,end));
            
            begin = ++end;end = mass.find(segf,end);
            if(end == std::string::npos) return false;
            x_ = std::stoi(mass.substr(begin,end));

            begin = ++end;end = mass.find(segf,end);
            if(end == std::string::npos) return false;
            opt_ = mass[begin];

            begin = ++end; end = mass.find(endf,end);
            y_ = std::stoi(mass.substr(begin,end));
            return true;
        }

    public:
        long long x_;
        long long y_;
        char opt_;
    };

    //    
    //    5_2_0\n\n
    class Response{
    public:
        Response(long long result = 0, int code = 0) 
            :result_(result),code_(code)
        {}
        Response(const std::string& mass) {
            Decode(mass);
        }
        ~Response() {}

        std::string Encode() {
            
            std::string ret = segf;
            ret += std::to_string(result_);
            ret += segf;
            ret += std::to_string(code_);
            ret += endf;

            int len = ret.size();
            ret = std::to_string(len) + ret;
            return ret;
        }

        bool Decode(std::string mass) {
            size_t begin = 0;
            size_t end = mass.find(segf,begin);
            if(end == std::string::npos) return false;
            int size = std::stoi(mass.substr(begin,end));
            
            begin = ++end;end = mass.find(segf,end);
            if(end == std::string::npos) return false;
            result_ = std::stoi(mass.substr(begin,end));

            begin = ++end; end = mass.find(endf,end);
            code_ = std::stoi(mass.substr(begin,end));
            return true;
        }

    public:
        long long result_;
        int code_;
    };
}