#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <queue>


#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "../model/model_mysql.hpp"
#include "thread_pool.hpp"




namespace ns_index{

    using namespace ns_sql;
    using namespace ns_thread;

    struct DocInfo{
        bool load;
        std::string title;   //文档的标题
        std::string content; //文档对应的去标签之后的内容
        std::string url;     //官网文档url
        uint64_t doc_id;          //文档的ID，暂时先不做过多理解
    };

    struct InvertedElem{
        uint64_t doc_id;
        std::string word;
        int weight;
        InvertedElem():weight(0){}
    };

    //倒排拉链
    typedef std::vector<InvertedElem> InvertedList;

    class Index{
        private:
            //正排索引的数据结构用数组，数组的下标天然是文档的ID
            std::vector<DocInfo> forward_index; //正排索引
            //倒排索引一定是一个关键字和一组(个)InvertedElem对应[关键字和倒排拉链的映射关系]
            std::unordered_map<std::string, InvertedList> inverted_index;
            thread_pool* _thread_pool;
            mysql_pool* _mysql_pool;
            static Index* instance;
            static std::mutex mtx;

        private:
            Index(){
                _mysql_pool = mysql_pool::get_pool(7);
                //_thread_pool = thread_pool::get_pool(5);
                // connection* my = _mysql_pool->get_connection();
                
                // //先查询，有多少条正排索引，然后再分页查询，
                // std::string sql = "select count(*) from forward_index";
                // std::vector<std::string> res;
                // my->query(sql,&res);
                // // 正排总条数
                // int for_size = atoi(res[0].c_str());
                // forward_index.resize(for_size + 1);
                // _mysql_pool->destroy(my);
                
            } //但是一定要有函数体，不能delete
            Index(const Index&) = delete;
            Index& operator=(const Index&) = delete;

            
        public:
            ~Index(){}
        public:
            static Index* GetInstance()
            {
                if(nullptr == instance){
                    mtx.lock();
                    if(nullptr == instance){
                        instance = new Index();
                    }
                    mtx.unlock();
                }
                return instance;
            }

            /***
             * 
             * 
             * 
             * 
             * 
             * 
             * 
             *      索引的查找
             * 
             *      从数据库中查找
             * 
             * 
             * 
             */


            //根据doc_id找到找到文档内容

        private:
            /****
             *      因为正排索引，一个一个查询的话，是很耗费时间的，所以用一个内存池对结果进行查询
             *  */        
            std::queue<int> tasks;
            pthread_mutex_t get_forward_mutex = PTHREAD_MUTEX_INITIALIZER;
            pthread_cond_t get_forward_cond = PTHREAD_COND_INITIALIZER;
            struct Data {
                std::queue<int>* _tasks;
                std::vector<DocInfo>* _forward_index;
                pthread_mutex_t* _mtx;
                pthread_cond_t* _cond;
                Data(std::queue<int>* tasks,std::vector<DocInfo>* forward_index,pthread_mutex_t* mtx,pthread_cond_t* cond) 
                    :_tasks(tasks),_forward_index(forward_index),_mtx(mtx),_cond(cond)
                {}
            };
            
            static void* _GetForwardIndex(void* args) {
                Data* data = (Data*)args;
                std::queue<int>* tasks = data->_tasks;
                std::vector<DocInfo>* forward_index = data->_forward_index;
                pthread_mutex_t* mtx = data->_mtx;
                pthread_cond_t* cond = data->_cond;
                delete data;
                connection* my = mysql_pool::get_pool()->get_connection();
                while(true) {
                    pthread_mutex_lock(mtx);
                    while(tasks->size() == 0) {
                        pthread_cond_wait(cond,mtx);
                    }
                    int doc_id = tasks->front();
                    tasks->pop();
                    pthread_mutex_unlock(mtx);
                    //拿到doc_id之后，进行查询
                    if ((*forward_index)[doc_id].load == false) {
                        // 走到这里，就说明索引并未被加载，
                        
                        std::vector<std::string> res;
                        if(my->get_forward_index(doc_id,&res) == false) {
                            return nullptr;
                        }
                        (*forward_index)[doc_id].load = true;

                        //查询出来的结果是按照 doc_id,title,content,url的顺序排列的
                        (*forward_index)[doc_id].doc_id = atoi(res[0].c_str());
                        (*forward_index)[doc_id].title = res[1];
                        (*forward_index)[doc_id].content = res[2];
                        (*forward_index)[doc_id].url = res[3];
                    }
                }
                mysql_pool::get_pool()->destroy(my);
                return nullptr;
            }

