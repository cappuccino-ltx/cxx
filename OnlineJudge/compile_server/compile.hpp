#pragma once


#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace compile{

    using namespace util;
    using namespace logger;


    class Compiler{
    public:
        Compiler() {}
        ~Compiler() {}
        //通过一个文件名对其进行编译，
        /*
            对一个文件进行编译之后，应该有一下几个文件(文件名：code)
            源文件-----------------code.cpp(code.cc)
            生成的可执行文件--------code.exe
            编译出错的错误信息------code.err
        */
        static bool Compile(const std::string& filename) {
            pid_t res = fork();
            if (res < 0) {
                //创建子进程失败
                LOG(FATAL) << "创建子进程失败" << std::endl;
                exit(1);
            }           
            else if(res == 0) {
                //子进程，在这里进行文件的编译
                
                //如果编译出现错误，我们期望的是将编译的报错输出到.err这个文件中
                umask(0);//将文件创建权限掩码设置为0，保证创建的文件权限是我们指定的那样
                int _err = open(Splice::Err_com(filename).c_str(),O_CREAT | O_WRONLY, 0644);
                if (_err < 0) {
                    //创建文件失败
                    LOG(FATAL) << "创建重定向文件失败" << std::endl;
                    exit(2);
                }
                dup2(_err, 2);

                //进行程序的替换---因为这里使用的是execlp,所以只需要指定程序名，
                execlp("g++","g++","-o",Splice::Exe(filename).c_str(),\
                 Splice::Src(filename).c_str(),nullptr);
                
                LOG(FATAL) << "程序替换失败" << std::endl;
                exit(0);
            } 
            else {
                //父进程
                int status = 0;//获取子进程的状态
                int ret = waitpid(res, &status, 0);
                if(ret > 0 && (status & 0x7f) == 0) {
                    //程序正常运行，
                    if(FileUtil::FileExists(Splice::Exe(filename))) {
                        //正常生成可执行文件
                        LOG(INFO) << "正常编译程序" << std::endl;
                        return true;
                    }
                }
                else {
                    //程序被信号杀掉
                    //(status >> 7) & 1 是否开启核心转储
                    //(status & 0x7f) 检查被几号信号杀死

                }
            }
            //编译失败，没有形成可执行程序
            LOG(FATAL) << "编译失败" << std::endl;
            return false;
        }
    };

}