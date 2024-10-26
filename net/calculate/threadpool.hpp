#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>


namespace ns_thread{

    template<class T>
    class ThreadPool{

    private:
        ThreadPool(std::function<void(T)> collback,int t_num){
            
            for (int i = 0; i < t_num; i++) {
                consumer_.emplace_back([collback,this](){
                    for (;;) {
                        
                        T task;
                        {
                            std::unique_lock<std::mutex> lock(this->mtx_);
                            this->cond_.wait(lock,[this](){return !this->tasks_.empty();});
                            task = this->tasks_.front();
                            this->tasks_.pop();

                        }
                        collback(task);
                    }
                });
            }
        }

    public:

        static ThreadPool<T>* GetInstance(std::function<void(T)> fun = [](T){}) {
            if(nullptr == pool_){
                std::unique_lock<std::mutex> lock(mtx_);
                if(nullptr == pool_) {
                    int t_num = std::thread::hardware_concurrency();
                    pool_ = new ThreadPool<T>(fun,t_num);
                }
            }
            return pool_;
        }
        
        

        void push(const T& task) {
            {
                std::unique_lock<std::mutex> lock(mtx_);
                tasks_.push(task);
                cond_.notify_all();
            }
        }

        ~ThreadPool(){
            for (int i = 0; i < consumer_.size(); i++) {
                consumer_[i].join();
            }
        }

    private:
        std::vector<std::thread>        consumer_;
        std::queue<T>                   tasks_;
        std::condition_variable         cond_;
        
        static std::mutex               mtx_;
        static ThreadPool<T>*           pool_;

    };
    template<class T>
    std::mutex               ThreadPool<T>::mtx_;
    template<class T>
    ThreadPool<T>*           ThreadPool<T>::pool_ = nullptr;


    

}