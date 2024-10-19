#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>

#include "../comm/log.hpp"
#include "../comm/util.hpp"


namespace compile_server{

    using namespace logger;
    using namespace util;

    // 提供编译服务的主机
    class Machine{
    public:
        std::string _ip;        //主机的ip地址
        int _port;              //主机提供服务的端口号
        uint64_t _load;         //主机的负载
        std::mutex* _mtx;       //std中的mutex是禁止拷贝的，使用指针
    public:
        Machine(const std::string& ip = "",int port = 0, uint64_t load = 0,std::mutex* mtx = nullptr)
            :_ip(ip),_port(port),_load(load),_mtx(mtx){}
        ~Machine(){}
    public:
        //获取负载
        uint64_t load(){
            uint64_t ret = 0;
            if(_mtx) _mtx->lock();
            ret = _load;
            if(_mtx) _mtx->unlock();
            return ret;
        }
        //增加负载
        void IncLoad(){
            if(_mtx) _mtx->lock();
            _load++;
            if(_mtx) _mtx->unlock();
        }
        //减少负载
        void DecLoad() {
            if (_mtx) _mtx->lock();
            _load--;
            if(_mtx) _mtx->unlock();
        }
        //清空负载
        void ResetLoad() {
            if(_mtx) _mtx->lock();
            _load = 0;
            if(_mtx) _mtx->unlock();
        }
    };
    //提供负载均衡的模块
    const std::string compile_server_path = "./config/compile_server.conf";
    class LoadBlance{
    private:
        //提供服务的主机
        std::vector<Machine> _machines;
        //这里使用——machines中的，每一个主机的下标，当做这些主机的标识
        //在线的主机
        std::vector<int> _online;
        //离线的主机
        std::vector<int> _offline;
        //用锁，保证LoadBlance的线程安全
        std::mutex _mtx;

    public:
        LoadBlance() {
            info(2,"LoadBlance()") << "创建LoadBlance对象" << std::endl;
            //获取主机列表
            
            if(!LoadServer()) {
                info(3,"LoadBlance()") << "加载编译服务失败，请检查配置文件" << compile_server_path << std::endl;
                exit(-4);
            }
            LOG(INFO) << "主机加载成功" << std::endl;
        }
        ~LoadBlance(){}

    public:
        /**
         * 获取最低的负载
         * @id:输出型参数，输出最低负载的主机id，也就是machines中的下标
         * @m:输出行参数，返回最低负载的主机指针
         * @returnvalue:是否找到负载最低的主机
        */
        bool SmartChoice(int* id, Machine** m) {
            //在多线程下，可能有多个线程来获取最低负载的主机，可能造成线程安全问题，
            _mtx.lock();
            int size = _online.size();
            if(size == 0) {
                _mtx.unlock();
                LOG(ERROR) << "所有主机均已离线，请查看conpile_server部署情况" << std::endl;
                return false;
            }
            int minload = _machines[_online[0]].load();
            *id = _online[0];
            *m = &_machines[_online[0]];
            for (int i = 1; i < size; i++) {
                uint64_t curload = _machines[_online[i]].load();
                if (curload < minload) {
                    minload = curload;
                    *id = _online[i];
                    *m = &_machines[_online[i]];
                }
            }
            _mtx.unlock();
            return true;
        }

        /**
         * 将主机全部上线，如果掉线的主机重合上线的话，这个时候是没有办法识别将他拿到在线列表中的，
         * 需要将所有的不在线主机全部上线，重新进行选择，
         */
        void AllOnline() {
            _mtx.lock();
            _online.insert(_online.end(), _offline.begin(), _offline.end());
            _offline.erase(_offline.begin(), _offline.end());
            LOG(INFO) << "所有的主机都上线了" << std::endl;;
            _mtx.unlock();
        }
        //对某一个主机进行离线
        void offMachine(int id){
            _mtx.lock();
            for (auto iter = _online.begin(); iter != _online.end(); iter++) {
                if (*iter == id) {
                    _machines[id].ResetLoad();
                    _online.erase(iter);
                    _offline.push_back(id);
                    break;
                }
            }
            _mtx.unlock();
        }

        //打印当前主机在线情况，
        void ShowMachines(){
            _mtx.lock();
            std::cout << "当前在线主机列表:" << std::endl;
            for(int id : _online) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            std::cout << "当前离线主机列表" << std::endl;
            for (int id : _offline) {
                std::cout << id << " " ;
            }
            std::cout << std::endl;
            _mtx.unlock();
        }


    private:
        bool LoadServer() {
            info(3,"LoadServer()") << "加载编译服务主机列表" << std::endl;
            info(4,"LoadServer()") << "读取编译服务的配置文件，获取主机信息"  << std::endl;
            std::ifstream server(compile_server_path);
            if(!server.is_open()) {
                info(4,"LoadServer()") << "打开编译服务配置文件失败" << std::endl ;
                return false;
            }
            std::string line;
            while(getline(server,line)){
                std::vector<std::string> tokens;
                StringUtil::SplitString(line,tokens,":");
                if (tokens.size() != 2) {
                    info(4,"LoadServer()") << "找到不完整的服务器信息:at" << compile_server_path << " :" << line << std::endl;
                    continue;
                }
                Machine m = {tokens[0],atoi(tokens[1].c_str()),0,new std::mutex};
                _online.push_back(_machines.size());
                _machines.push_back(m);
            }
            server.close();
            info(4,"LoadServer()") << "加载编译主机信息成功"  << std::endl;
            return true;
        }

    };
}