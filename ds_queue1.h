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

#define BUFSIZE  200

typedef struct Element
{
    char buf[BUFSIZE];
    int type;
} elm_t;

typedef struct Queue
{
    elm_t *pElmt;   //用于动态分配内存
    int front;      //指向头结点
    int rear;       //指向最后一个元素的下一结点
} queue_t;

class CDS_Queue
{
public:
    CDS_Queue();
    ~CDS_Queue();

public:
    queue_t *createEmptyQueue(int maxCount);
    void destroyQueue(queue_t *pQueue);
    bool enQueue(queue_t *pQueue, elm_t value);        //入队的函数
    bool outQueue(queue_t *pQueue, elm_t *pValue);    //出队的函数,同时保存出队的元素
    bool isEmpty(queue_t *pQueue);                   //判断队列是否为空的函数

protected:
    bool isFull(queue_t *pQueue);                    //判断队列是否满的函数
    void traverseQueue(queue_t *pQueue);             //遍历队列的函数

private:
    int m_iMaxCount;
};

#endif // DS_QUEUE_H
