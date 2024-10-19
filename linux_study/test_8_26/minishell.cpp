
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;


#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
#define NONE_REDIR 0;

int redir_status = NONE_REDIR;

char* CheckRedir(char* start){
    assert(start);
    char* end = start + strlen(start) - 1;

    while (end >= start) {
        if (*end == '>') {
            if (*(end - 1) == '>'){
                //追加重定向
                redir_status = APPEND_REDIR;
                *(end - 1) = '\0';
                end++;   
                break;
            }
            //输出重定向
            redir_status = OUTPUT_REDIR;
            *end = '\0';
            end++;    
            break;
        }
        else if(*end == '<'){
            //输入重定向
            redir_status = INPUT_REDIR;
            *end = '\0';
            end++;    
            break;
        }
        else{
            end--;
        }
    }
    if (end >= start) {
        return end;
    }
    return NULL;
}


//利用程序进程替换实现一个迷你shell
//思想就是，输入一个指令，以字符串的方式做解析，然后调用相应的指令文件，
int main() {
    int index = 0;
    char *args[64] = { 0 };//用来存储输入的指令
    char instruct[1024] = {'\0'};
    while (1) {
        //清空命令接收数组
        memset(instruct, '\0', sizeof(instruct) - 1);
        cout << "[root@我的主机 ~]#";

        //刷新输出缓冲区
        fflush(stdout);
        //获取用户输入的指令
        cin.getline(instruct, sizeof(instruct),'\n');
        instruct[strlen(instruct)] = '\0';

        //判断指令是否有重定向
        char *sep = CheckRedir(instruct);

        //分割指令
        args[0] = strtok(instruct, " ");
        index = 1;
        if (strcmp(args[0],"ls") == 0) {
            args[index++] = "--color=auto";
        }
        if (strcmp(args[0], "ll") == 0){
            args[0] = "ls";
            args[index++] = "-l";
            args[index++] = "--color=auto";
        }
        
        while (args[index++] = strtok(NULL, " "));

        //如果是cd指令，那就执行切换目录的函数
        if (strcmp(args[0], "cd") == 0){
            if (args[1] != NULL) chdir(args[1]);
            continue;
        }

        //创建子进程
        size_t id = fork();
        if (id == 0) {
            //子进程
            int fd = -1;
            if (sep != NULL) {
                switch(redir_status){
                    case INPUT_REDIR :
                        fd = open(sep,O_RDONLY);
                        dup2(fd,0);
                        break;
                    case OUTPUT_REDIR :
                        fd = open(sep, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        dup2(fd,1);
                        break;
                    case APPEND_REDIR :
                        fd = open(sep, O_WRONLY | O_CREAT | O_APPEND, 0666);
                        dup2(fd,1);
                        break;
                    default:
                        cout << "bud?" << endl;
                        break;
                }
            }

            execvp(args[0],args);
            exit(1);
        }
        //父进程
        int status = 0;
        size_t ret = waitpid(-1, &status, 0);
        if ( WIFEXITED(status) && ret==id ) {
            //子进程成功执行:
             cout << "return code:" << WEXITSTATUS(status) << endl;
        }
        else {
            cout << "wait process failed!"<< endl;
            return 1;
        }
    }

    return 0;
}
