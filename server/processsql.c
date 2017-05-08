#include "processsql.h"

MYSQL *g_conn; // mysql 连接
MYSQL_RES *g_res; // mysql 记录集
MYSQL_ROW g_row;// 字符串数组，mysql 记录行

#define MAX_BUF_SIZE 1024 // 缓冲区最大字节数

const char *g_host_name = "localhost";
const char *g_user_name = "root";
const char *g_password = "123456";
const char *g_db_name = "chatsample";
static char sql[MAX_BUF_SIZE];
const unsigned int g_db_port = 3306;

void print_mysql_error(const char *msg)
{
    // 打印最后一次错误
    if (msg)
        printf("%s: %s\n", msg, mysql_error(g_conn));
    else
        puts(mysql_error(g_conn));
}

int executesql(const char * sql)
{
    /*query the database according the sql*/
    if (mysql_real_query(g_conn, sql, strlen(sql))) // 如果失败
        return -1; // 表示失败

    return 0; // 成功执行
}


int init_mysql()
{
    // 初始化连接
    // init the database connection
    g_conn = mysql_init(NULL);

    /* connect the database */
    if(!mysql_real_connect(g_conn, g_host_name, g_user_name, g_password, g_db_name, g_db_port, NULL, 0)) //如果失败
        return -1;

    // 是否连接已经可用
    if (executesql("set names utf8")) // 如果失败
        return -1;

    return 0; // 返回成功
}

int InsertRegiseSql(char*name,char*password)
{
        memset(sql,0,MAX_BUF_SIZE);
        sprintf (sql,"insert into Register values ('%s','%s','null','null')",name,password);
        if (executesql(sql))
        {
                print_mysql_error(NULL);
                exit(1);
        }
        return 1;
}

int SearchRegiseSql(char*table,const char *loginname,const char *loginpassword)
{
    int loginflag = 0;
    memset(sql,0,MAX_BUF_SIZE);
    sprintf (sql,"SELECT * FROM %s",table);
    if (executesql(sql))
    {
        print_mysql_error(NULL);
        exit(1);
    }
    g_res = mysql_store_result(g_conn); // 从服务器传送结果集至本地，mysql_use_result直接使用服务器上的记录集
    int iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
    while ((g_row=mysql_fetch_row(g_res))) // 打印结果集
    {
        if (strcmp(loginname,g_row[0]) == 0)
        {
            if (strcmp(loginpassword,g_row[1]) == 0)
            {
                loginflag = 1;
                break;
            }
        }
    }
    mysql_free_result(g_res); // 释放结果集
    mysql_close(g_conn); // 关闭链接
    return loginflag;
}
//int main(void)
//{
//    puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
//
//    if (init_mysql());
//    print_mysql_error(NULL);
//
//
////    sprintf(sql, "create table course(SUBJECT varchar(5),\
////                    Systems varchar(20),\
////                    dept_name varchar(20),\
////                    number varchar(20),\
////                    primary key (SUBJECT),\
////                    ");
////    if (executesql(sql))
////        print_mysql_error(NULL);
//
//    sprintf(sql, "insert into course values ('CS-47', 'Database Systems', 'Comp. Sci.', 5)");
//
//    if (executesql(sql))
//        print_mysql_error(NULL);
//    printf("over\n");
//    if (executesql("SELECT * FROM course")) // 句末没有分号
//        print_mysql_error(NULL);
//
//    g_res = mysql_store_result(g_conn); // 从服务器传送结果集至本地，mysql_use_result直接使用服务器上的记录集
//
//    int iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
//    int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
//
//    printf("共%d个记录，每个记录%d字段\n", iNum_rows, iNum_fields);
//
//    puts("id\tname\n");
//
//    while ((g_row=mysql_fetch_row(g_res))) // 打印结果集
//        printf("%s\t%s\n", g_row[0], g_row[1]); // 第一，第二字段
//
//    mysql_free_result(g_res); // 释放结果集
//
//    mysql_close(g_conn); // 关闭链接
//
//    return EXIT_SUCCESS;
//}

