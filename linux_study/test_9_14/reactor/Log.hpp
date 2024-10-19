


#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <string>

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

#define LOGFILE "log.log"

const char *gLevelMap[] = {
    "DEBUG",
    "NORMAL",
    "WARNING",
    "ERROR",
    "FATAL"
};

void logmessage(int level, const char *format, ...) {
// #ifndef DEBUG_SHOW
//     if (level == DEBUG) return ;
// #endif
    //标准部分
    char stdbuffer[1024];
    time_t timestamp = time(nullptr);
    char date_time[32];
    strftime(date_time,sizeof(date_time), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
    snprintf(stdbuffer,sizeof(stdbuffer),"[%s] [%s]",gLevelMap[level],date_time);

    //自定义部分
    char logbuffer[1024];
    va_list args;
    va_start(args,format);
    vsnprintf(logbuffer, sizeof(logbuffer), format, args);
    va_end(args);

    //输出到控制台
    std::cout << stdbuffer << logbuffer << std::endl;
    //输出到文件
    //std::ofstream out(LOGFILE);
    //out << stdbuffer << logbuffer << std::endl;
    //out.close()
}

