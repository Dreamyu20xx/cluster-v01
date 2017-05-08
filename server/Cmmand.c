#include "Cmmand.h"

void selectCmmand(int msg_command, chatMssage *chatrece,chatMssage *chatsed,int ID)//ID 为当前用户的编号
{
    switch(msg_command)
    {
        case LOGIN:processLogin(chatrece,ID);break;
        case REGISTER:processRegister(chatrece,ID);break;
        case SEND:processSEND(chatrece,chatsed);break;
        case LOOK:processLook(chatrece,chatsed);break;
        case LOOKBEFORE:break;
        case RESPONSE:processResponse(chatrece);break;
    }
}
//对获取到的信息切割获取并上传到数据库 *****\n******\0
void  processRegister(chatMssage *chatrece,int ID)
{
    char chatname[10] = {0};
    char chatpassword[1024] = {0};
    int res = 0;
    node*pId;
    printChat(*chatrece);
    strcpy(chatname,chatrece->chat_sendName);
    strcpy(chatpassword,chatrece->chat_message);
    init_mysql();
    res = InsertRegiseSql( chatname,chatpassword);//插入成功
    if (res == 1)
    {
        printf("注册成功\n");
        pId = Find_node(ID);
        if(pId == NULL)
        {
            printf("find not \n");
            exit(1);
        }
        Insertclientname(pId,chatname);
    }
}
//对获取到的登陆信息与数据库上的信息进行比对
void processLogin(chatMssage *chatrece,int ID)
{
    char chatname[10] = {0};
    char chatpassword[1024] = {0};
    int res;
    node*pId;
    printChat(*chatrece);
    strcpy(chatname,chatrece->chat_sendName);
    strcpy(chatpassword,chatrece->chat_message);
    init_mysql();
    res = SearchRegiseSql("Register",chatname,chatpassword);
    if (res == 1)
    {
        printf("登陆成功\n");
        pId = Find_node(ID);//赋值这个节点
         travse_node();
        if(pId == NULL)
        {
            printf("find not \n");
            exit(1);
        }
        Insertclientname(pId,chatname);
    }else{
        printf("登陆失败\n");
    }
}

void processResponse(chatMssage *chatrece)
{
    printf("receive %s response\n",chatrece->chat_sendName);
}

void processLook(chatMssage *chatrece,chatMssage *chatsed)//察看当前用户
{
    chatsed->chat_valid = 1;
    chatsed->chat_command = LOOK;
    strcpy(chatsed->chat_sendName,"admin");
    strcpy(chatsed->chat_receiveName,chatrece->chat_sendName);
    sinert(chatsed->chat_message,1024);//传出察看信息
    printf("In processLOOK ,msg is ");
    printChat(*chatsed);
}
void processSEND(chatMssage *chatrece,chatMssage *chatsed)//私聊某人
{
    chatsed->chat_valid = 1;
    chatsed->chat_command = SEND;
    strcpy(chatsed->chat_sendName,chatrece->chat_sendName);
    strcpy(chatsed->chat_receiveName,chatrece->chat_receiveName);
    strcpy(chatsed->chat_message,chatrece->chat_message);
    printChat(*chatsed);
}
