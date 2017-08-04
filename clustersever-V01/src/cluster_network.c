/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * FileName: cluster_network.c
 * Author:      RenWeiGuo  Version :  1.0        Date: 2017.7.31
 * Description:     epoll函数以及socket编程的一些初始化操作
 * Version:         1.0
 * Function List:
    1. -------
 * History:
      <author>  <time>   <version >   <desc>
      RenWeiiGuo    17/07/31     1.0     build this moudle
 */
#include "cluster_network.h"

/**
 * Function:       socket_init
 * Description:    实现sock的初始化，依次执行sock（），bind(),listen()函数，
 * Calls:          socket bind  listen
 * Called By:
 * Input:			  struct socket_msg *socketMsg

 * Output:         struct socket_msg *socketMsg：对里面的socket初始化
 * Return:         socketMsg
 * Others:         无
 */
struct socket_msg * socket_init(struct socket_msg *socketMsg)
{
	bzero(&socketMsg->servaddr, sizeof(socketMsg->servaddr));
	socketMsg->servaddr.sin_family = AF_INET;
	socketMsg->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketMsg->servaddr.sin_port = htons(SERV_PORT);
	socketMsg->listenfd = socket(AF_INET,SOCK_STREAM,0);
	int opt = 1;
	setsockopt(socketMsg->listenfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)&opt,sizeof(opt));

	if( bind(socketMsg->listenfd, (struct sockaddr*)(&socketMsg->servaddr), sizeof(socketMsg->servaddr)) == -1)
	{
		printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}
	if( listen(socketMsg->listenfd, 10) == -1)
	{
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}  //开始监听
	printf ("listenfd is %d\n",socketMsg->listenfd);
	return socketMsg;

}
