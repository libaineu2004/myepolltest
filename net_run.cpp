#include "net_run.h"
#include "modbus_data.h"

extern bool g_bStop;

CNet_Run::CNet_Run(char* ip, int port, int maxConn, int maxSocketBuf, int maxMqCount) : m_Epl(false), m_session(maxSocketBuf)
{
    m_cIP = ip;
    m_iPort = port;
    m_iMaxConn = maxConn;
    m_iMaxSocketBuf = maxSocketBuf;
    m_shm.create_shm(maxMqCount);
}

CNet_Run::~CNet_Run()
{
    m_shm.delete_shm();
}

void CNet_Run::do_start(void)
{
    //init data
    m_iIndex = 0;
    CModbus_Data::init();

    //init tcp server
    m_iFD_Listen = m_TcpServer.socket_bind(m_cIP, m_iPort);
    m_Epl.create(m_iMaxConn);

    //添加一个客户描述符和事件
    m_Epl.add(m_iFD_Listen, EPOLLIN);
    m_iFD_Pipe = CIPC_Pipe::getInstance()->get_read_fd();
    m_Epl.add(m_iFD_Pipe, EPOLLIN);

    //add session
    m_session.add(m_iFD_Listen, 0, m_iPort);

    int num = 0;
    int fd = -1;
    uint32_t ev = 0;

    while (true)
    {
        num = m_Epl.wait(-1);
        //fprintf(stderr, "epoll event num=%d\n", num);

        for (int i = 0; i < num; i++)
        {
            fd = m_Epl.getEvent(i).data.fd;
            ev = m_Epl.getEvent(i).events;

            //根据描述符的类型和事件类型进行处理
            if (ev & EPOLLIN)
            {
                if (fd == m_iFD_Listen) //listen accept
                {
                    do_accept(m_iFD_Listen);
                }
                else if (fd == m_iFD_Pipe) //open the door
                {
                    fprintf (stderr, "so good,we can write data now! ");
                    CIPC_Pipe::getInstance()->read_pipe();

                    //index
                    m_iIndex++;
                    if (m_iIndex >= CModbus_Data::getcount())
                    {
                        m_iIndex = 0;
                    }

                    //send data
                    int num = m_session.get_map()->size();
                    fprintf (stderr, "the total num is:%d, contain listen socket.\n", num);
                    map<int, CNet_Session_Body*>::iterator pos;
                    for (pos = m_session.get_map()->begin(); pos != m_session.get_map()->end(); pos++)
                    {
                        if (pos->first == m_iFD_Listen)
                        {
                            continue;
                        }

                        if (pos->second == NULL)
                        {
                            continue;
                        }

                        do_write(pos->first);
                    }
                }
                else //read or socket close
                {
                    do_read(fd);
                }
            }
            else if ((ev & EPOLLERR) || (ev & EPOLLHUP)|| (!(ev & EPOLLIN))) //有异常发生
            {
                fprintf(stderr, "client abnormal.fd=%d\n", fd);
                close(fd);
                m_Epl.del(fd, EPOLLIN);
                m_session.del(fd);
            }
            else
            {
                fprintf(stderr, "what's wrong?\n");
            }
        }
    }

    fprintf(stderr, "net exit\n");
    m_Epl.del(m_iFD_Listen, EPOLLIN);
    m_Epl.del(m_iFD_Pipe, EPOLLIN);
    CIPC_Pipe::getInstance()->close_read_fd();
}

void CNet_Run::do_accept(int fd)
{
    int ip = 0;
    int port = 0;
    int clifd = m_TcpServer.socket_accept(fd, m_iMaxSocketBuf, &ip, &port);

    //添加一个客户描述符和事件
    m_Epl.add(clifd, EPOLLIN);

    //add session
    m_session.add(clifd, ip, port);
}

void CNet_Run::do_read(int fd)
{
    char buf[m_iMaxSocketBuf];
    memset(buf, 0, m_iMaxSocketBuf);
    int ret = read(fd, buf, m_iMaxSocketBuf);
    if (ret == -1)
    {
        perror("read error:");
        close(fd);
        m_Epl.del(fd, EPOLLIN);
        m_session.del(fd);
    }
    else if (ret == 0)
    {
        fprintf(stderr, "client close.fd=%d\n", fd);
        close(fd);
        m_Epl.del(fd, EPOLLIN);
        m_session.del(fd);
    }
    else
    {
        if (ret > ELMT_BUFSIZE)
        {
            ret = ELMT_BUFSIZE;
        }

        elm_t t;
        t.len = ret;
        t.fd = fd;
        memset(t.buf, 0, ELMT_BUFSIZE);
        memcpy(t.buf, buf, ret);
        if (m_shm.puch_data(t) == 0)
        {
            fprintf(stderr, "puch_data good!fd=%d\n", t.fd);
        }
    }
}

void CNet_Run::do_write(int fd)
{
    m_Epl.mod(fd, EPOLLOUT);

    uint8_t *buf = CModbus_Data::getcmd(m_iIndex);
    int ret = write(fd, buf, sizeof(buf));
    if (ret == -1)
    {
        fprintf(stderr, "write error\n");
        close(fd);
        m_Epl.del(fd, EPOLLOUT);
        m_session.del(fd);
    }
    else
    {
        m_Epl.mod(fd, EPOLLIN);
    }
}

