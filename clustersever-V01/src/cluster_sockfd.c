/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * FileName: cluster_sockfd.c
 * Author:      RenWeiGuo  Version :  1.0        Date: 2017.7.31
 * Description:     对sock文件描述符进行统一的管理
 * Version:         1.0
 * Function List:
    1. -------
 * History:
      <author>  <time>   <version >   <desc>
      RenWeiiGuo    17/07/31     1.0     build this moudle
 */

#include "cluster_sockfd.h"

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
void sockfd_Init(struct sockFdMsg *sockfd_container)
{
	sockfd_container->sock_head = NULL;
	sockfd_container->sock_tail = NULL;
	sockfd_container->sockfd_current_num = 0;

    //初始化互斥锁
    pthread_mutex_init ( &(sockfd_container->mutex_sock_lock), NULL);
}

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
int add_sockfd(struct sockFdMsg *sockfd_container,int sockfd)
{
	struct sockFdQue *sockTmp = (struct sockFdQue*)malloc(sizeof(struct sockFdQue));
//	printf("in add_socked malloc\n");
	if (sockTmp == NULL)
	{
		printf("malloc is failed\n");
		exit(1);
	}
	sockTmp->next = NULL;
	sockTmp->socketfd = sockfd;

	pthread_mutex_lock(&sockfd_container->mutex_sock_lock);
	if (sockfd_container->sockfd_current_num == 0)
	{
		sockfd_container->sock_head = sockTmp;
		sockfd_container->sock_tail = sockTmp;
	}else
	{
		sockfd_container->sock_head->next = sockTmp;
		sockfd_container->sock_head = sockTmp;
	}
	sockfd_container->sockfd_current_num++;
	pthread_mutex_unlock(&sockfd_container->mutex_sock_lock);
//	printf("sockfd_current num is %d\n ",sockfd_container->sockfd_current_num);
	return 1;
}

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
int rm_sockfd(struct sockFdMsg *sockfd_container)
{
	struct sockFdQue *sockTmp = sockfd_container->sock_tail;

	if (sockfd_container->sockfd_current_num != 0)
	{
		sockfd_container->sock_tail = sockTmp->next;
		free(sockTmp);
		sockfd_container->sockfd_current_num--;//个数减一
		return 1;

	}else
	{
		sockfd_container->sock_tail = NULL;
		sockfd_container->sock_head = NULL;
		return 0;
	}

}
