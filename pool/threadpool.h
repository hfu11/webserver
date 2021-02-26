#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <iostream>
#include <exception>
#include <pthread.h>
#include "locker.h"

template <typename T>
class threadpool{
public:
    threadpool(int thread_number = 8,int max_requests = 10000);

    ~threadpool();

    //添加新任务
    bool append(T* request);

private:
    //工作线程运行
    static void* worker_cb(void* arg);
    void run();

    int m_thread_number;    //线程数
    int m_max_requests;     //请求队列允许的最大请求数
    pthread_t *m_threads;   //线程池数组
    std::list<T*> m_workqueue;  //请求队列
    locker m_queuelocker;   //保护请求队列的锁
    sem m_queuestat;    //是否有任务需要处理
    bool m_stop;    //是否结束线程
};

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_requests):m_thread_number(thread_number),m_max_requests(max_requests),m_stop(false),m_threads(NULL){
    if(thread_number<=0 || max_requests<=0){
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads){
        throw std::exception();
    }

    //创建线程池
    for (int i = 0; i < m_thread_number; i++)
    {
        std::cout<<"create thread "<<i<<std::endl;
        if(pthread_create(&m_threads[i],NULL,worker_cb,this) != 0){
            delete [] m_threads;
            throw std::exception();
        }  

        if(pthread_detach(m_threads[i])){
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
threadpool<T>::~threadpool(){
    delete [] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* req){
    //对队列操作之前先加锁
    m_queuelocker.lock();

    if(m_workqueue.size() > m_max_requests){
        m_queuelocker.unlock();
        std::cout<<"msg queue full"<<std::endl;
        return false;
    }
    //加入队列
    m_workqueue.push_back(req);
    //解锁
    m_queuelocker.unlock();
    //通知线程来处理
    m_queuestat.post();

    return true;
}

//线程的回调函数是一个静态函数，因此需要用void* arg将对象返回给回调函数。之后才能通过返回的参数来调用类的动态成员
template<typename T>
void *threadpool<T>::worker_cb(void* arg){
    threadpool *pool = (threadpool*)arg;

    pool->run();

    return pool;
}

template<typename T>
void threadpool<T>::run(){
    while(!m_stop){
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }
        T* req = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!req){
            continue;
        }
        //执行任务
        req->Process();
    }
}

#endif