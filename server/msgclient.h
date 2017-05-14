#ifndef _MSGCLIENT_H_
#define _MSGCLIENT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct node
{
    int clientnumber;
    char clientname[10];
    struct node *next;
}node;

node*  create_node(int nature);
void Insert_node(node *p);
void Free_node(node *p);
node* Find_node(int nature);
void Insertclientname(node*p,char*name);
void Remove_node(node*p);
void Remove();
void travse_node();
void sinert(char*contailer,int num);//把相关数据写到内存中
int find_clientname_node(char*name);//返回响应的sock
int sock_flash(int*nflash,int len);//把sock编号写到内存中
int Isockfd(int *arrar,int fd,int len);//判别sock的编号
#endif
