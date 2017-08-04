/**
 * Copyright (C), 2016-2017, RenWeiGuo
 * FileName: cluster_main.c
 * Author:      RenWeiGuo  Version :  1.0        Date: 2017.7.24
 * Description:     创建n个reactor模式，并实现一万多个客户链接，并实现打印信息
 * Version:         1.0  实现啦一万个用户链接，读事件放在线程池里读，没做数据处理部分，没加超时时间检测部分
 * Function List:
    1. -------
 * History:
      <author>  <time>   <version >   <desc>
      RenWeiiGuo    17/07/24     1.0     build this moudle
      RenWeiiGuo    17/08/04     1.0     rebuild this moudle

 */
#include "cluster_event.h"

int main()
{
	system_Init();// 初始化成功
	while (1)
	{
		sleep(5);
	}
    return 0;
}

