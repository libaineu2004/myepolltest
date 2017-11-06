#include "ds_queue.h"

CDS_Queue::CDS_Queue()
{
    m_pElmt = NULL;
    m_pCur = NULL;
    m_iMaxCount = 1;
}

CDS_Queue::~CDS_Queue()
{

}

/*
创建空队列
*/
int CDS_Queue::createEmptyQueue(int maxCount)
{
    if (maxCount < 1)
    {
        return -1;
    }

    m_pElmt = (elm_t *)malloc(sizeof(elm_t) * maxCount);
    if (NULL == m_pElmt)
    {
        return -1;
    }

    m_pCur = (cur_t *)malloc(sizeof(cur_t));
    if (NULL == m_pCur)
    {
        return -1;
    }

    m_iMaxCount = maxCount;
    m_pCur->front = 0;
    m_pCur->rear = 0;

    return 0;
}

/*
 * 摧毁一个队列
  */
void CDS_Queue::destroyQueue(void)
{
    if (NULL != m_pElmt)
    {
        free(m_pElmt);
    }

    if (NULL != m_pCur)
    {
        free(m_pCur);
    }
}

/*
 *入队函数的实现
 */
bool CDS_Queue::enQueue(elm_t value)
{
    if (isFull())
    {
        printf("The queue is full and cannot be inserted again!\n");
        return false;
    }
    else
    {
        //向队列中添加新元素
        memcpy(&m_pElmt[m_pCur->rear], &value, sizeof(elm_t));

        //将rear赋予新的合适的值
        m_pCur->rear = (m_pCur->rear+1) % m_iMaxCount;
        return true;
    }
}

/*
 *出队函数的实现
 */
bool CDS_Queue::outQueue(elm_t* pValue)
{
    //如果队列为空,则返回false
    if (isEmpty())
    {
        printf("The queue is empty, unable to get out of the team!\n");
        return false;
    }
    else
    {
        memcpy(pValue, &m_pElmt[m_pCur->front], sizeof(elm_t));//先进先出
        m_pCur->front = (m_pCur->front+1) % m_iMaxCount;      //移到下一位置
        return true;
    }
}

/*
 *判断队列是否满的函数的实现
 */
bool CDS_Queue::isFull()
{
    if ((m_pCur->rear+1) % m_iMaxCount == m_pCur->front)     //队列满
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
bool CDS_Queue::isEmpty()
{
    if (m_pCur->front == m_pCur->rear)
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
void CDS_Queue::traverseQueue(void)
{
    int i = m_pCur->front;           //从头开始遍历
    printf("Traverse the queue:\n");
    while (i != m_pCur->rear)     //如果没有到达rear位置，就循环
    {
        printf("%d, %d  ", m_pElmt[i].fd, m_pElmt[i].buf[0]);
        i = (i+1) % m_iMaxCount;              //移到下一位置
    }

    printf("\n");
}