        public:



            DocInfo *GetForwardIndex(uint64_t doc_id)
            {
                // connection* my = _mysql_pool->get_connection();
                if(doc_id >= forward_index.size()){
                    std::cerr << "doc_id out range, error!" << std::endl;
                    return nullptr;
                }
                
                return &forward_index[doc_id];
            }

            //根据关键字string，获得倒排拉链
            InvertedList *GetInvertedList(const std::string &word)
            {
                auto iter = inverted_index.find(word);
                if(iter == inverted_index.end()){
                    //程序走到这里，就说明没有在倒排拉链中找到当前内容，需要我们去从数据库中查找，
                    
                    connection* my = _mysql_pool->get_connection();
                    std::vector<std::string> res;
                    int size = my->get_inverted_index(word,&res);
                    if(size == 0) {
                        return nullptr;
                    }
                    _mysql_pool->destroy(my);
                    std::vector<std::string>::iterator it = res.begin();
                    mtx.lock();
                    InvertedList &inverted_list = inverted_index[word];
                    for (int i = 0; i < size && it != res.end(); i++) {
                        InvertedElem item;
                        item.doc_id = atoi((*it++).c_str());
                        item.word = word;
                        item.weight = atoi((*it++).c_str()); //相关性
                        

                        /*
                        添加到任务队列中，让线程池去查找，
                        */
                        // pthread_mutex_lock(&get_forward_mutex);
                        // tasks.push(item.doc_id);
                        // pthread_mutex_unlock(&get_forward_mutex);

                        //添加到倒排中
                        inverted_list.push_back(std::move(item));

                    }
                    mtx.unlock();
                    
                }
                iter = inverted_index.find(word);
                return &(iter->second);
            }


            /**********
             * 
             * 
             * 
             * 
             * 
             * 
             * 
             *      索引的构建
             * 
             *      从文件到内存，索引的构建，
             * 
             * 
             * 
             * 
             */

            //从文件构建正排索引
            bool _BuildIndex(const std::string &input) //parse处理完毕的数据交给我
            {
                std::ifstream in(input, std::ios::in | std::ios::binary);
                if(!in.is_open()){
                    std::cerr << "sorry, " << input << " open error" << std::endl;
                    return false;
                }

                std::string line;
                int count = 0;
                while(std::getline(in, line)){
                    DocInfo * doc = BuildForwardIndex(line);
                    if(nullptr == doc){
                        std::cerr << "build " << line << " error" << std::endl; //for deubg
                        continue;
                    }
                    count++;
                    
                    if(count % 1000 == 0){
                        LOG(NORMAL, "当前的已经建立的索引文档: " + std::to_string(count));
                    }
                    
                }
                return true;
            }

