#ifndef CMMAND_H_
#define CMMAND_H_
#include "Message.h"
#include "processdata.h"
#include "processsql.h"
#include "msgclient.h"

void selectCmmand(int msg_command, chatMssage *chatrece,chatMssage *chatsed,int ID);
void processRegister( chatMssage *chatrece,int ID);
void processLogin(chatMssage *chatrece,int ID);
void processResponse(chatMssage *chatrece);
void processLook(chatMssage *chatrece,chatMssage *chatsed);
void processSEND(chatMssage *chatrece,chatMssage *chatsed);
#endif//CMMAND_H_
