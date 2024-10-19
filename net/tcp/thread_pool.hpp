#pragma once


#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
namespace ns_thread{
    template<class T>
    class ThreadPool{
    
    private:
        ThreadPool(int thread_num,std::function<void(const T&)> handler) :quit_(false), handler_(handler)
        {
            for (int i = 0; i < thread_num; i++) {
                consumer_.emplace_back([this](){
                    for (;;) {
                        T task;
                        {
                            std::unique_lock<std::mutex> lock(mtx_);
                            //等待条件，如果不符合这个条件的话，就代表没有任务了，就会继续等待
                            cond_.wait(lock,[this](){return !tasks_.empty(); });
                            task = tasks_.front();
                            tasks_.pop();
                        }
                        handler_(task);
                    }
                });
            }
        }

    public:
        static ThreadPool<T>* GetInstance(std::function<void(const T&)> fun = nullptr) {
            if (pool_ == nullptr) {
                mtx_.lock();
                if (pool_ == nullptr) {
                    int thread_num = std::thread::hardware_concurrency();
                    pool_ = new ThreadPool<T>(thread_num, fun);
                }
                mtx_.unlock();
            }
            return pool_;
        }
        ~ThreadPool() {
            int size = consumer_.size();
            for (int i = 0; i < size; i++) {
                consumer_[i].join();
            }
            if(pool_)
                delete pool_;
        }
        void Push(const T& task){
            mtx_.lock();
            tasks_.push(task);
            mtx_.unlock();
            cond_.notify_one();
        }

        void exit() {

        }


    private:
        std::queue<T>                       tasks_;
        std::vector<std::thread>            consumer_;
        std::condition_variable             cond_;
        bool                                quit_;

        static ThreadPool<T>*               pool_;
        static std::mutex                   mtx_;

        std::function<void(const T&)>       handler_;

    };
    template<class T>
    ThreadPool<T>* ThreadPool<T>::pool_ = nullptr;
    template<class T>
    std::mutex ThreadPool<T>::mtx_;
}