            //从文件构建正牌和倒排索引
            //根据去标签，格式化之后的文档，构建正排和倒排索引
            //data/raw_html/raw.txt
            bool BuildIndex(const std::string &input) //parse处理完毕的数据交给我
            {
                std::ifstream in(input, std::ios::in | std::ios::binary);
                if(!in.is_open()){
                    std::cerr << "sorry, " << input << " open error" << std::endl;
                    return false;
                }

                std::string line;
                int count = 0;
                while(std::getline(in, line)){
                    DocInfo * doc = _BuildForwardIndex(line,count + 1);
                    if(nullptr == doc){
                        std::cerr << "build " << line << " error" << std::endl; //for deubg
                        continue;
                    }

                    BuildInvertedIndex(*doc);
                    count++;
                    
                    if(count % 1000 == 0){
                        LOG(NORMAL, "当前的已经建立的索引文档: " + std::to_string(count));
                    }
                    
                }
                return true;
            }

            
        private:
            DocInfo *_BuildForwardIndex(const std::string &line,int id)
            {
                //1. 解析line，字符串切分
                //line -> 3 string, title, content, url
                std::vector<std::string> results;
                const std::string sep = "\3";   //行内分隔符
                ns_util::StringUtil::Split(line, &results, sep);
                if(results.size() != 3){
                    return nullptr;
                }
                //2. 字符串进行填充到DocIinfo
                forward_index[id].title = results[0]; //title
                forward_index[id].content = results[1]; //content
                forward_index[id].url = results[2];   ///url
                forward_index[id].doc_id = id; //先进行保存id，在插入，对应的id就是当前doc在vector中的下标!
                forward_index[id].load = true;
                
                return &forward_index.back();
            }

            DocInfo *BuildForwardIndex(const std::string &line)
            {
                //1. 解析line，字符串切分
                //line -> 3 string, title, content, url
                std::vector<std::string> results;
                const std::string sep = "\3";   //行内分隔符
                ns_util::StringUtil::Split(line, &results, sep);
                if(results.size() != 3){
                    return nullptr;
                }
                //2. 字符串进行填充到DocIinfo
                DocInfo doc;
                doc.title = results[0]; //title
                doc.content = results[1]; //content
                doc.url = results[2];   ///url
                doc.doc_id = forward_index.size() + 1; //先进行保存id，在插入，对应的id就是当前doc在vector中的下标!
                doc.load = true;
                //3. 插入到正排索引的vector
                //mtx.lock();
                forward_index.push_back(std::move(doc)); //doc,html文件内容
                //forward_index[doc.doc_id] = std::move(doc);
                //mtx.unlock();
                return &forward_index.back();
            }

            bool BuildInvertedIndex(const DocInfo &doc)
            {
                //DocInfo{title, content, url, doc_id}
                //word -> 倒排拉链
                struct word_cnt{
                    int title_cnt;
                    int content_cnt;

                    word_cnt():title_cnt(0), content_cnt(0){}
                };
                std::unordered_map<std::string, word_cnt> word_map; //用来暂存词频的映射表

                //对标题进行分词
                std::vector<std::string> title_words;
                ns_util::JiebaUtil::CutString(doc.title, &title_words);

                //对标题进行词频统计
                for(std::string s : title_words){
                    boost::to_lower(s); //需要统一转化成为小写
                    word_map[s].title_cnt++; //如果存在就获取，如果不存在就新建
                }

                //对文档内容进行分词
                std::vector<std::string> content_words;
                ns_util::JiebaUtil::CutString(doc.content, &content_words);

                //对内容进行词频统计
                for(std::string s : content_words){
                    boost::to_lower(s);
                    word_map[s].content_cnt++;
                }

#define X 10
#define Y 1
                for(auto &word_pair : word_map){
                    InvertedElem item;
                    item.doc_id = doc.doc_id;
                    item.word = word_pair.first;
                    item.weight = X*word_pair.second.title_cnt + Y*word_pair.second.content_cnt; //相关性
                    InvertedList &inverted_list = inverted_index[word_pair.first];
                    inverted_list.push_back(std::move(item));
                }

                return true;
            }

            struct threaddata{
                int size;      //总的条数，
                int *cur;       //当前的需要获取的下标
                std::mutex *mtx;
                std::vector<DocInfo> *forward_index;
                std::unordered_map<std::string, InvertedList> *inverted_index;
                threaddata(int _size,int *_cur, std::mutex *_mtx,std::vector<DocInfo> *_forward_index,std::unordered_map<std::string, InvertedList> *_inverted_index)
                    :size(_size),cur(_cur),mtx(_mtx),forward_index(_forward_index),inverted_index(_inverted_index)
                 {}
            };














