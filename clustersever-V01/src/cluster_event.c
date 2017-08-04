/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * FileName: cluster_event.c
 * Author:      RenWeiGuo  Version :  1.0        Date: 2017.7.31
 * Description:     对事件循环中的一些事件进行操作
 * Version:         1.0
 * Function List:
    1. -------
 * History:
      <author>  <time>   <version >   <desc>
      RenWeiiGuo    17/07/31     1.0     build this moudle
 */

#include "cluster_event.h"

static struct socket_msg socketMsg;//只能在这个文件中使用，表明服务器特有的信息，
static int g_reactor_epollfd[MAXRECTOR];//目的：建立MAXRECTOR个监听位，便于实现多reactor模式
static pthread_t g_reactor_id[MAXRECTOR];//目的：建立与reactor对应的线程
static	pthread_t accept_id;//accept主线程
struct threadpool *Pool;
/**
 * Function:       accept_thread
 * Description:    请求客户端链接的线程，一直连接
 * Calls:          Accept add_sockfd
 * Called By:
 * Input:			  arg : sockfd_container
 * Output:         NULL
 * Return:         无
 * Others:         无
 */
void *accept_thread(void *arg)
{
	struct sockFdMsg *sockfd_container = (struct sockFdMsg *)(arg);
	int sockfd;
    printf("in accept_thread is successful\n");

	while (1)
	{
		sockfd = Accept(socketMsg.listenfd,(struct sockaddr*)(&socketMsg.cliaddr),&socketMsg.addrlen);//等待socket事件发生
		add_sockfd(sockfd_container,sockfd);//加入队列中
//		printf("sockfd is %d\n",sockfd);
	}

	return NULL;
}

/**
 * Function:       epoll_init
 * Description:    创建 MAXRECTOR 的线程用来服务各个客户端
 * Calls:
 * Called By:
 * Input:          无文件信息

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void epoll_init(struct sockFdMsg *sockfd_container)
{
	int i;
	int err;
	for (i = 0; i < MAXRECTOR; i++)
	{

		err = pthread_create(&g_reactor_id[i], NULL, epoll_reactor, sockfd_container);

		if (err != 0)
		{
			fprintf(stderr, "can't create thread: %s\n", strerror(err));
			exit(1);
		}
	}
}

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
void *epoll_reactor(void *arg)
{

	int epollfd = epoll_create(MAXCLIENT); //创建一个epfd监听文件集

	struct EVENT bigEvent;// 大事件集合
    struct epoll_event epollevent[MAXCLIENT];//原始事件集合

	struct sockFdSet  sockfd_set;//定义这个集
	sockfdset_Init(&sockfd_set,MAXCLIENT);//初始化这个socket集
	sockfd_set.epollfd = epollfd;//在外面把epollfd输入进去
	event_Init(&bigEvent,epollfd);//事件链表初始化

	struct sockFdMsg *sockfd_container = (struct sockFdMsg *)arg;

	int n ;//表示接收事件的个数

	while (1)
	{
		In_reactor_event(sockfd_container,epollfd,sockfd_set.sockfd_set,&sockfd_set.maxfd);//新连接事件进队列
		n = epoll_wait(epollfd,epollevent,MAXCLIENT,10);
		if(n > 0 )
		{
			My_event(&bigEvent,epollevent,n);
			Read_event(Pool,&bigEvent);//用到了线程池
		}

		Remove_event(&sockfd_set,&bigEvent);//没用到线程池

	}

	return NULL;

}
/**
 * Function:       My_event
 * Description:    把传统的事件转换为我的事件链表
 * Calls:
 * Called By:
 * Input:          struct EVENT *bigEvent：目标事件，事件 struct epoll_event *epollevent,原始事件 int n 有几个事件需要转换

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void My_event(struct EVENT *bigEvent,struct epoll_event *epollevent,int n)
{
	int i = 0;
//	printf("n = %d\n",n);
	while (i < n)
	{
		add_epollevent(bigEvent,epollevent[i]);
//		printf("epollevent is %d\n",epollevent[i].data.fd);
		i++;
	}
}

/**
 * Function:       In_reactor_event
 * Description:    当进行线程调度时，差不多都要执行这个函数，每次从sock队列中获取一部分给自己用
 * Calls:			  pthread_mutex_lock pthread_mutex_unlock
 * Called By:
 * Input:          struct sockFdMsg *sockfd_container：表明那个总socket队列，epollfd：epoll描述符，sockfd_set：socket集，
						maxfd：sockfd_set标记里面最大的一个

 * Output:         无
 * Return:			  1:成功 0:失败
 * Others:         2017.8.2在这个函数中出现段错误
					  2017.8.3线程申请到50就不会出现这个问题
 */
