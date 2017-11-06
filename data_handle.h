#ifndef DATA_HANDLE_H
#define DATA_HANDLE_H

#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ipc_pipe.h"
#include "ipc_shm.h"

class CData_Handle
{
public:
    CData_Handle(int maxMqCount);
    ~CData_Handle();

public:
    void do_start(void);

private:
    CIPC_Shm m_shm;
};

#endif // DATA_HANDLE_H