            /*****
             * 
             * 
             * 
             * 
             *      通过mysql一次性构建索引到内存，比较耗费时间
             * 
             * 
             * 
             * 
             * 
             */





            static void* _load_forword(void* args) {
                struct threaddata* data = (struct threaddata*)args;
                int size = data->size;      
                int *cur = data->cur;       
                std::mutex *mtx = data->mtx;
                std::vector<DocInfo> *forward_index = data->forward_index;
                delete data;
                //获取数据库的连接
                connection* my = mysql_pool::get_pool()->get_connection();
                
                //获取总条数之后，进行分页查询，每次查询30条
                
                std::vector<std::string> res; // 结果集
                int begin = 0;
                while(begin < size) {
                    //获取需要从数据库读取的下标
                    mtx->lock();
                    begin = *cur;
                    *cur += 20;
                    mtx->unlock();

                    if(begin % 100 == 0)
                        std::cout << "正排索引建立进度---->>>" << begin << "/" << size << std::endl;
                    
                    res.clear();//对结果集进行清空
                    std::string sql = std::string("select doc_id,title,content,url \
                        from forward_index order by doc_id limit ") \
                        += std::to_string(begin) += ",20";
                    int n = my->query(sql,&res);
                    std::vector<std::string>::iterator it = res.begin();
                    // 对结果进行解析，
                    for (int i = 0; i < n && it != res.end(); i++) {
                        DocInfo doc;
                        doc.doc_id = atoi((*it++).c_str()); 
                        doc.title = *it++; //title
                        doc.content = *it++; //content
                        doc.url = *it++;   ///url
                        
                        //插入到正排索引的vector
                        //对于正排索引来说，提前开好了空间，所以只需要往对应的下标插入就可以了，不用加锁
                        (*forward_index)[doc.doc_id] = std::move(doc); //doc,html文件内容
                        
                    }
                    
                }
                mysql_pool::get_pool()->destroy(my);
                return nullptr;
                
            }

            static void* _load_inverted(void* args) {
                struct threaddata* data = (struct threaddata*)args;
                int size = data->size;      
                int *cur = data->cur;       
                std::mutex *mtx = data->mtx;
                std::unordered_map<std::string, InvertedList> *inverted_index = data->inverted_index;
                delete data;
                //获取数据库的连接
                connection* my = mysql_pool::get_pool()->get_connection();
                
                //获取总条数之后，进行分页查询，每次查询30条
                
                std::vector<std::string> res; // 结果集
                int begin = 0;
                while (begin < size) {
                    //获取需要从数据库读取的下标
                    mtx->lock();
                    begin = *cur;
                    *cur += 100;
                    mtx->unlock();

                    if(begin % 1000 == 0)
                        std::cout << "倒排索引建立进度---->>>" << begin << "/" << size << std::endl;

                    res.clear();//对结果集进行清空
                    std::string sql = std::string("select idx.inv_id,idx.word,list.doc_id,list.weight \
                        from inverted_list as list \
                        join inverted_index as idx on idx.inv_id = list.inv_id \
                        order by list.inv_id limit ")+std::to_string(begin)+",100";
                    int n = my->query(sql,&res);
                    std::vector<std::string>::iterator it = res.begin();
                    // 对结果集进行解析，
                    for(int i = 0; i < n && it != res.end(); i++) {
                        struct InvertedElem node;
                        it++;
                        node.word = *it++;
                        node.doc_id = atoi((*it++).c_str());
                        node.weight = atoi((*it++).c_str());
                        mtx->lock();
                        InvertedList& list = (*inverted_index)[node.word];
                        list.push_back(node);
                        mtx->unlock();
                    }
                    
                }
                mysql_pool::get_pool()->destroy(my);
                return nullptr;
            }

