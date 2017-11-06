#include "ipc_pipe.h"

CIPC_Pipe* CIPC_Pipe::s_pInstance = NULL;
CIPC_Pipe::CFree CIPC_Pipe::s_Fe;//类里面的静态变量要在类外部定义,如果没有这句话,程序结束时就不会运行CFree的析构函数.

int CIPC_Pipe::create_pipe(void)
{
    if (pipe(m_fd) == -1)
    {
        fprintf(stderr, "pipe create failed,%s\n", strerror(errno));
        return -1;
    }

    set_nonblocking(m_fd[0]); //read
    set_nonblocking(m_fd[1]); //write

    return 0;
}

int CIPC_Pipe::read_pipe(void)
{
    char buffer[READ_PIPE_BUF_MAX];
    if (read(m_fd[0], buffer, READ_PIPE_BUF_MAX) <= 0)
    {
        fprintf(stderr, "read_pipe failed,%s", strerror(errno));
        return -1;
    }

    return 0;
}

int CIPC_Pipe::write_pipe(void)
{
    char notify_chr = 'A';
    if (write(m_fd[1], &notify_chr, 1) <= 0)
    {
        fprintf(stderr, "write_pipe failed,%s", strerror(errno));
        return -1;
    }

    return 0;
}

int CIPC_Pipe::get_read_fd(void)
{
    return m_fd[0];
}

int CIPC_Pipe::get_write_fd(void)
{
    return m_fd[1];
}

int CIPC_Pipe::close_read_fd(void)
{
    if (close(m_fd[0]) == -1)
    {
        fprintf(stderr, "close_read_fd failed,%s", strerror(errno));
        return -1;
    }

    return 0;
}

int CIPC_Pipe::close_write_fd(void)
{
    if (close(m_fd[1]) == -1)
    {
        fprintf(stderr, "close_write_fd failed,%s", strerror(errno));
        return -1;
    }

    return 0;
}

int CIPC_Pipe::set_nonblocking(int fd)
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
