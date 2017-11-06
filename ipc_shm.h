#ifndef IPC_SHM_H
#define IPC_SHM_H

/*
共享内存允许几个不同的进程共享同一给定的存储区。当有一个进程往共享内存里写入数据，
其他连接同一段共享内存的进程将立刻观察到该段内存里内容的改变。但是这也带来了一个问题：
当一个进程往共享内存里写数据的时候，如果没有一个保护措施，那么另一个连接该共享内存的进程
也可以对该共享的内容做读写操作，而这样的读写操作必然会带来数据的混乱。因此我们在编写使用共享内存的进程时，
必须注意数据的同步，通常我们是利用之前介绍的信号量来实现的。
*/

#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include "ipc_sem.h"
#include "ds_queue.h"

class CIPC_Shm
{
public:
    CIPC_Shm();
    ~CIPC_Shm(){;}

public:
    int create_shm(int maxMqCount);//创建共享内存
    int delete_shm(void);//删除共享内存
    int puch_data(elm_t t);
    int pop_data(elm_t *t);

private:
    CIPC_Sem m_sem;
    CDS_Queue m_queue;
    int m_iSemid;
    int m_iShmid;
    void *m_pShm_addr;
};

#endif // IPC_SHM_H
