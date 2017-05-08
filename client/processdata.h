#ifndef PROCESSDATA_H_
#define PROCESSDATA_H_
#include "Message.h"
#include <string.h>

#define MSSGEMAX 1024

void processData(char * msg,chatMssage * cmsg);
void writeDataBuffer(chatMssage * cmsg,char * msg,int len);

#endif//PROCESSDATA_H_
