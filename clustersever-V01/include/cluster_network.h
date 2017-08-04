/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * File name:      cluster_network.h
 * Author:    RenWeiGuo   Version:    1.0    Date: 2017.7.31
 * Description:   epoll函数以及socket编程的一些初始化操作的索引
 * Others:         无
 * Function List:
    1. ....
 * History:
    1. Date:
       Author:
       Modification:
   2. ...
 */
#ifndef __CLUSTER_NETWORK_H__
#define __CLUSTER_NETWORK_H__


#include <errno.h>//存放着一些系统错误
#include <sys/epoll.h>//epoll系统调用的接口
#include <sys/types.h>//存放着accept的系统调用接口
#include <sys/socket.h>//同上
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> //htons 等一些函数的操作
#include "wrap.h"  //对read 等一些函数进行优化

#define SERV_PORT 8000 //端口号

struct socket_msg
{
    int listenfd;//一个用来初始监听端口.
    struct sockaddr_in servaddr,cliaddr;//一个服务端绑定用的地址，另一个是接收客户端的
    socklen_t addrlen ;//表示客户端的长度
};//socket信息

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
struct socket_msg * socket_init(struct socket_msg *socketMsg);


#endif //__CLUSTER_NETWORK_H__
