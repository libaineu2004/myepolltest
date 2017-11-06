#ifndef NET_EPOLLER_H
#define NET_EPOLLER_H

#include <assert.h>
#include <stddef.h>
#include <sys/epoll.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
/**
 * @ epoller操作类，已经默认采用了EPOLLET方式做触发
 */
class CNet_Epoller
{
public:

    /**
      * @ 构造函数.
      * http://blog.csdn.net/libaineu2004/article/details/75235251
      *
     * epoll对文件描述符的操作有两种模式：LT（level trigger）和ET（edge trigger）。LT模式是默认模式，LT模式与ET模式的区别如下：

    * LT模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，应用程序可以不立即处理该事件。下次调用epoll_wait时，会再次响应应用程序并通知此事件。

    * ET模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，应用程序必须立即处理该事件。如果不处理，下次调用epoll_wait时，不会再次响应应用程序并通知此事件。

    * ET模式在很大程度上减少了epoll事件被重复触发的次数，因此效率要比LT模式高。epoll工作在ET模式的时候，必须使用非阻塞套接口，以避免由于一个文件句柄的阻塞读/阻塞写操作把处理多个文件描述符的任务饿死。
      */
    CNet_Epoller(bool bEt = false);//use LT

    /**
     * @ 析够函数.
      */
    ~CNet_Epoller();

    /**
      * @ 生成 epoll句柄.
      *
     * @param max_connections epoll服务需要支持的最大连接数
      */
    void create(int max_connections);

    /**
      * @ 添加监听句柄.
      *
     * @param fd    句柄
     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
     *
      */
    void add(int fd, __uint32_t event);

    /**
      * @ 修改句柄事件.
      *
     * @param fd    句柄
     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
      */
    void mod(int fd, __uint32_t event);

    /**
      * @ 删除句柄事件.
      *
     * @param fd    句柄
     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
      */
    void del(int fd, __uint32_t event);

    /**
      * @ 等待时间.
      *
      * @param millsecond 毫秒
     * @return int       有事件触发的句柄数
      */
    int wait(int millsecond);

    /**
     * @ 获取被触发的事件.
      *
      * @return struct epoll_event&被触发的事件
      */
    struct epoll_event& getEvent(int i) { assert( m_pEvent != 0); return m_pEvent[i]; }

protected:

    /**
      * @ 控制 epoll，将EPOLL设为边缘触发EPOLLET模式
     * @param fd    句柄，在create函数时被赋值
     * @param event 需要监听的事件
      * @param op    EPOLL_CTL_ADD： 注册新的 fd到epfd 中；
      *                  EPOLL_CTL_MOD：修改已经注册的 fd的监听事件；
      *                  EPOLL_CTL_DEL：从 epfd中删除一个fd；
      *
      */
    void ctrl(int fd, __uint32_t events, int op);

private:
    int m_iEpollfd;
    int m_iMax_connections;//最大连接数
    struct epoll_event *m_pEvent;//事件集
    bool m_bEt;//是否是ET模式
};

#endif // NET_EPOLLER_H
