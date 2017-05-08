#ifndef PROCESSDATA_H_
#define PROCESSDATA_H_
#include "Message.h"
#include <string.h>

#define MSSGEMAX 1024

#define MAXLINE 80
typedef struct
{
    int ID;
    char buffer[MAXLINE] ;//客户端向服务器发送的数据
} addmsg ;

void processData(char * msg,chatMssage * cmsg);
void writeDataBuffer(chatMssage * cmsg,char * msg,int len);
int SearchRegiseSql(char*table,const char *loginname,const char *loginpassword);
void Initdata(addmsg *add);
void Insert(char*readbuf,int client,addmsg *add);
#endif//PROCESSDATA_H_
