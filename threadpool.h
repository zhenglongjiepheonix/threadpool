#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include<pthread.h>
#include<memory>
#include<vector>
#include<iostream>
using namespace std;


class MythreadPool
{
private:
    struct task
    {
        function<void(shared_ptr<void>)> fun;
        shared_ptr<void> args;
    };
    pthread_mutex_t lock;//lock 
    pthread_cond_t notify;// enter when pool is not empty
    vector<pthread_t> threadpool;
    vector<task> taskqueue;
    int head;
    int tail;
    int thread_count;//how many threads created
    int queue_siz;
    int task_count;//how many tasks running
    int shutdown;
    int started;//how many threads start running
public:
    MythreadPool(int _thread_count = 4, int _queue_siz = 1024);
    int warm_up();
    int close();
    static void* thread_execute(void* args);
    int add(shared_ptr<void> args, function<void(shared_ptr<void>)> fun);
};



#endif // THREADPOOL_H_INCLUDED

