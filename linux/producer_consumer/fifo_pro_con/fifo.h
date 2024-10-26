

#include "thread_pool_sem.hpp"
#include "../comm/task.hpp"

//mkfifo函数使用的头文件
#include <sys/types.h>
#include <sys/stat.h>

const std::string filename = "myfifo";
