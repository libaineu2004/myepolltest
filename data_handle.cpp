#include "data_handle.h"
#include "modbus_data.h"

CData_Handle::CData_Handle(int maxMqCount)
{
    m_shm.create_shm(maxMqCount);
}

CData_Handle::~CData_Handle()
{
    m_shm.delete_shm();
}

void CData_Handle::do_start(void)
{
    CModbus_Data::init();

    //一开始先延时，等待客户机连接
    sleep(10);

    int ret = 0;
    elm_t t;

    while (true)
    {
        sleep(5);//seconds

        //先从共享内存取出需要处理的接收数据
        while (true)
        {
            ret = m_shm.pop_data(&t);
            if (ret == ELMT_EMPTY)
            {
                printf("ok,well done,no data now!\n");
                break;
            }
            else
            {
                for (int i = 0; i < t.len; i++)
                {
                    fprintf(stderr, "%02X ", t.buf[i]);
                }

                printf(",pop_data,fd=%d\n", t.fd);
            }
        }

        //再准备好待发送的数据

        //用于通知子进程，可以发送数据了
        CIPC_Pipe::getInstance()->write_pipe();

    }

    CIPC_Pipe::getInstance()->close_write_fd();
}