int In_reactor_event(struct sockFdMsg *sockfd_container,int epollfd,int*sockfd_set,int *maxfd)
{
	struct epoll_event tev;//用来临时申请事件
	int set_num = 0;//标记进入sockfd_set;
	int event_num = 0;//进入线程的个数

	pthread_mutex_lock(&sockfd_container->mutex_sock_lock);
	/*三种情况1.源没有，2.每次超过放入的个数，3.超了自己所能承受的范围*/
	while ((sockfd_container->sockfd_current_num > 0) && (event_num < MAXCLIENT/100) && (set_num < MAXCLIENT))
	{
		while (sockfd_set[set_num] != -1)//小心这里段错误
		{
			set_num++;
			if (set_num == MAXCLIENT)return 0;
		}
		if(set_num == MAXCLIENT)//这个不能从开始识别出来，所以要加这句话
		{
			break;
		}
		tev.events = EPOLLIN;//放到读事件中监听
		tev.data.fd = sockfd_container->sock_tail->socketfd;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, tev.data.fd, &tev);
		rm_sockfd(sockfd_container);//任务队列减去一个

		sockfd_set[set_num] = tev.data.fd;//放到队列中
		event_num++;

//		printf("sockfd_current_num is %d,set_num is %d,event_num is %d\n",sockfd_container->sockfd_current_num,set_num,event_num);
//		printf("sockfd_set[%d] = %d\n",set_num,sockfd_set[set_num]);
//	printf("pthead %d get some data \n",pthread_self());
	}
	pthread_mutex_unlock(&sockfd_container->mutex_sock_lock);
	if ((*maxfd < set_num)&&(set_num< MAXCLIENT))*maxfd = set_num;

	return 1;
}
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
void sockfdset_Init(struct sockFdSet *sockfd_set,int size)
{
 	int i = 0;
 	while (i < size)
 	{
 		sockfd_set->sockfd_set[i] = -1;
 		i++;
 	}
	sockfd_set->maxfd = 0;
	sockfd_set->epollfd = -1;
 }

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
void event_Init(struct EVENT *bigEvent,int epollfd)
{
	bigEvent->epollfd = epollfd;
	bigEvent->head = NULL;
	bigEvent->active_sockfd = -1;
	pthread_mutex_init ( &(bigEvent->mutex_EVENT_lock), NULL);
}
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
void add_epollevent(struct EVENT *bigEvent,struct epoll_event epollevent)
{
	struct epollEvent *epollevent_perevise = bigEvent->head;//遍历
	struct epollEvent *epollevent_temp = (struct epollEvent *)malloc(sizeof(struct epollEvent));//申请内存
//	printf("epollevent is %d\n",epollevent.data.fd);
	if (epollevent_temp == NULL)
	{
		printf("malloc is failed");//申请失败
		exit(2);
	}
	epollevent_temp->epollevent = epollevent;
	epollevent_temp->isopen = SOCKOPEN;
	epollevent_temp->next = NULL;
	pthread_mutex_lock(&bigEvent->mutex_EVENT_lock);//上锁
/*******************找位置**************************************/
	if (bigEvent->head != NULL) //非空
	{
		while (epollevent_perevise->epollevent.data.fd != epollevent.data.fd && epollevent_perevise->next != NULL)//条件本身是最后一个元素和已经找到和他相同的元素
		{
			epollevent_perevise = epollevent_perevise->next;
		}
		if (epollevent_perevise->epollevent.data.fd == epollevent.data.fd)//再次判断是不是两者相等
		{
			epollevent_perevise->isopen = SOCKOPEN;//两者相等，则直接改它的文件描述符
			free(epollevent_temp);// 并释放掉申请的内存
			printf("来了数据未处理,下次又来了，LT模式\n");
			pthread_mutex_unlock(&bigEvent->mutex_EVENT_lock);// 解锁
			return;//返回
		}
/******************放值************************************/
		epollevent_perevise->next = epollevent_temp;
//		printf("epollevent.data.fd is %d",epollevent_perevise->next->epollevent.data.fd);
	}else    //空链表
	{
		bigEvent->head = epollevent_temp;
	}

	pthread_mutex_unlock(&bigEvent->mutex_EVENT_lock);//解锁
}
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
void Read_event(struct threadpool * pool,struct EVENT *bigEvent)
{
	int res;//调用线程池的返回值
	struct epollEvent *epollevent_perevise = bigEvent->head;//遍历
	int issockopen = 0;//默认是没有

	while (epollevent_perevise != NULL)
	{
		if (epollevent_perevise->isopen == SOCKOPEN )//判断的依据，这个是不是可读的
		{
			issockopen = 1;//表明可以从中读取数据
			break;
		}
		epollevent_perevise =  epollevent_perevise->next;

	}
	while ((1 == issockopen ) && (1 != add_pthread_job(pool,read_thread_callfuction,bigEvent)))
	{
		usleep(200);
		printf("添加线程失败\n");
	}//调用线程池失败继续调用

}

