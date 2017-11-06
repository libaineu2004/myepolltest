#include <iostream>
#include <signal.h>
#include <sys/resource.h>    /*setrlimit */
#include "data_handle.h"
#include "net_run.h"
#include "ipc_pipe.h"

using namespace std;

#define IPADDRESS      "127.0.0.1"
#define PORT           8011 //linux终端命令"netstat -nalp|grep 8011|wc -l",可以查看tcp端口的客户端连接数
#define MAXCONN        50000
#define MAXSOCKETBUF   1024
#define MAXMQCOUNT     1000
bool g_bStop = false;

void sigterm_handler(int signo)
{
    signal(SIGINT, NULL);
    g_bStop = true;
}

void prog_exit(int signo)
{
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;
}

void daemon_run_method1()
{
    int pid;
    signal(SIGCHLD, SIG_IGN);
    //1）在父进程中，fork返回新创建子进程的进程ID；
    //2）在子进程中，fork返回0；
    //3）如果出现错误，fork返回一个负值；
    pid = fork();
    if (pid < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }
    //父进程退出，子进程独立运行
    else if (pid > 0)
    {
        exit(0);
    }

    //之前parent和child运行在同一个session里,parent是会话（session）的领头进程,
    //parent进程作为会话的领头进程，如果exit结束执行的话，那么子进程会成为孤儿进程，并被init收养。
    //执行setsid()之后,child将重新获得一个新的会话(session)id。
    //这时parent退出之后,将不会影响到child了。
    setsid();
    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }

    if (fd > 2)
    {
        close(fd);
    }
}

void daemon_run_method2()
{
    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = prog_exit;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);

    daemon (1, 1);
}

int set_fdlimit()
{
    //设置每个进程允许打开的最大文件数
    //这项功能等价于linux终端命令 "ulimit -n 102400"
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXCONN;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
    {
        perror("setrlimit error");
        return -1;
    }

    return 0;
}

void init_signal(void)//设置信号处理
{
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGKILL, prog_exit);
    signal(SIGTERM, prog_exit);
}

int main(int argc, char *argv[])
{
    //设置信号处理
    init_signal();

    //处理命令行参数
    char ip[30] = {0};
    int port, maxConn, maxSocketBuf, maxMqCount;
    if (argc < 2)
    {
        memcpy(ip, IPADDRESS, sizeof(ip));
        port = PORT;
        maxConn = MAXCONN;
        maxSocketBuf = MAXSOCKETBUF;
        maxMqCount = MAXMQCOUNT;
    }
    else if (argc == 6)
    {//"127.0.0.1" "8011" "1024" "100000" "10000"
        memcpy(ip, argv[1], sizeof(ip));
        port = atoi(argv[2]);
        maxConn = atoi(argv[3]);
        maxSocketBuf = atoi(argv[4]);
        maxMqCount = atoi(argv[5]);
    }
    else
    {
        exit(EXIT_FAILURE);
    }

    //守护者进程
    //daemon_run_method1();
    //daemon_run_method2();

    //设置每个进程允许打开的最大文件数,socket
    if (set_fdlimit() < 0)
    {
        return -1;
    }

    //建立IPC半双工管道，作为业务层的开关量
    if (CIPC_Pipe::getInstance()->create_pipe())
    {
        printf("error in execute main init function\n");
        return -1;
    }

    pid_t pid;

    if ((pid = fork ()) < 0)
    {
        printf ("fork error\n");
        return -1;
    }
    else if (pid > 0)//父进程
    {
        CIPC_Pipe::getInstance()->close_read_fd();
        CData_Handle data(maxMqCount);
        data.do_start();
    }
    else //==0子进程,子进程qt无法单步调试
    {
        CIPC_Pipe::getInstance()->close_write_fd();
        CNet_Run run(ip, port, maxConn, maxSocketBuf, maxMqCount);
        run.do_start();
    }

    return 0;
}