            static void* start(void* args) {
                struct threaddata* data = (struct threaddata*)args;
                std::vector<DocInfo> *forward_index = data->forward_index;
                std::unordered_map<std::string, InvertedList> *inverted_index = data->inverted_index;
                delete data;
                //获取数据库连接
                connection* my = mysql_pool::get_pool()->get_connection();
                
                //先查询，有多少条正排索引，然后再分页查询，
                std::string sql = "select count(*) from forward_index";
                std::vector<std::string> res;
                my->query(sql,&res);
                // 正排总条数
                int for_size = atoi(res[0].c_str());
                forward_index->resize(for_size + 1);

                res.clear();
                sql = "select count(*) from inverted_list";
                my->query(sql,&res);
                // 倒排总条数
                int inv_size = atoi(res[0].c_str());

                //创建线程内控制同步的数据
                int for_cur = 0;
                int inv_cur = 0;
                std::mutex mtx1;
                std::mutex mtx2;

                //线程数量
                const int for_thread_num = 10;
                const int inv_thread_num = 20;

                std::vector<pthread_t> forward(for_thread_num);
                std::vector<pthread_t> inverted(inv_thread_num);

                //获取正排和倒排的总条数之后，创建线程池进行加载，
                std::cout << "--------------索引开始建立-------------" << std::endl;
                for (int i = 0; i < for_thread_num; i++) {
                    threaddata* data = new threaddata(for_size,&for_cur,&mtx1,forward_index,inverted_index);
                    pthread_create(&forward[i],nullptr,_load_forword,data);
                }
                for (int i = 0; i < inv_thread_num; i++) {
                    threaddata* data = new threaddata(inv_size,&inv_cur,&mtx2,forward_index,inverted_index);
                    pthread_create(&inverted[i],nullptr,_load_inverted,data);
                }
                for (int i = 0; i < for_thread_num;i ++) {
                    pthread_join(forward[i],nullptr);
                }
                for (int i = 0; i < inv_thread_num;i ++) {
                    pthread_join(inverted[i],nullptr);
                }

                std::cout << "--------------索引建立完成-------------" << std::endl;
                mysql_pool::get_pool()->destroy(my);
                return nullptr;
            }

            

        public:
                /***
                 * 这个函数的的功能是创建一个线程，这个两个线程是用来从数据库加载正排和倒排索引的，
                 * 创建完两个线程之后，就可以退出了，优先处理用户的请求，
                 */
            void LoadIndex() {
                pthread_t id = 0;
                threaddata* data = new threaddata(0,nullptr,nullptr,&forward_index,&inverted_index);
                pthread_create(&id,nullptr,start,data);
                pthread_detach(id);//将线程分离
            }

            void test_inverted_load() {     
                
                //获取数据库的连接
                connection* my = mysql_pool::get_pool()->get_connection();
                std::vector<std::string> res; // 结果集
                //获取总条数之后，进行分页查询，每次查询30条
                std::string sql = "select count(*) from inverted_list";
                my->query(sql,&res);
                // 倒排总条数
                int size = atoi(res[0].c_str());
                
                
                int begin = 0;
                while (begin < size) {
                    //获取需要从数据库读取的下标

                    if(begin % 1000 == 0)
                        std::cout << "倒排索引建立进度---->>>" << begin << "/" << size << std::endl;

                    res.clear();//对结果集进行清空
                    std::string sql = std::string("select idx.inv_id,idx.word,list.doc_id,list.weight \
                        from inverted_list as list \
                        join inverted_index as idx on idx.inv_id = list.inv_id \
                        order by list.inv_id limit ")+std::to_string(begin)+",100";
                    int n = my->query(sql,&res);
                    std::vector<std::string>::iterator it = res.begin();
                    // 对结果集进行解析，
                    for(int i = 0; i < n && it != res.end(); i++) {
                        struct InvertedElem node;
                        it++;
                        node.word = *it++;
                        node.doc_id = atoi((*it++).c_str());
                        node.weight = atoi((*it++).c_str());
                        InvertedList& list = inverted_index[node.word];
                        list.push_back(node);
                    }
                    begin+= 100;
                }
            }





