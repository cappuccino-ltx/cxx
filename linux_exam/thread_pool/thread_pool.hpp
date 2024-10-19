

#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <string>


namespace Pool{

    template<class T>
    struct thread_data{
        thread_data(const std::string& name,pthread_mutex_t* mtx, pthread_cond_t* con_pro, pthread_cond_t* con_con, std::queue<T>* tasks)
            :_name(name),_mtx(mtx),_con_pro(con_pro), _con_con(con_con),_tasks(tasks)
        {}
        std::string _name;
        pthread_mutex_t* _mtx;
        pthread_cond_t* _con_pro;
        pthread_cond_t* _con_con;
        std::queue<T>* _tasks;
    };

    template<class T> 
    class ThreadPool{
        public:
        static ThreadPool<T>* get(int pro_num = 5, int con_num = 5) {
            if (nullptr == _pool) {
                pthread_mutex_lock(&_mtx);
                if (nullptr == _pool) {
                    _pool = new ThreadPool(pro_num, con_num);
                }
                pthread_mutex_unlock(&_mtx);
            }
            return _pool;
        }

       

        void start() {
            //启动生产者线程
            for (int i = 0; i < _pro.size(); i++) {
                thread_data<T>* data = new thread_data<T>(std::string("生产者线程") + std::to_string(i),&_mtx,&_pro_con,&_con_con,&_tasks);
                pthread_create(&_pro[i],nullptr,producer,data);
            }
            //启动消费者线程
            for (int i = 0; i < _con.size(); i ++) {
                thread_data<T>* data = new thread_data<T>(std::string("消费者线程") + std::to_string(i),&_mtx,&_pro_con,&_con_con,&_tasks);
                pthread_create(&_pro[i],nullptr,consumer,data);
            }
        }
        static void* producer(void* args) {
            thread_data<T>* data = (thread_data<T>*)args;
            std::string name = data->_name;
            pthread_mutex_t* mtx = data->_mtx;
            pthread_cond_t* con_pro = data->_con_pro;
            pthread_cond_t* con_con = data->_con_con;
            std::queue<int>* tasks = data->_tasks;
            delete data;
            srand(time(nullptr));    
            while(1) {
                //生产者每一秒生产1个任务
                sleep(1);
                int num = rand();
                //获取锁
                pthread_mutex_lock(mtx);
                while(tasks->size() > 20){
                    pthread_cond_wait(con_pro,mtx);
                }
                std::cout << name << "生产任务" << num << std::endl;
                tasks->push(num);
                //发信号让消费者消费
                pthread_cond_signal(con_con);
                //解锁
                pthread_mutex_unlock(mtx);
            }
            return nullptr;
        }
        static void* consumer(void* args) {
            thread_data<T>* data = (thread_data<T>*)args;
            std::string name = data->_name;
            pthread_mutex_t* mtx = data->_mtx;
            pthread_cond_t* con_pro = data->_con_pro;
            pthread_cond_t* con_con = data->_con_con;
            std::queue<int>* tasks = data->_tasks;
            delete data;
            while(1) {
                //消费者消费，
                sleep(1);
                //获取锁.
                pthread_mutex_lock(mtx);
                while(tasks->size() == 0){
                    pthread_cond_wait(con_con,mtx);
                }
                int num = tasks->front();
                tasks->pop();
                std::cout << name << "消费任务：" << num << std::endl;

                //发信号让生产者生产
                pthread_cond_signal(con_pro);
                //解锁
                pthread_mutex_unlock(mtx);
            }
            return nullptr;
        }
    private:
        ThreadPool(int pro_num, int con_num) 
            :_pro(pro_num)
            ,_con(con_num)
        {}
    private:
        std::queue<T> _tasks;
        std::vector<pthread_t> _pro;
        std::vector<pthread_t> _con;
        static ThreadPool<T>* _pool;
        static pthread_mutex_t _mtx;
        static pthread_cond_t _pro_con;
        static pthread_cond_t _con_con;
    };
    template<class T>
    ThreadPool<T>* ThreadPool<T>::_pool = nullptr;
    template<class T>
    pthread_mutex_t ThreadPool<T>::_mtx = PTHREAD_MUTEX_INITIALIZER;
    template<class T>
    pthread_cond_t ThreadPool<T>::_pro_con = PTHREAD_COND_INITIALIZER;
    template<class T>
    pthread_cond_t ThreadPool<T>::_con_con = PTHREAD_COND_INITIALIZER;
}
