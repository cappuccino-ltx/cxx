#pragma once


#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace run{

    using namespace util;
    using namespace logger;

    class Runner{

    public:
        /**
         *@cpu_limit: CPU的时长，单位是秒
         *@mem_limit: 程序占用内存的大小，单位是KB
         */
        static void SetProLimit(int cpu_limit, int mem_limit) {
            //设置CPU时长
            struct rlimit r_cpu;
            r_cpu.rlim_cur = cpu_limit;
            r_cpu.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &r_cpu);
            //设置内存大小
            struct rlimit r_mem;
            r_mem.rlim_cur = mem_limit * 1024;
            r_mem.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &r_mem);
        }
        /*
            运行文件的话，需要用到的文件：
                code.exe----------可执行文件
                code.in ----------标准输入文件
                code.out----------标准输出的文件
                code.err----------标准错误的文件
        */
        static int Runing(const std::string& filename, int cpu_limit, int mem_limit) {
            int res = fork();
            if (res < 0) {
                LOG(FATAL) << "创建子进程失败" << std::endl;
                //创建进程失败
                exit(1);
            }
            else if (res == 0) {
                //子进程
                std::string _exefile = Splice::Exe(filename);
                std::string _infile = Splice::In(filename);
                std::string _outfile = Splice::Out(filename);
                std::string _errfile = Splice::Err_run(filename);

                //将标准输出和标准错误重定向到out和err中，
                umask(0);
                int _in = open(_infile.c_str(),O_CREAT | O_WRONLY, 0644);
                int _out = open(_outfile.c_str(), O_CREAT | O_WRONLY, 0644);
                int _err = open(_errfile.c_str(), O_CREAT | O_WRONLY, 0644);
                if (_in < 0 || _out < 0 || _err < 0) {
                    //打开文件失败，关闭之前打开的文件
                    LOG(FATAL) << "打开重定向文件失败" << std::endl;
                    exit(2);
                }
                dup2(_in , 0);
                dup2(_out, 1);
                dup2(_err, 2);

                //限制资源大小
                SetProLimit(cpu_limit,mem_limit);

                //进行程序替换
                execl(_exefile.c_str(),_exefile.c_str(),nullptr);
                LOG(FATAL) << "程序替换失败" << std::endl;
                exit(3);
            }
            else {
                //父进程
                int status = 0;
                int ret = waitpid(res, &status, 0);
                // if (ret > 0 && (status & 0x7f) == 0) {
                //     //子进程正常执行
                //     //退出码 (status >> 8) & 0x7f
                // }
                // else if (ret > 0){
                //     //子进程被信号中断
                //     //信号： (status & 0x7f)
                // }
                //这里只需要返回状态码
                //6号信号代表，超出内存限制，
                //24号新号代表，代码运行超时
                LOG(DEBUG) << "子进程退出状态" << (status & 0x7f) << \
                " 子进程的退出码" << ((status >> 8) & 0x7f)<< std::endl;
                return status & 0x7f;
            }
            return -1;
        }
    };
}