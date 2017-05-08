#include "msgclient.h"

static node*head = NULL;
node*  create_node(int nature)
{
    node *p = (node*)malloc(sizeof(node));

    if (p == NULL)
    {
        printf("new is failed!\n");
        exit(1);
    }else{
        p->clientnumber = nature;
        memset(p->clientname,0,10);
        p->next = NULL;
    }
    return p;
}

void Insert_node(node *p)
{
    p->next = head;
    head = p;
}

void Free_node(node *p)
{
    if (p != NULL)
    {
         free(p);
    }
}

node* Find_node(int nature)
{
    node *pre = head;

    while (pre != NULL)
    {
        if (pre->clientnumber == nature)
        {
            return pre;
        }
        pre = pre->next;
    }
    return NULL;
}

int find_clientname_node(char*name)
{
    node*pre = head;
    while (pre != NULL)
    {
        if(strcmp(pre->clientname,name) == 0)
        {
            return pre->clientnumber;
        }
        pre = pre->next;
    }
    return -1;
}

void Insertclientname(node*p,char*name)
{
    if (strlen(p->clientname) == 0)
    {
        strcpy(p->clientname,name);
    }
}

void Remove_node(node*p)
{
    node *pre = head;

    if (head == p)
    {
        head = p -> next;
        Free_node(p);
        return ;
    }

    while (pre != NULL)
    {
        if (pre -> next == p)
        {
            pre -> next = p -> next;
            Free_node(p);
            return;
        }
        pre = pre->next;
    }
}
void Remove()
{
    node *pre = head;
    while (head != NULL)
    {
        pre = head->next;
        Free_node(head);
        head = pre;
    }
}
void travse_node()
{
    node *pre = head;
    while (pre != NULL)
    {
         printf("number = %d name = %s",pre->clientnumber,pre->clientname);
         pre = pre->next;
    }
    printf("\n");
}
void sinert(char*contailer,int num)
{
    char name[10];
    node*pre = head;

    memset(contailer,0,num);
    travse_node();
    while(pre != NULL)
    {
        memset(name,0,10);
        strcpy(name,pre->clientname);
        strcat(name,"\n");
        strcat(contailer,name);
        pre = pre->next;
    }
    printf("data is %s",contailer);
}


