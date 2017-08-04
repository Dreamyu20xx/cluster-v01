/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * File name:      cluster_sockfd.h
 * Author:    RenWeiGuo   Version:    1.0    Date: 2017.7.31
 * Description:   对sock文件描述符进行统一的管理
 * Others:         无
 * Function List:
    1. ....
 * History:
    1. Date:
       Author:
       Modification:
   2. ...
 */
#ifndef __CLUSTER_SOCKFD_H__
#define __CLUSTER_SOCKFD_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct sockFdQue{
	int socketfd;
    struct sockFdQue *next;
};

struct sockFdMsg
{
	struct sockFdQue *sock_head;//sock头
	struct sockFdQue *sock_tail;//sock尾
	int sockfd_current_num;//当前任务数
	pthread_mutex_t mutex_sock_lock;//封锁对sock集的访问
};

/**
 * Function:       sockfd_init
 * Description:    实现sockfd的初始化
 * Calls:          pthread_mutex_init
 * Called By:
 * Input:			  struct sockFdMsg *sockfd_container

 * Output:         struct sockFdMsg *sockfd_container：对里面的sockFdMsg初始化
 * Return:         无
 * Others:         无
 */
void sockfd_Init(struct sockFdMsg *sockfd_container);

/**
 * Function:       add_sockfd
 * Description:    添加sockfd到队列中
 * Calls:          pthread_mutex_init malloc
 * Called By:
 * Input:			  struct sockFdMsg *sockfd_container int sockfd :输入的文件描述符

 * Output:         struct sockFdMsg *sockfd_container：对里面的sockFdMsg初始化
 * Return:         1:成功
 * Others:         无
 */
int add_sockfd(struct sockFdMsg *sockfd_container,int sockfd);

/**
 * Function:       rm_sockfd
 * Description:    移除sockfd到队列中
 * Calls:          pthread_mutex_init free
 * Called By:
 * Input:			  struct sockFdMsg *sockfd_container

 * Output:         struct sockFdMsg *sockfd_container
 * Return:         1:成功 0:失败
 * Others:         无
 */
int rm_sockfd(struct sockFdMsg *sockfd_container);

#endif //__CLUSTER_EVENT_H__



