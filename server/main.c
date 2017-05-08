/* server.c */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "wrap.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "msgclient.h"
#include "processdata.h"

#define MAXLINE 80
#define SERV_PORT 8000
chatMssage chatreceive;
chatMssage chatsend;

pthread_t ntid;
char sendbuf[MAXLINE] ;//服务器向客户端发送的数据
char readbuf[MAXLINE] ;//客户端向服务器发送的数据
addmsg readbuf_msg;//整合数据
addmsg sendbuf_msg;
int  client[FD_SETSIZE];//得到当前的客户端的东西
fd_set rset, allset;//fd_set
char normalsenddata[] ="$@00$$$@$";//默认回应的数据
int msg_status = 0;//判断是否发送数据成功，成功 1，不成功 0；

void init_net(int *listenfd,struct sockaddr_in *servaddr,int *opt);
//打印当前线程的PID号
void printids(const char *s)
{
	pid_t      pid;
	pthread_t  tid;

	pid = getpid();                      /*得到进程ID*/
	tid = pthread_self();             /*得到线程ＩＤ*/
	printf("%s pid %u tid %u (0x%x)\n", s, (unsigned int)pid,
	       (unsigned int)tid, (unsigned int)tid);
}
//创建新的进程
void *thr_fn(void *arg)
{
    node*p;
	printids(arg);
	while (1)
	{
	    processData(readbuf_msg.buffer,&chatreceive);
	    if (chatreceive.chat_valid == 1 )
	    {
	        printf("解析成功");//把数据响应发回去
	        printf("readbuf_msg.ID is %d\n",readbuf_msg.ID);
            selectCmmand(chatreceive.chat_command, &chatreceive,&chatsend,readbuf_msg.ID);
            if (chatreceive.chat_command == LOGIN ||chatreceive.chat_command == REGISTER)//处理登陆和注册信息
            {
                chatsend.chat_valid = 1;
                chatsend.chat_command = ANSEWER;
                strcpy(chatsend.chat_sendName,"admin");
                strcpy(chatsend.chat_receiveName,chatreceive.chat_sendName);
                strcpy(chatsend.chat_message,"ok");
                sendbuf_msg.ID = readbuf_msg.ID;//确定发送信息的人
            }
            if (chatreceive.chat_command == LOOK)
            {
                sendbuf_msg.ID = readbuf_msg.ID;//确定发送信息的人
            }
            if (chatreceive.chat_command == SEND)
            {
                sendbuf_msg.ID = find_clientname_node(chatreceive.chat_receiveName);
                travse_node();
                printf("chatreceive.chat_sendname is %s",chatreceive.chat_sendName);
                p = Find_node(sendbuf_msg.ID);
                printf("sockfd is %d,name is %s \n",sendbuf_msg.ID,p->clientname);

            }
            if (chatreceive.chat_command == RESPONSE)//如果另一边收到应答啦，就开始发送默认消息清空缓存
            {
                memset(sendbuf_msg.buffer,0,MAXLINE);
                printf("command = Response so clean sendbuf_msg.buffer is %s\n",sendbuf_msg.buffer);

            }else
            {
                writeDataBuffer(&chatsend,sendbuf_msg.buffer,MAXLINE);
                //printf(" sendbuf_msg.buffer is %s\n",sendbuf_msg.buffer);
            }

            printf("setup sendbuf_msg.buffer is %s\n",sendbuf_msg.buffer);//打印处理完要发送的数据
            memset(readbuf_msg.buffer,0,MAXLINE);//读缓冲请0
	    }else{//无效的数据
            }
        if (msg_status == 1)//成功发送回数据后清空
        {
                printf("clean sendbuf_msg.buffer is %s\n",sendbuf_msg.buffer);
                memset(sendbuf_msg.buffer,0,MAXLINE);
                msg_status = 0;//立刻zhi0
        }
	}
	return NULL;
}
//主程序
int main(void)
{
	int i, maxi, maxfd, connfd, sockfd;
	node*p;

    ssize_t n;
    int nready;

	char str[INET_ADDRSTRLEN];//字符转换的大小
	socklen_t cliaddr_len;//sock长度

	int listenfd;//The three variable is for TCP/IP Configration
	struct sockaddr_in	cliaddr, servaddr;

	int opt = 1;//setsockopt
	init_net(&listenfd,&servaddr,&opt);
	int fd_ssetn = 0;

	maxfd = listenfd;		/* 初始化 */
	maxi = -1;			/* index into client[] array */
	int err;//进程创建成功的标志
    Initchat(&chatreceive);//初始化收到的信息
    Initchat(&chatsend);//初始化发送的信息
    Initdata(&readbuf_msg);//初始化接收到的数据
    Initdata(&sendbuf_msg);//初始化接收到的数据
	err = pthread_create(&ntid, NULL, thr_fn, "new thread: ");
	if (err != 0)
	{
		fprintf(stderr, "can't create thread: %s\n", strerror(err));
		exit(1);
	}
	printids("main thread:");

	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;	/* 初始化客户端的数据 */

	FD_ZERO(&allset);//清空fd_set相应位
	FD_SET(listenfd, &allset);//设置监听位

    for ( ; ; )
	{
		rset = allset;	/* structure assignment */
//		printf("nready = %d\n",nready );
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);//	dusai wait for client connect it
//		printf(" select maxfd = %d\n ",maxfd);
//		printf("nready = %d\n",nready );
		if (nready < 0)
			perr_exit("select error");

		if (fd_ssetn  = FD_ISSET(listenfd, &rset))
		{ /* new client connection */
			printf("FD_ISSET = %d\n",fd_ssetn );


//            travse_node();
			cliaddr_len = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);//record connfd from Client
			inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));//change commo in point,minute biaoshi

