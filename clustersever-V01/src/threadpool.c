/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * FileName: threadpool.c
 * Author:      RenWeiGuo  Version :  1.0        Date: 2017.7.21
 * Description:     线程池的编写与测试
 * Version:         1.0
 * Function List:
    1. -------
 * History:
      <author>  <time>   <version >   <desc>
      RenWeiiGuo    17/07/21     1.0     build this moudle
 */
#include <threadpool.h>

/**
 * Function:       threadpool_init
 * Description:    实现线程池的初始化，设定最大有多少个线程，最多允许多少个任务
 * Calls:          malloc pthread_create printf exit
 * Called By:
 * Input:          thread_max,设置线程的最大数目 job_max 设置任务的最大数目

 * Output:         线程池的入口地址
 * Return:         pool
 * Others:         无
 */
struct threadpool *threadpool_init(int thread_max,int job_max)
{
    struct threadpool * pool = (struct threadpool *)malloc(sizeof(struct threadpool));//建立一个线程池
    pool->pthread_max = thread_max;
    pool->pthread_close = 0;//表示打开线程
    pool->threadID = (pthread_t *)malloc(sizeof(pthread_t)*pool->pthread_max);
	printf("pool thread malloc\n");
    //初始化互斥锁和条件变量
    pthread_mutex_init ( &(pool->mutex_job_lock), NULL);
    pthread_cond_init( &pool->cond_isjob,NULL);

    pool->pjob_max = job_max;
    pool->job_head = NULL;//初始化时还没有任务
    pool->job_tail = NULL;
    pool->job_current_num = 0;

    int i = 0;
    int err;//表明线程是否创建成功

    while (i < pool->pthread_max)
    {
        err = pthread_create(&pool->threadID[i],NULL,thread_basic_function,pool);
        if (err != 0)
        {
            printf("线程池 线程%d创建失败\n",i);
            exit(1);
        }
        i++;
    }
    printf("创建%d线程成功\n",pool->pthread_max);
    return pool;

}
/**
 * Function:       thread_basic_function
 * Description:    实现单个线程的初始化功能，没有任务时，他处于堵塞状态，有任务则进如队列中
 * Calls:           pthread_mutex_lock  pthread_cond_wait pthread_mutex_unlock free pthread_exit；
 * Called By:
 * Input:          arg；这里这个参数用来传递线程池

 * Output:         无
 * Return:         无
 * Others:         2017.8.3 把任务放入锁中，只有执行完程序才会解锁，回调函数必须是非循环的函数。又改回来了
 */
void *thread_basic_function(void*arg)
{
    struct threadpool *pool = (struct threadpool*)(arg);
    struct job *job_temp ;//用来存放临时的job

    while(1)
    {
        pthread_mutex_lock(&pool->mutex_job_lock);//上锁
      //  printf("current_num = %d\n",pool->job_current_num);
        while (pool->job_current_num == 0 && pool->pthread_close == 0)
        {
  //          printf("current_num = %d\n",pool->job_current_num);
            pthread_cond_wait(&pool->cond_isjob, &pool->mutex_job_lock);
        }

        if (pool->pthread_close == 1)//表示关闭线程
        {
  //          printf("线程结束\n");
            pthread_mutex_unlock(&pool->mutex_job_lock);/*不能在上锁的情况下结束线程*/
            pthread_exit((void*)(3));//退出线程
  //          printf("pthread_exit end\n");
        }

        job_temp = pool->job_head;//减

        pool->job_head = job_temp->next;
        pool->job_current_num--;//表明已经用过线程
 //       traverse_job(pool);//遍历一下任务组
        if (pool->job_current_num == 0)//减完了
        {
           pool->job_tail = NULL;
           pool->job_head = NULL;
        }

        pthread_mutex_unlock(&pool->mutex_job_lock);
		(job_temp->callbackfunction)(job_temp->arg);//执行外面的任务
        free(job_temp);//执行完之后释放掉变量
    }
}

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
int add_pthread_job(struct threadpool * pool,void *(*callbackfunction)(void *arg),void *arg)
{
	if (pool->job_current_num >= pool->pjob_max)
	{
		return 0;
	}
    struct job* work = (struct job*)malloc(sizeof(struct job));
   // printf(" job is malloc\n");
    if (work == NULL)
    {
      printf("malloc is failed\n");
      return 0;
    }
    work->callbackfunction = callbackfunction;
    work->arg = arg;
    work->next = NULL;

    pthread_mutex_lock(&pool->mutex_job_lock);//上锁
 //   printf("pthread_mutex is sucessful,arg is %s\n",(char *)(work->arg) );

    pool->job_current_num++;
    if (pool->job_head == NULL && pool->job_tail == NULL)//表示从开始加
    {
        pool->job_head = work;
        pool->job_tail = work;
    }else
    {
        pool->job_tail->next = work;//加
        pool->job_tail = work;
    }
  //  traverse_job(pool);//遍历一下任务组
 //   printf("job_current_num+ is sucessful\n");
    if (pool->pthread_close == 1)
    {
        return 0;//直接退出函数
    }
    if (pool->job_current_num > 0)
    {
        pthread_cond_signal(&pool->cond_isjob);//告诉一些线程有任务啦
  //      printf("pthread_cond_signal is sucessful\n");
    }
    pthread_mutex_unlock(&pool->mutex_job_lock);

    return 1;
}
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
 void pthread_pool_destory(struct threadpool * pool)
 {
    pthread_mutex_lock(&pool->mutex_job_lock);//上锁

    if (pool->job_current_num != 0)
    {
        struct job *job_temp = pool->job_head;//在这里 job_temp起一个遍历的作用
        while (job_temp != NULL)
        {
            job_temp = job_temp->next;
            free(pool->job_head );
            pool->job_head = job_temp;
        }
        pool->job_tail = NULL;
        pool->job_current_num = 0;
    }

 //   printf("job num is over\n");
    pthread_mutex_unlock(&pool->mutex_job_lock);
    pool->pthread_close = 1;//关闭线程
    pthread_cond_broadcast(&pool->cond_isjob);//唤醒所有都塞的线程

    int i = 0;
    void *thread_ret;
    while (i < pool->pthread_max)//检测线程是不是都关闭啦
    {
        pthread_join(pool->threadID[i], &thread_ret);//等待线程关闭
//        printf("pool->pthread_max is %d\n",i);
        i++;
    }
    printf("all pthread is over\n");
    pthread_cond_destroy(&pool->cond_isjob);//释放条件变量
    pthread_mutex_destroy(&pool->mutex_job_lock);// 销毁互斥锁

    free (pool->threadID);//释放线程的空间
    free(pool);//释放线程池
 }
/**
 * Function:       traverse_job
 * Description:    检查job
 * Calls:          printf
 * Called By:
 * Input:          pool： 线程池；

 * Output:         无
 * Return:         无
 * Others:         无
 */
 void traverse_job(struct threadpool * pool)
 {
        struct job *job_preive = pool->job_head;
        while (job_preive != NULL)
        {
            printf("job is %s\n",(char *)(job_preive->arg));
            job_preive =job_preive->next;
        }
 }
