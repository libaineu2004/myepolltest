#ifndef IPC_SEM_H
#define IPC_SEM_H

/*
 * http://blog.csdn.net/libaineu2004/article/details/76540022
信号量其实是一个计数器，用于计算当前的资源是否可用。为了获得共享资源，并且保持数据的同步，我们可以这样来使用信号量：
1)测试控制该资源的信号量
2)若此信号量的值为正，则进程可以使用该资源。对信号量做减1操作，表示他占用了这个资源
3)若此信号量为的值0，则表示该资源已被占用。进程挂起等待，直到占用资源的进程释放该资源，使信号量的值为正。此时进程被唤醒，又返回至第1）步
通常信号量的值可以取任意正值，代表有多少个共享的资源。但一般情况下，我只使用0和1两个值，分别代表某一个资源的占用与空闲，这也叫做二进制信号量。

信号量集是包含一个或多个信号量的集合，但通常我们只使用含有一个信号量的信号量集。
*/

#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#define FTOK_FILE "/etc/profile"

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

class CIPC_Sem
{
public:
    CIPC_Sem(){;}
    ~CIPC_Sem(){;}

public:
    int create_sem(void);//创建信号量
    int delete_sem(int semid);//删除信号量
    int set_semvalue(int semid);//初始化信号量。在使用之前必须先初始化
    int sem_p(int semid);//占用资源，即执行p操作（p操作是用于描述获取信号量的术语）
    int sem_v(int semid);//释放资源，即v操作（v操作适用于描述释放信号量的术语）
};

#endif // IPC_SEM_H
