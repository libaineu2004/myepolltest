#include "ds_queue.h"

CDS_Queue::CDS_Queue()
{
    m_iMaxCount = 1;
}

CDS_Queue::~CDS_Queue()
{

}

/*
创建空队列
*/
queue_t* CDS_Queue::createEmptyQueue(int maxCount)
{
    if (maxCount < 1)
    {
        return NULL;
    }

    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    if (NULL == queue)
    {
        return NULL;
    }

    queue->pElmt = (elm_t *)malloc(sizeof(elm_t) * maxCount);
    if (NULL == queue->pElmt)
    {
        return NULL;
    }

    queue->front = 0;
    queue->rear = 0;
    m_iMaxCount = maxCount;

    return queue;
}

/*
 * 摧毁一个队列
  */
void CDS_Queue::destroyQueue(queue_t *pQueue)
{
    if (NULL != pQueue)
    {
        if (NULL != pQueue->pElmt)
        {
            free(pQueue->pElmt);
        }

        free(pQueue);
    }
}

/*
 *入队函数的实现
 */
bool CDS_Queue::enQueue(queue_t *pQueue, elm_t value)
{
    if (isFull(pQueue))
    {
        printf("队列已满,不能再插入元素了!\n");
        return false;
    }
    else
    {
        //向队列中添加新元素
        memcpy(&pQueue->pElmt[pQueue->rear], &value, sizeof(elm_t));

        //将rear赋予新的合适的值
        pQueue->rear = (pQueue->rear+1) % m_iMaxCount;
        return true;
    }
}

/*
 *出队函数的实现
 */
bool CDS_Queue::outQueue(queue_t *pQueue, elm_t* pValue)
{
    //如果队列为空,则返回false
    if (isEmpty(pQueue))
    {
        printf("队列为空，出队失败!\n");
        return false;
    }
    else
    {
        memcpy(pValue, &pQueue->pElmt[pQueue->front], sizeof(elm_t));//先进先出
        pQueue->front = (pQueue->front+1) % m_iMaxCount;      //移到下一位置
        return true;
    }
}

/*
 *判断队列是否满的函数的实现
 */
bool CDS_Queue::isFull(queue_t *pQueue)
{
    if ((pQueue->rear+1) % m_iMaxCount == pQueue->front)     //队列满
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 *判断队列是否为空函数的实现
 */
bool CDS_Queue::isEmpty(queue_t *pQueue)
{
    if (pQueue->front == pQueue->rear)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 *遍历队列的函数实现
 */
void CDS_Queue::traverseQueue(queue_t *pQueue)
{
    int i = pQueue->front;           //从头开始遍历
    printf("遍历队列：\n");
    while (i != pQueue->rear)     //如果没有到达rear位置，就循环
    {
        printf("%d, %d  ", pQueue->pElmt[i].type, pQueue->pElmt[i].buf[0]);
        i = (i+1) % m_iMaxCount;              //移到下一位置
    }

    printf("\n");
}
