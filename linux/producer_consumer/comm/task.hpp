#pragma once

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

namespace taskspace{
    class Task{
    public:
        Task(){
            struct timeval t = { 0 };
            gettimeofday(&t,nullptr);
            _pro_time = (t.tv_sec + pthread_self()) % 5 + 1;
            _run_time = (t.tv_usec + pthread_self()) % 5 + 1;
        }
        void pro(){
            std::cout << "任务生产 ：" << _pro_time << std::endl;
            sleep(_pro_time);
        }
        void run(){
            std::cout << "任务运行 ：" << _run_time << std::endl;
            sleep(_run_time);
        }
    private:
        int _pro_time;      //生产所花费的时间
        int _run_time;      //运行所花费的时间
    };
}