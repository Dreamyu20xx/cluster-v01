/* client.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "wrap.h"
#include "processdata.h"
#include "Message.h"
#include "logfile.h"
#include <fcntl.h>
#include <pthread.h>
#include "Cmmand.h"

#define MAXLINE 80
#define SERV_PORT 8000
void islogin(int loginflag);//登陆或者注册信息
void FindUser(void);//察看用户目录
void sendmessage(void);//发送消息
pthread_t ntid;
char sendbuf[MAXLINE] = "I am ready!\n";//客户端向服务器发送的数据
volatile char readbuf[MAXLINE] ;//服务器向客户端发送的数据
chatMssage chatclient; //用于解析数据
currentuserlog logclient;//当前程序的用户
volatile int msgse_flag = 0;//是否决定发送数据
user_register userone;//注册或登陆的当前信息
int msgre_flag = 1;
int msg_status = 0;//判断是否发送数据成功，成功 1，不成功 0；
char normalsenddata[] ="$@00$$$@$";//默认发送或回应的数据
char username_arrar[100];
char msgrecenote[MAXLINE];//处理来自别人发过来的信息


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


//创建新的线程
void *thr_fn(void *arg)
{
	struct sockaddr_in servaddr;

	int sockfd, n;
	int flags;

	printids(arg);
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if((flags = fcntl(sockfd,F_GETFD,0)) == -1)
        {
           printf("read sockfd failed\n");
          exit(1);
       }else{
           flags |= O_NONBLOCK;
           fcntl(sockfd,F_SETFD,&flags);

       }

	while (1)
	{
		//发送标志
	    if (msgse_flag == 0)//发送默认的数据
	    {
//			printf("send some normal data is %s \n",normalsenddata);
	       Write(sockfd, normalsenddata, 9);
	    }else if(msgse_flag == 1)//发送处理的数据
	    {
			printf("send some process data is %s\n",sendbuf);
	       Write(sockfd, sendbuf, strlen(sendbuf));
	    }
		memset(readbuf,0,MAXLINE);
		n = Read(sockfd, readbuf, MAXLINE);//if foreign command interrupt ,just try again


		if (n > 0)
		{
//		    printf("接受到的数据 is %s\n",readbuf);
		}
		sleep(1);

	}

	Close(sockfd);

return NULL;

}

//创建新的线程
void *thr_f2(void *arg)
{
	printids(arg);//打印自己的线程号
	while (1)
	{
        
        processData(readbuf,&chatclient);
           	
		if (chatclient.chat_valid == 1&&chatclient.chat_command == SEND)//数据是否有效
		{ 
			printChat(chatclient);
           	selectCmmand(chatclient.chat_command, &chatclient,msgrecenote,NULL);//从数据的命令位处理这些数据，并做出相应的动作				
			if (chatclient.chat_command == SEND)//如果服务器成功接受到来自客户端的数据
			{
				printf("username is %s\t,receivename is %s \n",logclient.username,chatclient.chat_receiveName);
				if (strcmp(logclient.username,chatclient.chat_receiveName) == 0)//再次确认是不是给自己发送的信息
				{
					printf("%s\n",msgrecenote);//打印一下发送者发回的信息
					memset(readbuf,0,MAXLINE);//清除接收
				}

									
			}
		
		}	
		sleep(1);
	}
return NULL;

}
int main(int argc, char *argv[])
{

    char res;
    int err;//进程创建成功的标志

    Initchat(&chatclient);//初始化信息结构体

	err = pthread_create(&ntid, NULL, thr_fn, "new thread: ");
	if (err != 0)
	{
		fprintf(stderr, "can't create thread: %s\n", strerror(err));
		exit(1);
	}
    printids("main thread\n");
    //第一步，发送注册信息 或者登陆信息
    printf("请问你是新用户吗。y or n\n");
    if ((res=getchar())== 'y')
    {
         islogin(1);

    }else if (res == 'n')
    {
         islogin(2);

    }else
    {
         printf("select error\n");
         exit(1);
    }
    while(1)
    {
    	printf("please input cammand number\n1.查看当前用户\n2.私聊\n3群发\n");
    	if ((res = getchar()) == '1')
    	{
    		getchar();//消除\n的影响
    		FindUser();
    	}else if (res == '2')
    	{
    		getchar();//消除\n的影响
    		sendmessage();
    	}else if (res == '3')
    	{
    		getchar();//消除\n的影响
   		}
    
    }


	return 0;
}


void islogin(int loginflag)
{
    int Issucess = 0;
    int i = 0;
    
    int err;//进程创建成功的标志

    memset(userone.username,0,10);//初始化
    memset(userone.password,0,20);
    memset(logclient.username,0,10);//初始化
    memset(logclient.password,0,20);
    getchar();
    printf("username : \n");
    fgets(userone.username,20,stdin);
    while (userone.username[i] != '\n')i++;
    userone.username[i] = '\0';
    printf("password : \n");
    fgets(userone.password,10,stdin);
    i = 0;
    while (userone.password[i] != '\n')i++;
    userone.password[i] = '\0';
    strcpy(logclient.username,userone.username);
    strcpy(logclient.password,userone.password);
    printf ("username :%s\n password :%s\n",userone.username, userone.password);

	if(loginflag == 1)//注册
	{
        chatclient.chat_command = REGISTER;
        chatclient.chat_valid = 1;
        strcpy(chatclient.chat_sendName,userone.username);
        strcpy(chatclient.chat_receiveName,"admin");
        strcpy(chatclient.chat_message,userone.password);
        writeDataBuffer(&chatclient,sendbuf,MAXLINE);
        printChat(chatclient);
        printf("send data is %s",sendbuf);
        msgse_flag = 1;//表示可以发送处理的数据

	}else if (loginflag == 2) //登陆
	{
        chatclient.chat_command = LOGIN;
        chatclient.chat_valid = 1;
        strcpy(chatclient.chat_sendName,userone.username);
        strcpy(chatclient.chat_receiveName,"admin");
        strcpy(chatclient.chat_message,userone.password);
        writeDataBuffer(&chatclient,sendbuf,MAXLINE);
        printChat(chatclient);
        printf("send data is %s",sendbuf);
        msgse_flag = 1;//表示可以发送处理的数据
	}
	
	while (Issucess != 1)
    {

        if (msgre_flag == 1)
        {
            sleep(1);
            processData(readbuf,&chatclient);
           	

			if (chatclient.chat_valid == 1)//数据是否有效
			{ 
				printChat(chatclient);
           		selectCmmand(chatclient.chat_command, &chatclient,sendbuf,NULL);//从数据的命令位处理这些数据，并做出相应的动作				
				if (chatclient.chat_command == ANSEWER)//如果服务器成功接受到来自客户端的数据
				{
					msgse_flag = 1;//发送处理之后的数据
					sleep(5);//等待发送成功把
					msgse_flag = 0;//发送默认的数据

					err = pthread_create(&ntid, NULL, thr_f2, "new thread: ");//建一个新线程用来处理来自私聊或群发的信息
					if (err != 0)
					{
						fprintf(stderr, "can't create thread: %s\n", strerror(err));
						exit(1);
					}				
					Issucess = 1;
									
				}
		
			}
        }

    }
}

void FindUser(void)
{
	int Issucess = 0;
	chatclient.chat_command = LOOK;
   	chatclient.chat_valid = 1;
    strcpy(chatclient.chat_sendName,userone.username);
    strcpy(chatclient.chat_receiveName,"admin");
    strcpy(chatclient.chat_message," ");
    writeDataBuffer(&chatclient,sendbuf,MAXLINE);
    printChat(chatclient);
    printf("send data is %s",sendbuf);
    msgse_flag = 1;//表示可以发送处理的数据
    
	while (Issucess != 1)
    {

        if (msgre_flag == 1)
        {
            sleep(1);
            processData(readbuf,&chatclient);
           	

			if (chatclient.chat_valid == 1)//数据是否有效
			{ 
				printChat(chatclient);
           					
				if (chatclient.chat_command == LOOK)//如果服务器成功接受到来自客户端的数据
				{
					selectCmmand(chatclient.chat_command, &chatclient,sendbuf,username_arrar);//从数据的命令位处理这些数据，并做出相应的动作	
					msgse_flag = 1;//发送处理之后的数据
					sleep(5);//等待发送成功把
					msgse_flag = 0;//发送默认的数据
					Issucess = 1;
									
				}
		
			}
        }

    }
}


void sendmessage(void)
{
	int number ;
	int Issucess = 0;
	char msg[MAXLINE] = "beautiful";
	printf("please input visit client number\n");
	
	number = getchar() - 49;
	getchar();
	chatclient.chat_command = SEND;
   	chatclient.chat_valid = 1;	
	strcpy(chatclient.chat_sendName,userone.username);
	strcpy(chatclient.chat_receiveName,&(username_arrar[number*10]));
	strcpy(chatclient.chat_message,msg);
    writeDataBuffer(&chatclient,sendbuf,MAXLINE);
    printChat(chatclient);	
    msgse_flag = 1;//表示可以发送处理的数据
    sleep(5);
    msgse_flag = 0;//表示 不可以发送处理的数据

}




















