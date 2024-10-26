#pragma once

#include <iostream>
#include <string>

#include <json/json.h>

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
#ifdef MYSELF
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
#else
            Json::Value root;
            root["x"] = Json::Value::Int64(x_);
            root["y"] = Json::Value::Int64(y_);
            root["opt"] = opt_;

            Json::FastWriter writer;
            return writer.write(root);
#endif      
        }

        bool Decode(std::string mass) {
#ifdef MYSELF
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
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(mass,root);

            x_ = root["x"].asInt64();
            y_ = root["y"].asInt64();
            opt_ = root["opt"].asInt();
            return true;
#endif
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
#ifdef MYSELF
            std::string ret = segf;
            ret += std::to_string(result_);
            ret += segf;
            ret += std::to_string(code_);
            ret += endf;

            int len = ret.size();
            ret = std::to_string(len) + ret;
            return ret;
#else 
            Json::Value root;
            root["result"] = Json::Value::Int64(result_);
            root["code"] = code_;

            Json::FastWriter writer;
            return writer.write(root);
#endif
        }

        bool Decode(std::string mass) {
#ifdef MYSELF
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
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(mass,root);
            result_ = root["result"].asInt64();
            code_ = root["code"].asInt();
            return true;
#endif
        }

    public:
        long long result_;
        int code_;
    };
}