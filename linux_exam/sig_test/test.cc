

#include <iostream>
#include <signal.h>


void headler(int sig_num) {
    std::cout << "收到了信号：" << sig_num << std::endl;
    exit(sig_num);
}
//注册所有信号
void regis() {
//注册所有信号的动作
    for (int i = 1; i < 32; i++) {
        struct sigaction act;
        act.sa_handler = headler;
        sigaction(i,&act,nullptr);
    }
}

//a.i空指针异常-----------收到了11号信号
void test01() {
    int* ptr = nullptr;
    *ptr = 10;
}

//a.ii
//列举三种导致进程崩溃的原因：
/**
    1,除0错误-------8号信号
    2.ctrl+c,从键盘生成硬中断信号，系统到当前进程的软中断信号是2号
    3.ctrl+\,同上，产生三号信号
 */
void test02() {
    while(1);//ctrl+c,2号信号
    //int a = 10 / 0;//除0错误
}

//544338

//a.iii 
/**
    刚开始没有产生coredump文件，原因是
        没有开启核心转储，用limit -c查看是否开启核心转储，输出为0的话，就没有开启，
        再次使用limit -c size,设置核心转储文件大小，

        没有使用 -g选项编译源文件

        对信号的行为进行了注册，这样也是无法生成coredump文件的
 */

int main() {
    //regis();
    test01();
    //test02();
    return 0;
}



