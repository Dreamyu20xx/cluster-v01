#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define false 0
#define true 1

typedef enum
{
    EMPTY = 0,
    LOGIN ,
    REGISTER,
    SEND,
    LOOK,
    LOOKBEFORE,
    ANSEWER,//服务器对客户端的发送响应数据
    RESPONSE//客户端向服务段响应接受到数据
}COMMAND;

typedef struct
{
    COMMAND chat_command;
    int chat_valid;
    char chat_sendName[20];
    char chat_receiveName[20];
    char chat_message[1024];

}chatMssage;

void Initchat(chatMssage* chatone);
void printChat(chatMssage chatsombody);


#endif //MESSAGE_H_
