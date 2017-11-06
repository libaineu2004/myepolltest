#ifndef NET_RUN_H
#define NET_RUN_H

#include "net_epoller.h"
#include "net_tcp.h"
#include "net_session.h"
#include "ipc_pipe.h"
#include "ipc_shm.h"
#include <map>
using namespace std;

class CNet_Run
{
public:
    CNet_Run(char* ip, int port, int maxConn, int maxSocketBuf, int maxMqCount);
    ~CNet_Run();

public:
    void do_start(void);

protected:
    void do_accept(int fd);
    void do_read(int fd);
    void do_write(int fd);

protected:

private:
    char* m_cIP;
    int m_iPort;
    int m_iMaxConn;
    int m_iMaxSocketBuf;
    int m_iFD_Listen;
    int m_iFD_Pipe;
    int m_iIndex;
    CNet_TCP m_TcpServer;
    CNet_Epoller m_Epl;//如果实例化使用指针会导致tcp accept异常，所以这里没有使用指针
    CNet_Session_Map m_session;
    CIPC_Shm m_shm;
};

#endif // NET_RUN_H
