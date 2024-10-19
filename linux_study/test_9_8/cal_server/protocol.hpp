
#include <iostream>
#include <string>
#include <cstring>
#include <jsoncpp/json/json.h>

//#define MYSELF 0

#define SPACE " "
#define SPACE_LEN strlen(SPACE)
#define SEP "\r\n"
#define SEP_LEN strlen(SEP)


std::string Decode(std::string & buffer) {
    size_t pos = buffer.find(SEP);
    if (pos == std::string::npos){
        return "";
    }
    int len = atoi(buffer.substr(0, pos).c_str());
    int surplus = buffer.size() - pos -  2 * SEP_LEN;
    if (surplus >= len) {
        //说明有完整的报文
        buffer.erase(0,pos + SEP_LEN);
        std::string str = buffer.substr(0,len);
        buffer.erase(0,len + SEP_LEN);
        return str;
    }
    return "";
}
std::string Encode(std::string & s) {
    std::string new_package = std::to_string(s.size());
    new_package += SEP;
    new_package += s;
    new_package += SEP;
    return new_package;
}


class request{

public:
    request(){}
    request(int x,int y,char op):_x(x),_y(y),_op(op) {}
    ~request(){}

    std::string Serialize() {
#ifdef MYSELF
        std::string str;
        str += std::to_string(_x);
        str += SPACE;
        str += _op;
        str += SPACE;
        str += std::to_string(_y);
        return str;
#else
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["op"] = _op;
        Json::FastWriter writer;
        return writer.write(root);

#endif
    }

    bool Deserialize(std::string &str) {
#ifdef MYSELF
        std::size_t left =  str.find(SPACE);
        if (left == std::string::npos){
            return false;
        }
        std::size_t right = str.rfind(SPACE);
        if (right == std::string::npos){
            return false;
        }
        _x = atoi(str.substr(0,left).c_str());
        _y = atoi(str.substr(right + SPACE_LEN).c_str());
        if(left + SPACE_LEN > str.size()) {
            return false;
        }
        _op = str[left + SPACE_LEN];
        return true;
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(str,root);
        _x = root["x"].asInt();
        _y = root["y"] .asInt();
        _op = root["op"].asInt();
        return true;
#endif
    }

public:
    int _x;
    int _y;
    char _op;
};
class response{

public:
    response(){}
    response(int code, int result):_code(code),_result(result) {}
    ~response(){}

    std::string Serialize() {
#ifdef MYSELF
        std::string str;
        str += std::to_string(_code);
        str += SPACE;
        str += std::to_string(_result);
        return str;
#else
        Json::Value root;
        root["code"] = _code;
        root["result"] = _result;
        Json::FastWriter write;
        return write.write(root);

#endif
    }

    bool Deserialize(std::string &str) {
#ifdef MYSELF
        std::size_t pos =  str.find(SPACE);
        if (pos == std::string::npos){
            return false;
        }
        _code = atoi(str.substr(0,pos).c_str());
        _result = atoi(str.substr(pos + SPACE_LEN).c_str());
        return true;
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(str,root);
        _code = root["code"].asInt();
        _result = root["result"].asInt();
        return true;

#endif
    }

public:
    int _code;
    int _result;
};