#pragma once

#include <iostream>
#include <string>

using namespace std::literals;

namespace ns_protocol{

    const std::string end = "\n\n";
    const std::string seg = " ";

    class Request{
    public:
        Request(long long x, long long y,const std::string& opt) 
            :x_(x),y_(y),opt_(opt)
        {}
        ~Request() {}

        std::string Encode() {
            //    5\31_+_1\3\n\n
            std::string ret = 
        }

        void Decode(std::string) {}

    public:
        long long x_;
        long long y_;
        std::string opt_;
    };

    class Response{
    public:
        Response(long long x, int opt) 
            :result_(x),code_(opt)
        {}
        ~Response() {}

    public:
        long long result_;
        int code_;
    };
}