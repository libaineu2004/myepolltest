#include "net_tcp.h"

int CNet_TCP::socket_bind(const char* ip, int port)
{
    int	listenfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf (stderr, "socket error:%s\n",strerror(errno));
        return -1;
    }

    //set nonblock
    set_nonblocking(listenfd);

    //一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用。
    int reuse_addr = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1)
    {
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//ok
    //  servaddr.sin_addr.s_addr = inet_addr("172.16.6.178");
    //  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//使用"127.0.0.1"，实体机windows客户端连接不进来虚拟机
    //  inet_pton(AF_INET, ip, &servaddr.sin_addr);//使用"127.0.0.1"，实体机windows客户端连接不进来虚拟机

    if (bind(listenfd, (struct sockaddr *)(&servaddr), sizeof(servaddr)) == -1)
    {
        fprintf (stderr, "bind %s:%d error:%s\n", ip, port, strerror(errno));
        return -1;
    }

    //backlog是侦听队列的长度，在内核函数中，首先对backlog作检查，如果大于128，则强制使其等于128。
    if (listen(listenfd, LISTENQ) == -1)//SOMAXCONN
    {
        fprintf(stderr, "listen error:%s\n", strerror(errno));
        return -1;
    }

    fprintf(stderr, "TCP Server listenning...port=%d\n", port);

    return listenfd;
}

int CNet_TCP::socket_bind(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, listenfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listenfd == -1)
            continue;

        s = bind(listenfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0)
        {
            /* We managed to bind successfully! */
            break;
        }

        close(listenfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(result);

    //set nonblock
    set_nonblocking(listenfd);

    s = listen(listenfd, SOMAXCONN);
    if (s == -1)
    {
        perror ("listen");
    }

    return listenfd;
}

int CNet_TCP::socket_accept(int listenfd)
{
    static int count = 0;

    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen;
    cliaddrlen = sizeof(cliaddr);//这句话必须有
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
    if (clifd == -1)
    {
        perror("accpet error:");
    }
    else
    {
        printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

        count++;
        printf("accept num=: %d\n",count);
    }

    //set nonblock
    set_nonblocking(clifd);

    return clifd;
}

int CNet_TCP::socket_accept(int listenfd, int maxbuf, int *ip, int *port)
{
    int clifd = -1;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;

    while (1)
    {
        cliaddrlen = sizeof(cliaddr);//这句话必须有
        clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
        *ip = cliaddr.sin_addr.s_addr;
        *port = cliaddr.sin_port;

        if (clifd > 0)
        {
            fprintf(stderr, "accept a new client: fd=%d,ip=%s,port=%d\n", clifd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

            //set nonblock
            set_nonblocking(clifd);

            //set socket buffer
            int bufsize = maxbuf;
            setsockopt(clifd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof (int));
            setsockopt(clifd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof (int));

            return clifd;
        }
        else
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))    //没有连接需要接收了
            {
                break;
            }
            else if (errno == EINTR)    //可能被中断信号打断,,经过验证对非阻塞socket并未收到此错误,应该可以省掉该步判断
            {
                continue;
            }

            perror("accpet error:");
        }
    }

    return clifd;
}

int CNet_TCP::socket_accept(int listenfd, int maxbuf)
{
    struct sockaddr in_addr;
    socklen_t in_len;
    int clifd = -1;
    int s = 0;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    while (1)
    {
        //We have a notification on the listening socket, which
        //means one or more incoming connections.
        in_len = sizeof in_addr;
        clifd = accept(listenfd, &in_addr, &in_len);
        if (clifd == -1)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                //We have processed all incoming connections.
                break;
            }
            else
            {
                perror ("accept");
                break;
            }
        }

        //将地址转化为主机名或者服务名
        s = getnameinfo (&in_addr, in_len,
                         hbuf, sizeof hbuf,
                         sbuf, sizeof sbuf,
                         NI_NUMERICHOST | NI_NUMERICSERV);//flag参数:以数字名返回
        //主机地址和服务地址
        if (s == 0)
        {
            printf("Accepted connection on descriptor"
                   "(fd=%d, host=%s, port=%s)\n", clifd, hbuf, sbuf);
        }

        //set nonblock
        set_nonblocking(clifd);

        //set socket buffer
        int bufsize = maxbuf;
        setsockopt(clifd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof (int));
        setsockopt(clifd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof (int));

        return clifd;
    }

    return -1;
}

int CNet_TCP::set_nonblocking(int fd)
{
    int flags, s;

    //得到文件状态标志
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    //设置文件状态标志
    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}
