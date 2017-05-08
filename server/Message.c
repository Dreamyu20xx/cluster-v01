#include "Message.h"

void Initchat(chatMssage* chatone)
{
    chatone->chat_valid = 0;
    memset(chatone->chat_sendName,0,20);
    memset(chatone->chat_receiveName,0,20);
    memset(chatone->chat_message,0,1024);
    chatone->chat_command = EMPTY;
}
void printChat(chatMssage chatsombody)
{
    printf("volid is %d\n",chatsombody.chat_valid);
    printf("cmd is %d\n",chatsombody.chat_command);

    printf("send is %s \nreceive is %s \n",chatsombody.chat_sendName,chatsombody.chat_receiveName);
    printf("Data is %s\n",chatsombody.chat_message);

}