/**
 * Function:       Remove_event
 * Description:    移除事件,从sockfd_set中和事件链表中移除这个事件
 * Calls:
 * Called By:
 * Input:          struct epollEvent *event 事件 sockfd_set：传入传出参数，用来初始化这个集，num：大小

 * Output:         无
 * Return:			  无
 * Others:         2017.8.3在删除操作哪里有一个加锁解锁操作，防止过程被打断。
 *					  20178.4 在epollevent_perevise != NULL && epollevent_perevise->isopen == SOCKCLOSE语句中不能把后一句放到前面，否则会出现段错误
 */
void Remove_event(struct sockFdSet *socketfd_set,struct EVENT *bigEvent)
{
	int i = 0;//初始化互斥锁

	struct epollEvent *epollevent_perevise = bigEvent->head;//遍历
	struct epollEvent *epollevent_revise  ;//遍历
//	printf(" bigEvent->head is %d\n",epollevent_perevise);//打印头指针
	while(epollevent_perevise != NULL && epollevent_perevise->isopen == SOCKCLOSE )  //如果是头部要删
	{
		pthread_mutex_lock(&bigEvent->mutex_EVENT_lock);//上锁
		rm_sockFdSet(socketfd_set, epollevent_perevise->epollevent.data.fd); //从这里删除
		bigEvent->head = epollevent_perevise->next; //重新找头
		free(epollevent_perevise);//释放头
		epollevent_perevise = bigEvent->head;
//		printf(" bigEvent->head is %d\n",epollevent_perevise);//打印头指针
		pthread_mutex_unlock(&bigEvent->mutex_EVENT_lock);//上锁
	}
/*********上一步退出条件：指针指向的sock不是关闭，和指针已经指向末尾如果是末尾执行下面的语句***********/
	if (epollevent_perevise == NULL)
	{
		return ;
	}
/************如果是第二种情况的发生******************9**********************************************/
	epollevent_revise = epollevent_perevise->next; //不是头部要删
	while (epollevent_perevise != NULL && epollevent_revise != NULL)
	{
		if (epollevent_revise->isopen == SOCKCLOSE)//关闭状态
		{
			pthread_mutex_lock(&bigEvent->mutex_EVENT_lock);//上锁
			rm_sockFdSet(socketfd_set, epollevent_revise->epollevent.data.fd);
			epollevent_perevise->next = epollevent_revise->next;
			free(epollevent_revise);
			epollevent_revise = epollevent_perevise->next;
			pthread_mutex_unlock(&bigEvent->mutex_EVENT_lock);//上锁
		}else
		{
			epollevent_perevise =  epollevent_perevise->next;
			epollevent_revise = epollevent_perevise->next;
		}
	}

}

