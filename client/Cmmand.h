#ifndef CMMAND_H_
#define CMMAND_H_
#include "Message.h"
#include "processdata.h"
void processAnswer(char*sockWriteBuf,chatMssage *chat);
void selectCmmand(int msg_command,chatMssage *chat,char*sockWriteBuf,char *user);//ID 为当前用户的编号
void print2arrar(char *user);
void init2arrar(char *user,int size);
void processLOOK(char*sockWriteBuf,char *user,chatMssage *chat);
void processsend(char*msg,chatMssage *chat);
#endif//CMMAND_H_
