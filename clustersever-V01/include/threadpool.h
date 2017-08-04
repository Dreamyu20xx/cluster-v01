/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * File name:      threadpool.h
 * Author:    RenWeiGuo   Version:    1.0    Date: 2017.7.21
 * Description:   此程序在于完成线程池的一些基本操作，便于后面epoll后面的使用
 * Others:         无
 * Function List:
    1. ....
 * History:
    1. Date:
       Author:
       Modification:
   2. ...
 */
#ifndef __THRREADPOOL_H__
#define __THRREADPOOL_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct job{
    void *(*callbackfunction)(void *arg);
    void *arg;
    struct job *next;
};

struct threadpool
{
    pthread_t *threadID; //指向线程的指针，实际代表的线程ID集
    int pthread_max;//创造线程的最大数目
    int pjob_max;//创造任务的最大数目
    int job_current_num;//当前任务数
    int pthread_close;//是否关闭线程池

    struct job *job_head;//传入job的任务工作头
    struct job *job_tail;//传入job的任务工作尾

    pthread_mutex_t mutex_job_lock;//封锁任务资源的互斥锁
    pthread_cond_t cond_isjob;//是否有任务的条件变量
};

/**
 * Function:       threadpool_init
 * Description:    实现线程池的初始化，设定最大有多少个线程，最多允许多少个任务
 * Calls:          malloc pthread_create printf exit
 * Called By:
 * Input:          thread_max,设置线程的最大数目 job_max 设置任务的最大数目

 * Output:         线程池的地址
 * Return:         pool
 * Others:         无
 */
struct threadpool *threadpool_init(int thread_max,int job_max);

/**
 * Function:       thread_basic_function
 * Description:    实现单个线程的初始化功能，没有任务时，他处于堵塞状态，有任务则进如队列中
 * Calls:           pthread_mutex_lock  pthread_cond_wait pthread_mutex_unlock free；
 * Called By:
 * Input:          arg；这里这个参数用来传递线程池

 * Output:         无
 * Return:         无
 * Others:         无
 */
void *thread_basic_function(void*arg);

/**
 * Function:       add_pthread_job
 * Description:    添加线程任务
 * Calls:          malloc pthread_mutex_lock pthread_mutex_unlock pthread_cond_signal
 * Called By:
 * Input:          pool： 线程池；callbackfunction：用户自己的函数，arg：用户自己的函数的形参。

 * Output:         1:成功；0：失败
 * Return:         无
 * Others:         无
 */
int add_pthread_job(struct threadpool * pool,void *(*callbackfunction)(void *arg),void *arg);
/**
 * Function:       pthread_pool_destory
 * Description:    销毁线程池（销毁各个线程，互斥锁，条件变量，线程的空间，线程池）
 * Calls:          free pthread_mutex_lock pthread_mutex_unlock pthread_cond_destroy pthread_cond_broadcast
                        pthread_mutextattr_destroy pthread_join
 * Called By:
 * Input:          pool： 线程池；callbackfunction：用户自己的函数，arg：用户自己的函数的形参。

 * Output:         1:成功；0：失败
 * Return:         无
 * Others:         无
 */
void pthread_pool_destory(struct threadpool * pool);

/**
 * Function:       traverse_job;
 * Description:    检查job
 * Calls:          printf
 * Called By:
 * Input:          pool： 线程池；

 * Output:         1:成功；0：失败
 * Return:         无
 * Others:         无
 */
 void traverse_job(struct threadpool * pool);

#endif //__THRREADPOOL_H__

