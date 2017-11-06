#include "net_session.h"

CNet_Session_Body::CNet_Session_Body(int ip, int port, int bufSize)
{
    m_iIP = ip;
    m_iPort = port;
    m_cSend = new char[bufSize];
    m_cRecv = new char[bufSize];
    memset(m_cSend, 0, bufSize);
    memset(m_cRecv, 0, bufSize);
}

CNet_Session_Body::~CNet_Session_Body()
{
    if (m_cSend != NULL)
    {
        delete[] m_cSend;
        m_cSend = NULL;
    }

    if (m_cRecv != NULL)
    {
        delete[] m_cRecv;
        m_cRecv = NULL;
    }
}

//--------------
CNet_Session_Map::CNet_Session_Map(int bufSize)
{
    m_iMaxBuf = bufSize;
}

CNet_Session_Map::~CNet_Session_Map()
{
    map<int, CNet_Session_Body*>::iterator pos;
    for (pos = m_map.begin(); pos != m_map.end(); pos++)
    {
        if (pos->second == NULL)
        {
            continue;
        }

        delete pos->second;
        pos->second = NULL;
    }

    m_map.clear();
}

void CNet_Session_Map::add(int fd, int ip, int port)
{
    CNet_Session_Body* s = new CNet_Session_Body(ip, port, m_iMaxBuf);
    m_map.insert(pair<int, CNet_Session_Body*>(fd, s));
}

void CNet_Session_Map::del(int key)
{
    map<int, CNet_Session_Body*>::iterator pos;

    if ((pos = m_map.find (key)) == m_map.end())
    {
        return;
    }

    CNet_Session_Body* p = pos->second;
    delete p;
    p = NULL;

    m_map.erase(key);
}

CNet_Session_Body* CNet_Session_Map::get_body(int key)
{
    map<int, CNet_Session_Body*>::iterator pos = m_map.find(key);
    if (pos == m_map.end())
    {
        return NULL;
    }

    CNet_Session_Body *p = pos->second;

    return p;
}
