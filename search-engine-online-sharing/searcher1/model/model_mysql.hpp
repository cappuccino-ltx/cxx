#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <pthread.h>
#include <mysql/mysql.h>

#include "../comm/log.hpp"



const std::string mysql_config = "./resource/mysql.config";

namespace ns_sql{

    

    class connection {
        private:
            MYSQL*          _conn; 
            std::string     _host;
            std::string     _user;
            std::string     _passwd;
            std::string     _db;
            int             _port;
            
        public:
            
            /**
             *      @host:  主机ip
             *      @user:  用户名
             *      @passwd:密码
             *      @db:    数据库
             *      @port:  端口
             */

            connection(const std::string& host,const std::string& user,const std::string& passwd,const std::string& db,int port) 
                :_conn(nullptr),_host(host),_port(port),_user(user),_passwd(passwd),_db(db)
            {
                _conn = mysql_init(nullptr);
                //连接数据库
                if(nullptr == mysql_real_connect(_conn,_host.c_str(),_user.c_str(),_passwd.c_str(),_db.c_str(),port,nullptr,0)){
                    LOG(FATAL,"数据库连接失败");
                    exit(-1);
                }
            }
            ~connection() {
                mysql_close(_conn);
            }

            /**
             *      query,执行sql语句，执行查询语句时候，将查询到的结果，按照字段的粒度放到vector中返回，
             *      @sql:       要执行的sql语句
             *      @result:    返回的结果，每个字段都被用string的方式放到了result中，
             *      @return:    查询的记录条数
             */
            int query(const std::string& sql, std::vector<std::string>* result = nullptr) {
                if(0 != mysql_query(_conn,sql.c_str())) {
                    LOG(ERROR,sql + " :执行失败");
                    std::cout << mysql_error(_conn) << std::endl;
                    return 0;
                }
                if(result == nullptr)  {
                    //LOG("INFO", sql + "执行成功");
                    return 0;
                }
                //提取结果
                MYSQL_RES *res = mysql_store_result(_conn);
                if(nullptr == res) {
                    LOG(WARNING, "未查询到结果");
                    return 0;
                }
                //std::cout << "查询成功" << std::endl;
                int rows = mysql_num_rows(res);
                int cols = mysql_num_fields(res);
                for (int i = 0; i < rows; i++) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    for (int j = 0; j < cols; j++) {
                        result->push_back(row[j]);
                    }
                }
                mysql_free_result(res);
                return rows;
            }

            MYSQL* get_mysql() {
                return _conn;
            }

            
            


            //根据doc_id获取正排索引
             bool get_forward_index(int doc_id,std::vector<std::string>* res) {
                //std::vector<std::string>* res = new std::vector<std::string>();
                std::string sql = "select doc_id,title,content,url from forward_index where doc_id = " + std::to_string(doc_id);
                int n = query(sql,res);
                if (n != 1) return false;
                return true;
            }

            //根据分词结果获取倒排索引
             int get_inverted_index(const std::string& word,std::vector<std::string>* res) {
                //std::vector<std::string>* res = new std::vector<std::string>();
                std::string sql = "select l.doc_id,l.weight from inverted_list as l join inverted_index as i \
                on l.inv_id = i.inv_id where word = '" + word + "'";
                // MYSQL_STMT* stmt = mysql_stmt_init(_conn);
                // mysql_stmt_prepare(stmt,sql.c_str(),sql.size());

                // MYSQL_BIND bind;
                // bind.buffer_type = MYSQL_TYPE_STRING;
                // bind.buffer = (void*)word.c_str();
                // bind.buffer_length = word.size();

                // if(mysql_stmt_bind_param(stmt,&bind) != 0) {
                //     LOG(ERROR,"sql参数绑定失败");
                //     return 0;
                // }
                // if(mysql_stmt_execute(stmt) != 0) {
                //     LOG(ERROR, "sql执行失败");
                //     return 0;
                // }

                int n = query(sql,res);
                return n;
            }
    };

    class mysql_pool{
        private:
            static pthread_mutex_t      _mtx;
            static pthread_cond_t       _pop_cond;
            std::queue<connection*>    _conn;
            static mysql_pool*           _pool;

            mysql_pool(int n) {
                Json::Value* info = ns_util::SourceUtil::getConnectionInfo(mysql_config);
                std::string host = (*info)["host"].asString();
                std::string user = (*info)["user"].asString();
                std::string passwd = (*info)["passwd"].asString();
                std::string db = (*info)["db"].asString();
                int port = (*info)["port"].asInt();
                
                for (int i = 0; i < n; i++) {
                    _conn.push(new connection(host,user,passwd,db,port));
                }
            }
            ~mysql_pool() {
                for (int i = 0; i < _conn.size(); i++) {
                    delete _conn.front();
                    _conn.pop();
                }
            }
        public:
            static mysql_pool* get_pool(int n = 7) {
                if(nullptr == _pool) {
                    pthread_mutex_lock(&_mtx);
                    if(nullptr == _pool) {
                        _pool = new mysql_pool(n);
                    }
                    pthread_mutex_unlock(&_mtx);
                }
                return _pool;
            }

            connection* get_connection(){
                connection* my = nullptr;
                pthread_mutex_lock(&_mtx);
                if(_conn.size() == 0) {
                    pthread_cond_wait(&_pop_cond,&_mtx);
                }
                my = _conn.front();
                _conn.pop();
                pthread_mutex_unlock(&_mtx);
                return my;
            }
            void destroy(connection* my){
                pthread_mutex_lock(&_mtx);
                _conn.push(my);
                pthread_cond_signal(&_pop_cond);
                pthread_mutex_unlock(&_mtx);
            }

    };
    pthread_mutex_t mysql_pool::_mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mysql_pool::_pop_cond = PTHREAD_COND_INITIALIZER;
    mysql_pool* mysql_pool::_pool  = nullptr;
    
}