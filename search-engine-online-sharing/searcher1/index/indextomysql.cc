

#include <iostream>

#include "../model/model_mysql.hpp"
#include "index.hpp"
#include "../comm/util.hpp"

const std::string input = "../data/raw_html/raw.txt";


using namespace ns_index;
using namespace ns_util;
using namespace ns_sql;

int main()
{
    //建立索引

    // 1. 获取或者创建index对象
    ns_index::Index* index = ns_index::Index::GetInstance();
    LOG(NORMAL, "获取index单例成功...");
    //2. 根据index对象建立索引
    index->BuildIndex(input);
    LOG(NORMAL, "建立正排和倒排索引成功...");

    index->WriteMysql();
    

    return 0;
}