/**
 * Function:       read_thread_callfuction
 * Description:    线程池调用这个回调函数实现从socket读取数据，如果read读为0，则直接关闭写入关闭
 * Calls:
 * Called By:
 * Input:          arg :在这里指，struct EVENT:确保是原子操作

 * Output:         无
 * Return:			  无
 * Others:         无
 */
void *read_thread_callfuction(void *arg)
{
	struct EVENT *bigEvent = (struct EVENT *)(arg);
	char buffer[1000];
	int i;
	bzero(buffer,1000);
	struct epollEvent *epollevent_perevise = bigEvent->head;//遍历

	pthread_mutex_lock(&bigEvent->mutex_EVENT_lock);//上锁
//	printf("read active_sockfd is %d\n",bigEvent->active_sockfd);
	while (epollevent_perevise != NULL )
	{
		if (SOCKOPEN == epollevent_perevise->isopen)
		{
				i = Read(epollevent_perevise->epollevent.data.fd,buffer,1000);
				if (i == 0 )
				{
					epollevent_perevise->isopen = SOCKCLOSE;//表明关闭
					printf("sockfd %d  is close\n",epollevent_perevise->epollevent.data.fd);
				}else if(i > 0)
				{
					epollevent_perevise->isopen = SOCKIDLE;//表明关闭
					printf("sockfd %d receive is %s\n",epollevent_perevise->epollevent.data.fd,buffer);
				}else{
					printf("read function has error\n");
				}
		}
		epollevent_perevise = epollevent_perevise->next;
	}
	pthread_mutex_unlock(&bigEvent->mutex_EVENT_lock);//上锁

}
/**
 * Function:       rm_sockFdSet
 * Description:    根据每个线程的sock集，删除这个关闭的文件描述符，
						1.把这个事件从eoll删除
						2.删除这个文件描述付
 * Calls:
 * Called By:
 * Input:          sockfd_set :事件集sockfd：要删除的文件描述符

 * Output:         无
 * Return:			  1:success
 * Others:         对于关闭sockfd,出现bad描述符的问题 解决办法
 */
int rm_sockFdSet(struct sockFdSet *socketfd_set,int sockfd)
{
	int i = 0;
	struct epoll_event tev;//用来临时申请事件

	while (sockfd != socketfd_set->sockfd_set[i])
	{
		i++;
	}
	/*从集中去除这个事件*/

	socketfd_set->sockfd_set[i] = -1;
	if (i == socketfd_set->maxfd)//重新赋值给maxfd
	{
		while (socketfd_set->sockfd_set[i] != -1)
		{
			i--;
		}
		socketfd_set->maxfd = i;//
	}

	tev.events = EPOLLIN;//
	tev.data.fd = sockfd;
    epoll_ctl(socketfd_set->epollfd, EPOLL_CTL_DEL, sockfd, &tev);//删除这个事件
	printf("del sockfd is %d\n",sockfd);//打印删掉的文件描述符
    Close(sockfd);//关闭这个sock文件描述符
    return 1;
}
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
void system_Init()
{
	Pool = threadpool_init(50,40);//线程初始化
	socket_init(&socketMsg);//监听号进行初始化
	struct sockFdMsg sockfd_container;
	sockfd_Init(&sockfd_container);

	int err;
	err = pthread_create(&accept_id, NULL, accept_thread, &sockfd_container);
	if (err != 0)
	{
		fprintf(stderr, "can't create thread: %s\n", strerror(err));
		exit(1);
	}

	epoll_init(&sockfd_container);//epoll初始化
}
