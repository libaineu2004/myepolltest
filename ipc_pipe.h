#ifndef IPC_PIPE_H
#define IPC_PIPE_H

#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define READ_PIPE_BUF_MAX	10

class CIPC_Pipe
{
private:
    CIPC_Pipe(){;}//单例模式,禁止外部实例化

public:
    //获取静态对象指针
    static CIPC_Pipe *getInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CIPC_Pipe();
        }

        return s_pInstance;
    }

public:
    int create_pipe(void);
    int read_pipe(void);
    int write_pipe(void);
    int get_read_fd(void);
    int get_write_fd(void);
    int close_read_fd(void);
    int close_write_fd(void);

protected:
    int set_nonblocking(int fd);

private:
    int m_fd[2];

private:
    static CIPC_Pipe *s_pInstance;
    //类CFree被定义为CSingleton的私有内嵌类，以防该类被在其他地方滥用。它的唯一作用就是在析构函数中删除CSingleton的实例。
    //程序运行结束时，系统会调用CSingleton的静态成员CFree的析构函数，该析构函数会删除单例的唯一实例。
    class CFree
    {
    public:
        ~CFree()
        {
            if (CIPC_Pipe::s_pInstance != NULL)
            {
                delete CIPC_Pipe::s_pInstance;
                CIPC_Pipe::s_pInstance = NULL;
            }
        }
    };
    //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
    static CFree s_Fe;
};

#endif // IPC_PIPE_H
