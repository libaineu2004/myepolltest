#include "ipc_sem.h"

int CIPC_Sem::create_sem(void)
{
    int semid = 0;
    key_t key;

    //系统建立IPC通讯 （消息队列、信号量和共享内存） 时必须指定一个ID值。通常情况下，该id值通过ftok函数得到.
    //同一段程序，用于保证两个不同用户下的两组相同程序获得互不干扰的IPC键值.
    key = ftok(FTOK_FILE, 11);
    if (key == -1)
    {
        printf("%s : key = -1!\n",__func__);
        return -1;
    }

    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1)
    {
        printf("%s : semid = -1!\n",__func__);
        return -1;
    }

    return semid;
}

int CIPC_Sem::delete_sem(int semid)
{
    if (semctl(semid, 0, IPC_RMID) == -1)
    {
        printf("%s : can't rm the sem!\n",__func__);
        return -1;
    }

    return 0;
}

int CIPC_Sem::set_semvalue(int semid)
{
    union semun sem_arg;
    sem_arg.val = 1;

    if (semctl(semid, 0, SETVAL, sem_arg) == -1)//若只有一个信号量则取0即可
    {
        printf("%s : can't set value for sem!\n",__func__);
        return -1;
    }

    return 0;
}

int CIPC_Sem::sem_p(int semid)
{
    struct sembuf sem_arg;
    sem_arg.sem_num = 0;
    sem_arg.sem_op = -1;//操作中改变的数值，+1表示释放资源，-1表示占用资源
    sem_arg.sem_flg = SEM_UNDO;

    if (semop(semid, &sem_arg, 1) == -1)
    {
        printf("%s : can't do the sem_p!\n",__func__);
        return -1;
    }

    return 0;
}

int CIPC_Sem::sem_v(int semid)
{
    struct sembuf sem_arg;
    sem_arg.sem_num = 0;
    sem_arg.sem_op = +1;//操作中改变的数值，+1表示释放资源，-1表示占用资源
    sem_arg.sem_flg = SEM_UNDO;

    if (semop(semid, &sem_arg, 1) == -1)
    {
        printf("%s : can't do the sem_v!\n",__func__);
        return -1;
    }

    return 0;
}
