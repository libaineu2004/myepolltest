#ifndef NET_TCP_H
#define NET_TCP_H

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define LISTENQ     SOMAXCONN //10

class CNet_TCP
{
public:
    CNet_TCP() {;}
    ~CNet_TCP() {;}

public:
    int socket_bind(const char* ip, int port);
    int socket_bind(char *port);
    int socket_accept(int listenfd);
    int socket_accept(int listenfd, int maxbuf, int *ip, int *port);
    int socket_accept(int listenfd, int maxbuf);

protected:
    int set_nonblocking(int fd);
};

#endif // NET_TCP_H
