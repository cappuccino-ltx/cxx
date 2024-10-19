
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "compile.hpp"
#include "runner.hpp"

namespace compile_run{

    using namespace util;
    using namespace logger;
    using namespace compile;
    using namespace run;
    class Compile_Run{
    public:
        static void RemoveFile(const std::string& filename) {
            //源文件
            std::string _src = Splice::Src(filename);
            if (FileUtil::FileExists(Splice::Src(filename))) 
                unlink(_src.c_str());
            //编译错误的文件
            std::string _err_com = Splice::Err_com(filename);
            if (FileUtil::FileExists(_err_com)) 
                unlink(_err_com.c_str());
            // 可执行文件
            std::string _exe = Splice::Exe(filename);
            if (FileUtil::FileExists(_exe))
                unlink(_exe.c_str());
            // 运行错误文件
            std::string _err_run = Splice::Err_run(filename);
            if (FileUtil::FileExists(_err_run))
                unlink(_err_run.c_str());
            // 输入文件
            std::string _in = Splice::In(filename);
            if (FileUtil::FileExists(_in))
                unlink(_in.c_str());
            // 输出文件
            std::string _out = Splice::Out(filename);
            if (FileUtil::FileExists(_out))
                unlink(_out.c_str());
        }
        static std::string StatusToDesc(int status, const std::string filename) {
            std::string ret;
            switch(status) {
                case 0: {
                    ret = "代码运行成功";
                    break;
                }
                case -1: {
                    ret = "提交的代码为空";
                    break;
                }
                case -2: {
                    ret = "未知错误" ;
                    break;
                }
                case -3: {
                    //"编译出错";
                    FileUtil::ReadFile(Splice::Err_com(filename),&ret,true);
                    break;
                }
                //     运行时出现段错误，11号信号
                //     运行时超出内存限制，6号信号
                //     运行时超出时间限制，24号信号
                case 6: {
                    ret = "超出内存限制" ;
                    break;
                }
                case 24: {
                    ret = "超出时间限制" ;
                    break;
                }
                case 11: {
                    ret = "段错误";
                    break;
                }
                case 8: {
                    ret = "浮点数错误";
                    break;
                }
                default: {
                    ret = "未知错误" + std::to_string(status);
                    break;
                }
            }
            return ret;
        }
        
        /***
            @in_json:输入的json串中应该包含的信息：
                {
                    "code" : "",        //代码
                    "input" : "",       //输入
                    "cpu_limit" : "",   //运行时间限制
                    "mem_limit" : ""    //运行空间限制
                }
            @out_json:输出的json串中应该包含的信息：
                {
                    "status" : "",      //状态码
                    "reason" : "",      //请求的结果
                    //选填内容
                    "stdout" : "",      //程序的运行结果
                    "stderr" : ""       //程序的错误结果
                }
        **/
        static void Start(const std::string &in_json, std::string* out_json) {
            //在这里通过传进来的json，解析出代码以及代码的输入，和资源的限制，
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json,in_value);

            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            //goto语句中不允许定义变量，所以将变量都放到goto之前定义

            //设置一个状态标志位，方便最后进行差错处理
            int status_code = 0;
            //定义返回的json对象
            Json::Value out_value;
            //定义程序运行的结果
            int run_result = 0;
            std::string filename;

            // 如果json串中的长度为0，那么就直接返回，状态：提交的代码为空
            if (code.size() == 0) {
                status_code = -1;//-1代表代码为空
                LOG(INFO) << "提交的代码为空" << std::endl;
                goto END;
            }

            // 生成代码的源文件，和编译信息文件，    
            //     需要生成具有唯一标识的文件名，进行路径拼接，写入信息，
            filename = FileUtil::UniqFileName();

            //写入源文件，生成临时文件
            if(!FileUtil::WriteFile(Splice::Src(filename), code)\
                && !FileUtil::WriteFile(Splice::In(filename),input) \
            ) {
                status_code= -2;//内部错误,位置错误；
                LOG(ERROR) << "生成源文件,以及输入文件失败" << std::endl;
                goto END;
            }
            
            //     编译错误，就不用进行执行代码的操作了
            if (!Compiler::Compile(filename)) {
                status_code= -3;
                LOG(INFO) << "编译失败";
                goto END;
            }
            // 如果代码的编译没有出现错误的话，需要我们对运行的结果进行差错处理，进行返回运行状态以及状态描述，
            run_result = Runner::Runing(filename,cpu_limit, mem_limit);
            if (run_result < 0) {
                status_code = -2;
                LOG(ERROR) << "运行结果返回值小于0：" << run_result <<std::endl;
                goto END;
            }
            else if (run_result > 0) {
                status_code = run_result;
                LOG(INFO) << "程序被信号杀死：" << run_result << std::endl;
                goto END; 
            }
            else if (run_result == 0) {
                status_code = 0;
                LOG(INFO) << "运行成功" << std::endl;                
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = StatusToDesc(status_code,filename);

            if(status_code == 0) {
                //编译运行全部成功
                std::string _out;
                FileUtil::ReadFile(Splice::Out(filename), &_out,true);
                out_value["stdout"] = _out;
                std::string _err;
                FileUtil::ReadFile(Splice::Err_run(filename), &_err,true);
                out_value["stderr"] = _err;
            }
            JJson::StyledWriter writer;                                                                                                                                                                       
  184             *out_json = writer.write(out_value);son::StyledWriter writer;
            *out_json = writer.write(out_value);

            //删除文件
            RemoveFile(filename);

        }
    };
}
