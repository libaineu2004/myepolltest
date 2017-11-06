#ifndef DS_QUEUE_H
#define DS_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
 *循环队列，用数组实现
 * http://blog.csdn.net/libaineu2004/article/details/76851469
 */

#define ELMT_BUFSIZE  200
#define ELMT_EMPTY    -2

typedef struct SCursor //之所以定义为结构体，是为了方便共享内存，游标变量必须放入共享内存
{
    int front;     //指向头结点
    int rear;      //指向最后一个元素的下一结点
} cur_t;

typedef struct SElement
{
    char buf[ELMT_BUFSIZE];
    int len;
    int fd;
} elm_t;

class CDS_Queue
{
public:
    CDS_Queue();
    ~CDS_Queue();

public:
    int createEmptyQueue(int maxCount);
    void destroyQueue(void);
    bool enQueue(elm_t value);       //入队的函数
    bool outQueue(elm_t *pValue);    //出队的函数,同时保存出队的元素
    bool isEmpty(void);              //判断队列是否为空的函数
    elm_t *getQueueHead(void)
    {
        return m_pElmt;
    }

    void setQueueHead(elm_t *pElmt)
    {
        m_pElmt = pElmt;
    }

    cur_t *getCursor(void)
    {
        return m_pCur;
    }

    void setCursor(cur_t *cur)
    {
        m_pCur = cur;
        m_pCur->front = 0;
        m_pCur->rear = 0;
    }

    void setMaxCount(int count)
    {
        m_iMaxCount = count;
    }

protected:
    bool isFull(void);               //判断队列是否满的函数
    void traverseQueue(void);        //遍历队列的函数

private:
    elm_t *m_pElmt;
    cur_t *m_pCur;
    int m_iMaxCount;
};

#endif // DS_QUEUE_H
