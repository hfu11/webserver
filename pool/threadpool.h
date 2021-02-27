#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>

class ThreadPool{
public:
    explicit ThreadPool(size_t threadCount = 8):pool_m(std::make_shared<Pool>()){
        assert(threadCount>0);
        for (size_t i = 0; i < threadCount; i++)
        {
            std::thread([pool = pool_m]{
                std::unique_lock<std::mutex>locker(pool->mtx);
                while(!pool->isClosed){
                    if(pool->tasks.size()){
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();
                        locker.unlock();
                        task();
                        locker.lock();
                    }
                    //wait会使线程阻塞，并释放locker
                    else pool->cond.wait(locker);
                }
            }).detach();
        }
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool(){
        if((static_cast<bool>(pool_m))){
            {
                std::lock_guard<std::mutex> locker(pool_m->mtx);
                pool_m->isClosed = true;
            }
        }
        pool_m->cond.notify_all();
    }

    template<class F>
    void AddTask(F&& task){
        {
            std::lock_guard<std::mutex> locker(pool_m->mtx);
            pool_m->tasks.emplace(std::forward<F>(task));
        }
        pool_m->cond.notify_one();
    }



private:
    struct Pool{
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_m;
};


#endif