//			printf("received from %s at PORT %d\n",
//			       str,
//			       ntohs(cliaddr.sin_port));

			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0) //save clinet connfd
				{
					client[i] = connfd; /* save descriptor */
					break;
				}
			}
			// if too much  exit
			if (i == FD_SETSIZE) {
				fputs("too many clients\n", stderr);
				exit(1);
			}

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd; /* for select */
			if (i > maxi)
				maxi = i;	/* max index in client[] array */

			//printf("FD_ISSET nready = %d\n",nready);
			if (--nready == 0)
			{
				continue;	/* no more readable descriptors */
			}

		}

/* 检查任何一个已连接的设备发送的数据 */
		for (i = 0; i <= maxi; i++)
		{	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset))// look if sockfd 's read bit is set,success
			{
			    memset(readbuf, 0,MAXLINE);//接受数据前清空缓冲区
				if ( (n = Read(sockfd, readbuf, MAXLINE)) == 0)//在这里接受数据
				{
					/* connection closed by client */
					printf("close  sockfd = %d\n",fd_ssetn );
					travse_node();
					p = Find_node(sockfd);//删除这个节点
					if(p == NULL)
					{
					    printf("find node is error\n");
                        exit(1);
					}
					Remove_node(p);
					travse_node();
					Close(sockfd);
					FD_CLR(sockfd, &allset);//clear sockfd all bit ;
					client[i] = -1;//
				} else //主要在这里发送数据
				{
                    readbuf_msg.ID = sockfd ;//保存下这socket端口号
                    if ((p = Find_node(sockfd)) == 0)//未找到这个节点重新插入这个节点
                    {
                        printf("new sockd is %d \n",sockfd);
                        p = create_node(sockfd);//插入这个节点
                        Insert_node(p);
                       // travse_node();//打印这个链表

                    }
                    printf("DATA IS %s,sendbuf_msg is %s\n",readbuf,sendbuf_msg.buffer);

                    Insert(readbuf,sockfd,&readbuf_msg);
                    if (strlen(sendbuf_msg.buffer) != 0)//判断发送数据缓冲是否非空
                    {
                        printf("sockfd is  %d,sendmsg.id is %d,data is  %s\n",sockfd,sendbuf_msg.ID,sendbuf_msg.buffer);//发回返回的数据以及sock编号
                        if(sendbuf_msg.ID == sockfd)
                        {
                         //   printf("found sockfd\n");
                            printf("send normal data is %s\n",sendbuf_msg.buffer);
                            Write(sockfd, sendbuf_msg.buffer, strlen(sendbuf_msg.buffer));//发完后清楚内容
                            msg_status = 1;//设置清楚标志

                        }else
                        {
                             printf("send default data is %s\n",normalsenddata);
                             Write(sockfd, normalsenddata, strlen(normalsenddata));//发送默认的数据
                        }

                    }else
                    {
                        printf("send default data is %s\n",normalsenddata);
                        Write(sockfd, normalsenddata, strlen(normalsenddata));
                    }

				}


				if (--nready == 0)
				{
//					printf("MAXI nready = %d\n",nready);
					break;	/* no more readable descriptors */
				}
//				printf("MAXI nready == %d\n",nready);
			}
//			sleep(1);
		}
	}

	return 0;
}
//初始化服务器
void init_net(int *listenfd,struct sockaddr_in *servaddr,int *opt)
{
	*listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(servaddr, sizeof(*servaddr));
	(*servaddr).sin_family      = AF_INET;
	(*servaddr).sin_addr.s_addr = htonl(INADDR_ANY);
	(*servaddr).sin_port        = htons(SERV_PORT);

	setsockopt(*listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(*opt));//setsockopt

	printf("bing start\n");
	Bind(*listenfd, (struct sockaddr *)servaddr, sizeof(*servaddr));
	printf("bing success\n");

	Listen(*listenfd, 20);
	printf("listenfd success\n");
}
//对接收到的数据解析，然后存放到数据库中，再把数据发回。
