#include "net_udp.h"

int CNet_UDP::socket_bind(const char* ip, short port)
{
    int	fd;
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//ok
    //inet_pton(AF_INET, ip, &servaddr.sin_addr);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "socket error:%s\n", strerror(errno));
        return -1;
    }

    if (bind(fd, (struct sockaddr *)(&servaddr), sizeof(struct sockaddr)) == -1)
    {
        fprintf (stderr, "bind %s:%d error:%s\n", ip, port, strerror(errno));
        return -1;
    }

    //set nonblock
    set_nonblocking(fd);

    //set socket buffer
    int bufsize = 5000 * 1024;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof(int));
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof(int));

    printf("open udp %s:%d\t[ok]\n", ip, port);

    return fd;
}

int CNet_UDP::set_nonblocking(int fd)
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
