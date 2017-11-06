#ifndef NET_UDP_H
#define NET_UDP_H

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

class CNet_UDP
{
public:
    CNet_UDP() {;}
    ~CNet_UDP() {;}

public:
    int socket_bind(const char* ip, short port);

protected:
    int set_nonblocking(int fd);
};

#endif // NET_UDP_H
