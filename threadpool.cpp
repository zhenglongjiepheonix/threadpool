#include "threadpool.h"


MythreadPool::MythreadPool(int _thread_count, int _queue_siz):thread_count(_thread_count),queue_siz(_queue_siz)
{
    lock = PTHREAD_MUTEX_INITIALIZER;
    notify = PTHREAD_COND_INITIALIZER;
    task_count = 0;
    head = tail = 0;
    shutdown = started = 0;
    threadpool.resize(_thread_count);
    taskqueue.resize(_queue_siz);
}

int MythreadPool::warm_up()
{
    for(int i = 0; i < thread_count; i++)
    {
        if(pthread_create(&threadpool[i],NULL,thread_execute,(void*)(this)) != 0)
        {
            thread_count = i;
            return -1;
        }
        else
        {
            started++;
        }
    }
    return 0;
}

int MythreadPool::add(shared_ptr<void> args, function<void(shared_ptr<void>)> fun)
{
    if(pthread_mutex_lock(&lock) != 0)
    {
        return -1;
    }
    if(task_count == queue_siz || shutdown)return -1;

    taskqueue[tail].fun = fun;
    taskqueue[tail].args = args;

    tail = (tail+1)%queue_siz;
    ++task_count;

    if(pthread_cond_signal(&notify) != 0)
    {
        return -1;
    }
    if(pthread_mutex_unlock(&lock) != 0)return -1;

    return 0;
}
int MythreadPool::close()
{
    pthread_mutex_lock(&lock);
    shutdown = 1;
    pthread_mutex_unlock(&lock);
    return 0;
}

void* MythreadPool::thread_execute(void* args)
{
    MythreadPool *p = (MythreadPool*)args;
    while(1)
    {
        task t;
        pthread_mutex_lock(&(p->lock));
        while(p->task_count == 0 && !(p->shutdown))pthread_cond_wait(&(p->notify),&(p->lock));
        if(p->shutdown)
        {
            break;
        }
        t.fun = (p->taskqueue)[p->head].fun;
        t.args = (p->taskqueue)[p->head].args;
        (p->taskqueue)[p->head].fun = NULL;
        (p->taskqueue)[p->head].args = NULL;
        p->head = (p->head+1)%(p->queue_siz);
        p->task_count--;
        pthread_mutex_unlock(&(p->lock));
        (t.fun)(t.args);
    }
    --(p->started);
    pthread_mutex_unlock(&(p->lock));
    pthread_exit(NULL);
    return NULL;
}
