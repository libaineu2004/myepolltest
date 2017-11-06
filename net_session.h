#ifndef NET_SESSION_H
#define NET_SESSION_H

#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <map>
using namespace std;

class CNet_Session_Body;
typedef map<int, CNet_Session_Body*> sessin_map;

class CNet_Session_Body
{
public:
    CNet_Session_Body(int ip, int port, int bufSize);
    ~CNet_Session_Body();

private:
    int m_iIP;
    int m_iPort;
    char *m_cSend;
    char *m_cRecv;
};

class CNet_Session_Map
{
public:
    CNet_Session_Map(int bufSize = 1024);
    ~CNet_Session_Map();

public:
    void add(int fd, int ip, int port);
    void del(int key);
    CNet_Session_Body* get_body(int key);
    sessin_map* get_map(void)
    {
        return &m_map;
    }

private:
    int m_iMaxBuf;
    sessin_map m_map;
};

#endif // NET_SESSION_H
