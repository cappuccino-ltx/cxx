


#include <iostream>
#include "index/index.hpp"
#include "searcher/searcher.hpp"

using namespace ns_index;

int main() {
    //Index* index = Index::GetInstance();
    //index->LoadIndex();

    // ns_searcher::Searcher *search = new ns_searcher::Searcher();
    // //search->InitSearcher();
    // std::string query;
    // std::string json_string;
    // char buffer[1024];
    // while(true){
    //     std::cout << "Please Enter You Search Query# ";
    //     fgets(buffer, sizeof(buffer)-1, stdin);
    //     buffer[strlen(buffer)-1] = 0;
    //     query = buffer;
    //     search->Search(query, &json_string);
    //     std::cout << json_string << std::endl;
    // }

    ns_index::Index *index = ns_index::Index::GetInstance();
    index->test_inverted_load();
    
    return 0;
}