            /***
             * 
             *      将内存中的索引写入到数据库中，比较耗费时间，
             *      
             *      正排索引------8000
             *      
             *      倒排分词------23000
             * 
             *      倒排拉链------600000
             * 
             * 
             *      耗时--12小时左右
             */



        
            bool WriteMysql() {
                //获取数据库连接
                Json::Value* info = ns_util::SourceUtil::getConnectionInfo(mysql_config);
                std::string host = (*info)["host"].asString();
                std::string user = (*info)["user"].asString();
                std::string passwd = (*info)["passwd"].asString();
                std::string db = (*info)["db"].asString();
                int port = (*info)["port"].asInt();
                
                connection my(host,user,passwd,db,port);
                
                
                //插入正排索引
                LOG(INFO, "开始往数据库中插入正排索引");
                int count = 0;
                for (DocInfo& d : forward_index) {
                    count++;
                    if (count % 100 == 0)
                        std::cout << "正排索引插入进度: " << count << std::endl;
                    //这里需要注意，因为文章中的内容有单引号和双引号，所以，会导致插入动作的失败，
                    //所以需要我们对参数进行预处理，通过bind的方式将数据绑定到sql中，再去执行，
                    const char* sql = "insert into forward_index (title,content,url) values(?,?,?)";
                    MYSQL_STMT* stmt = mysql_stmt_init(my.get_mysql());
                    mysql_stmt_prepare(stmt, sql, strlen(sql));

                    MYSQL_BIND bind[3];
                    memset(bind, 0, sizeof(bind));

                    bind[0].buffer_type = MYSQL_TYPE_STRING;
                    bind[0].buffer = (void*)d.title.c_str();
                    bind[0].buffer_length = d.title.length();

                    bind[1].buffer_type = MYSQL_TYPE_STRING;
                    bind[1].buffer = (void*)d.content.c_str();
                    bind[1].buffer_length = d.content.length();

                    bind[2].buffer_type = MYSQL_TYPE_STRING;
                    bind[2].buffer = (void*)d.url.c_str();
                    bind[2].buffer_length = d.url.length();

                    if(0 != mysql_stmt_bind_param(stmt, bind) ) {
                        std::cout << count << mysql_stmt_error(stmt) << std::endl;
                    }
                    if(0 != mysql_stmt_execute(stmt) ){
                        std::cout << count << mysql_stmt_error(stmt) << std::endl;
                    }
                    mysql_stmt_close(stmt);
                }
                std::cout << "正排索引插入完成,共计：" << count << std::endl;
                std::cout << "开始插入倒排索引" <<std::endl;
                count = 0;
                for (auto& d : inverted_index) {
                    count++;
                    if(count % 100 == 0) 
                        std::cout << "倒排索引插入进度: " << count << std::endl;
                    const char* sql = "insert into inverted_index (word) values (?)";
                    MYSQL_STMT* stmt = mysql_stmt_init(my.get_mysql());
                    mysql_stmt_prepare(stmt,sql,strlen(sql));
                    MYSQL_BIND bind;
                    memset(&bind,0,sizeof(bind));

                    bind.buffer_type = MYSQL_TYPE_STRING;
                    bind.buffer = (void*)d.first.c_str();
                    bind.buffer_length = d.first.size();

                    mysql_stmt_bind_param(stmt,&bind);
                    mysql_stmt_execute(stmt);
                    mysql_stmt_close(stmt);

                    for (InvertedElem& l : d.second) {
                        char buffer[128];
                        //这里的参数都是数字，所以不会对sql语句造成影响，所以不需要绑定，
                        sprintf(buffer,\
                        "insert into inverted_list (inv_id,doc_id,weight) values(%d,%d,%d)",\
                        count,(int)l.doc_id,l.weight);
                        my.query(buffer);
                    }
                    //std::cout <<std::endl;
                }
                std::cout << "倒排索引插入完成,共计：" << count << std::endl;

                return true;
            }
    };
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
}







