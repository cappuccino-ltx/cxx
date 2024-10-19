

#include <iostream>
#include <string>
#include <mysql/mysql.h>

const std::string host = "127.0.0.1";
const std::string user = "ltx";
const std::string password = "544338";
const std::string db = "test01";
const unsigned int port = 3306;

int main() {
    //获取连接的版本
    // std::cout << "mysql client version :" << mysql_get_client_info() << std::endl;
    MYSQL *my = mysql_init(nullptr);
    if (nullptr == my) {
        std::cerr << "mysql init error" <<std::endl;
        exit(1);
    }
    if (mysql_real_connect
            (my,host.c_str(),user.c_str(),password.c_str(),
            db.c_str(),port,nullptr,0) == nullptr){
        std::cerr << "mysql client error" << std::endl;
        exit(2);
    }
    mysql_set_character_set(my, "utf8");
    std::string str = "select * from book";
    //mysql_query() 函数执行成功后返回1，
    int n = mysql_query(my,str.c_str());
    if (n == 0) std::cout << "query success" << std::endl;
    else {
        std::cerr << str << " failed" << std::endl;
        exit(3);
    }

    //提取查询结果集
    MYSQL_RES *res = mysql_store_result(my);
    if (nullptr == res) {
        std::cerr << "mysql_store_result error" << std::endl;
        exit(4);
    }
    
    //获取查询返回结果集的行列
    int rows = mysql_num_rows(res);
    int fields = mysql_num_fields(res);
    //可以提取查询的列名
    MYSQL_FIELD *field = mysql_fetch_fields(res);
    for (int i = 0; i < fields; ++i) {
        std::cout << field[i].name << "\t";
    }
    std::cout << std::endl;
    //根据行列将返回结果集打印出来
    for (int i = 0; i < rows; ++i) {
        //拿到一行数据
        MYSQL_ROW row = mysql_fetch_row(res);
        for (int j = 0; j < fields; ++j) {
            std::cout << row[j] << "\t";
        }
        std::cout << std::endl;
    }
    mysql_close(my);
    return 0;

}