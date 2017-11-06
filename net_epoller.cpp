#include "net_epoller.h"

CNet_Epoller::CNet_Epoller(bool bEt)
{
    m_iEpollfd   = -1;
    m_pEvent     = NULL;
    m_bEt        = bEt;
    m_iMax_connections = 1024;
}

CNet_Epoller::~CNet_Epoller()
{
    if (m_pEvent != NULL)
    {
        delete[] m_pEvent;
        m_pEvent = NULL;
    }

    if (m_iEpollfd > 0)
    {
        close(m_iEpollfd);
    }
}

void CNet_Epoller::create(int max_connections)
{
    m_iMax_connections = max_connections;

    m_iEpollfd = epoll_create(m_iMax_connections);

    //m_iEpollfd = epoll_create1(0);//除了参数size被忽略外,此函数和epoll_create完全相同

    if (m_pEvent != NULL)
    {
        delete[] m_pEvent;
        m_pEvent = NULL;
    }

    m_pEvent = new epoll_event[m_iMax_connections];
}

void CNet_Epoller::ctrl(int fd, __uint32_t events, int op)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    if (m_bEt)
    {
        ev.events = events | EPOLLET;
    }
    else
    {
        ev.events = events;
    }

    int ret = epoll_ctl(m_iEpollfd, op, fd, &ev);
    if (op == EPOLL_CTL_ADD && ret == -1)
    {
        fprintf(stderr, "epoll_ctl add err\n");
    }
}

void CNet_Epoller::add(int fd, __uint32_t event)
{
    ctrl(fd, event, EPOLL_CTL_ADD);
}

void CNet_Epoller::mod(int fd, __uint32_t event)
{
    ctrl(fd, event, EPOLL_CTL_MOD);
}

void CNet_Epoller::del(int fd, __uint32_t event)
{
    ctrl(fd, event, EPOLL_CTL_DEL);
}

int CNet_Epoller::wait(int millsecond)
{
    //millsecond == 0,立即返回
    //millsecond == -1,不清的，永久阻塞
    return epoll_wait(m_iEpollfd, m_pEvent, m_iMax_connections, millsecond);
}
