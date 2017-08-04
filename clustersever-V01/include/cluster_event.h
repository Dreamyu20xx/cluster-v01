/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * File name:      cluster_event.h
 * Author:    RenWeiGuo   Version:    1.0    Date: 2017.7.31
 * Description:   对一些事件进行一些操作
 * Others:         无
 * Function List:
    1. ....
 * History:
    1. Date:
       Author:
       Modification:
   2. ...
 */
#ifndef __CLUSTER_EVENT_H__
#define __CLUSTER_EVENT_H__

#include "cluster_sockfd.h"
#include "cluster_network.h"
#include "threadpool.h"

#define MAXRECTOR 10 //设置最大的reactor数目
#define MAXCLIENT 1000 //单个线程能结的最大客户端

/**epollEvent描述isopen的状态**/
#define SOCKCLOSE 1
#define SOCKOPEN 0
#define SOCKIDLE -1

struct sockFdSet
{
	int sockfd_set[MAXCLIENT];
	int maxfd;
	int epollfd;//每个reactor对应的epollfd
};

struct epollEvent
{
	struct epoll_event epollevent;
	int isopen;//表明这个文件的状态时否打开
	struct epollEvent*next;//数据结构链表
};

struct EVENT
{
	struct epollEvent *head;//定义事件的头指针
	int epollfd;//找到当前的epollfd
	pthread_mutex_t mutex_EVENT_lock;//封锁事件的互斥锁
	int active_sockfd;//只要改动的sock
};

/**
 * Function:       event_Init
 * Description:    初始化一个事件链表
 * Calls:
 * Called By:
 * Input:          bigEvent：传入传出参数，用来初始化这个集， epollfd:当前epoll的描述符

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void event_Init(struct EVENT *bigEvent,int epollfd);

 /**
 * Function:       add_epollevent
 * Description:    向链表中添加事件
 * Calls:
 * Called By:
 * Input:          bigEvent：这个事件，epollevent:要进去链表的事件

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void add_epollevent(struct EVENT *bigEvent,struct epoll_event epollevent);
/**
 * Function:       accept_thread
 * Description:    请求客户端链接的线程，一直连接
 * Calls:          pthread_mutex_init malloc
 * Called By:
 * Input:			  arg : struct sockFdMsg *sockfd_container
 * Output:         NULL
 * Return:         无
 * Others:         无
 */
void *accept_thread(void *arg);

/**
 * Function:       epoll_init
 * Description:    实现epoll的初始化，并开启各自的线程
 * Calls:
 * Called By:
 * Input:          无文件信息

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void epoll_init(struct sockFdMsg *sockfd_container);

/**
 * Function:       sockfd_Init
 * Description:    初始化socket集这个函数
 * Calls:
 * Called By:
 * Input:          sockfd_set：传入传出参数，用来初始化这个集，size：集的大小

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void sockfdset_Init(struct sockFdSet *sockfd_set,int size);

/**
 * Function:       epollevent_Init
 * Description:    初始化epollEvent这个函数
 * Calls:
 * Called By:
 * Input:          event：传入传出参数，用来初始化这个集，size：集的大小 epollfd:当前epoll的描述符

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void epollevent_Init(struct epollEvent *event,int size,int epollfd);

/**
 * Function:       My_event
 * Description:    把传统的事件转换为我的事件
 * Calls:
 * Called By:
 * Input:          struct epollEvent *event：目标事件，即我的事件 struct epoll_event *epollevent,原始事件 int n 有几个事件需要转换

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void My_event(struct EVENT *bigEvent,struct epoll_event *epollevent,int n);
/**
 * Function:       In_reactor_event
 * Description:    当进行线程调度时，差不多都要执行这个函数，每次从sock队列中获取一部分给自己用
 * Calls:			  pthread_mutex_lock pthread_mutex_unlock
 * Called By:
 * Input:          struct sockFdMsg *sockfd_container：表明那个总socket队列，epollfd：epoll描述符，sockfd_set：socket集，
						maxfd：sockfd_set标记里面最大的一个

 * Output:         无
 * Return:			  无
 * Others:         无
 */
int In_reactor_event(struct sockFdMsg *sockfd_container,int epollfd,int*sockfd_set,int *maxfd);

/**
 * Function:       epoll_reactor
 * Description:    单独的各个线程用来服务个客户端
 * Calls:
 * Called By:
 * Input:          现在为空

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void *epoll_reactor(void *arg);
/**
 * Function:       read_thread_callfuction
 * Description:    初始化socket集这个函数
 * Calls:
 * Called By:
 * Input:          arg :在这里时sockfd_set

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void *read_thread_callfuction(void *arg);

/**
 * Function:       rm_sockFdSet
 * Description:    根据关闭事件的条件，关闭这个线程
 * Calls:
 * Called By:
 * Input:          sockfd_set :事件集sockfd：要删除的文件描述符

 * Output:         无
 * Return:			  1:success
 * Others:         无
 */
int rm_sockFdSet(struct sockFdSet *sockfd_set,int sockfd);

/**
 * Function:       Remove_event
 * Description:    移除事件,从sockfd_set中和事件链表中移除这个事件
 * Calls:
 * Called By:
 * Input:          struct epollEvent *event 事件 sockfd_set：传入传出参数，用来初始化这个集，num：大小

 * Output:         无
 * Return:			  无
 * Others:         2017.8.3在删除操作哪里有一个加锁解锁操作，防止过程被打断。
 */
void Remove_event(struct sockFdSet *socketfd_set,struct EVENT *bigEvent);

/**
 * Function:       Read_event
 * Description:    从这些事件中，有读事件，直接交给线程池处理
 * Calls:
 * Called By:
 * Input:          struct threadpool * pool: 线程池 bigEvent:外接口的bigEvent

 * Output:         无
 * Return:			  无
 * Others:         2017.8.7:在调用添加任务时检测，添加任务是否成功
 */
void Read_event(struct threadpool * pool,struct EVENT *bigEvent);
/**
 * Function:       system_Init
 * Description:    系统初始化，初始化线程池，等一些网络信息
 * Calls:
 * Called By:
 * Input:          无

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void system_Init();
#endif //__CLUSTER_EVENT_H__
