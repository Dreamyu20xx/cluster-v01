/*
============================================================================
Name : processsql.h
Author :
Version :
Copyright : Your copyright notice
Description : Hello World in C, Ansi-style
Addition: when you compile ,you must add lmysqlclient
============================================================================
*/
#ifndef _PROCESSSQL_H_
#define _PROCESSSQL_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>
void print_mysql_error(const char *msg);
int executesql(const char * sql);
int init_mysql();
int InsertRegiseSql(char*name,char*password);

#endif//_PROCESSSQL_H_
