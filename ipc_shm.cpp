#include "ipc_shm.h"

CIPC_Shm::CIPC_Shm()
{
    m_pShm_addr = NULL;
}

int CIPC_Shm::create_shm(int maxMqCount)
{
    /*获取信号量标识符*/
    m_iSemid = m_sem.create_sem();
    if (m_iSemid == -1)
    {
        printf("%s : semid = %d!\n", __func__, m_iSemid);
        return -1;
    }

    /*创建信号量之后的，初始化操作*/
    if (m_sem.set_semvalue(m_iSemid))
    {
        printf("%s : set_semvalue failed!\n", __func__);
        return -1;
    }

    /*获取共享内存标识符*/
    int size = sizeof(cur_t) + sizeof(elm_t) * maxMqCount;
    m_iShmid = shmget(ftok(FTOK_FILE, 111), size, IPC_CREAT | 0666);
    if (m_iShmid == -1)
    {
        printf("%s : shmid = %d!\n", __func__, m_iShmid);
        //perror("shmget");
        fprintf (stderr, "error: %s\n",strerror(errno));
        fprintf (stderr, "error: %d\n",errno);
        //cat /proc/sys/kernel/shmmax
        //ipcs -a
        //ipcrm -m 622595
        //http://blog.csdn.net/libaineu2004/article/details/76919711
        return -1;
    }

    /*当前进程连接该共享内存段*/
    m_pShm_addr = shmat(m_iShmid, 0 , 0);
    if (m_pShm_addr == (void *)-1)
    {
        printf("%s : pshm_addr == (void*)0!\n", __func__);
        return -1;
    }

    cur_t *p1 = (cur_t *)m_pShm_addr;
    m_queue.setCursor(p1);

    elm_t *p2 = (elm_t *)(m_pShm_addr + sizeof(cur_t));
    m_queue.setQueueHead(p2);
    m_queue.setMaxCount(maxMqCount);

    printf("read process semid is %d, shmid is %d!\n", m_iSemid, m_iShmid);

    return 0;
}

int CIPC_Shm::delete_shm(void)
{
    /*删除信号量*/
    if (m_sem.delete_sem(m_iSemid))
    {
        printf("%s : del_sem failed!\n", __func__);
        return -1;
    }

    /*进程和共享内存脱离*/
    if (shmdt(m_pShm_addr) == -1)
    {
        printf("%s : shmdt failed!\n", __func__);
        return -1;
    }

    /*删除共享内存*/
    if (shmctl(m_iShmid, IPC_RMID, 0) == -1)
    {
        printf("%s : shmctl failed!\n", __func__);
        return -1;
    }

    printf("BYE!\n");
    return 0;
}

int CIPC_Shm::puch_data(elm_t t)
{
    /*占用信号量，p操作*/
    if (m_sem.sem_p(m_iSemid))
    {
        printf("%s : sem_p failed !\n", __func__);
        return -1;
    }

    int ret = 0;
    if (m_queue.enQueue(t))
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    /*释放信号量，v操作*/
    if (m_sem.sem_v(m_iSemid))
    {
        printf("%s : sem_v failed!\n", __func__);
        return -1;
    }

    return ret;
}

int CIPC_Shm::pop_data(elm_t *t)
{
    /*占用信号量，p操作*/
    if (m_sem.sem_p(m_iSemid))
    {
        printf("%s : sem_p failed !\n", __func__);
        return -1;
    }

    int ret = 0;
    if (m_queue.outQueue(t))
    {
        ret = 0;
    }
    else
    {
        ret = ELMT_EMPTY;
    }

    /*释放信号量，v操作*/
    if (m_sem.sem_v(m_iSemid))
    {
        printf("%s : sem_v failed!\n", __func__);
        return -1;
    }

    return ret;
}
