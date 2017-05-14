#include "processdata.h"


void processData(char * msg,chatMssage * cmsg)
{
    char chatbuffer[MSSGEMAX];
    int chatlen = strlen(msg);
    int i ,j,k = 0;
//    printf("len is %d\n",chatlen);
    Initchat(cmsg);
    if (msg[0] == '$'&&msg[1] == '@'&&msg[chatlen - 2] == '@'&&msg[chatlen - 1] == '$')
    {
     //   printf("recieve some data\n");
        cmsg ->chat_valid = msg[2] - 48;

        if (cmsg ->chat_valid == 0)
        {
            return;
        }
        i = 3;
        j = 3;
        while(i < chatlen - 2)
        {
            if (msg[i] == '$')
            {
                switch (k)
                {
                    case 0:cmsg->chat_command = msg[i - 1] -48;k++;j = i;break;
                    case 1:strncpy(cmsg->chat_sendName,msg + j+1,i-j-1);k++;j = i;break;
                    case 2:strncpy(cmsg->chat_receiveName,msg + j+1,i-j-1);k++;j = i;break;
                    case 3:strncpy(cmsg->chat_message,msg + j+1,i - j-1);k++;j = i;break;
                }
            }
            i++;
        }
    }

}

void writeDataBuffer(chatMssage * cmsg,char * msg,int len)
{
    memset(msg,0,len);
    msg[0] = '$';
    msg[1] = '@';
    msg[2] = cmsg ->chat_valid + 48;
    msg[3] = cmsg->chat_command + 48;
    msg[4] = '$';
    
    strcat(msg,cmsg->chat_sendName);
    strcat(msg,"$");
    strcat(msg,cmsg->chat_receiveName);
    strcat(msg,"$");
    strcat(msg,cmsg->chat_message);
    strcat(msg,"$@$");
}
