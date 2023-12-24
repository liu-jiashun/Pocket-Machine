#include "ringbuffer.h"
#include <stdio.h>
#include <stdlib.h>

#include ".\MALLOC\malloc.h"

ring pt_ring_buff = {0};

/*环形队列初始化*/
struct ring_buff *fifo_init(void)
{
    struct ring_buff *p = NULL;
    p = (struct ring_buff *)mymalloc(SRAMIN, sizeof(struct ring_buff));
    if (p == NULL)
    {
        printf("fifo_init mymalloc error\n");
        return NULL;
    }
    p->W = 0;
    p->R = 0;
    return p;
}

/*判断环形队列是否已经满了*/
int get_ring_buff_fullstate(struct ring_buff *p_ring_buff)
{
    /*如果写位置减去读位置等于队列长度，就说明这个环形队列已经满*/
    if ((p_ring_buff->W - p_ring_buff->R) == LEN)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

/*判断环形队列为空*/
int get_ring_buff_emptystate(struct ring_buff *p_ring_buff)
{
    /*如果写位置和读的位置相等，就说明这个环形队列为空*/
    if (p_ring_buff->W == p_ring_buff->R)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}
/*插入数据*/
int ring_buff_insert(struct ring_buff *p_ring_buff, int data)
{
    if (p_ring_buff == NULL)
    {
        printf("insert: p null\n");
        return (-1);
    }

    if (get_ring_buff_fullstate(p_ring_buff) == 1)
    {
        printf("insert: buff is full\n");
        return (-2);
    }

    p_ring_buff->array[p_ring_buff->W % LEN] = data;

    p_ring_buff->W++;
    return (0);
}

/*读取环形队列数据*/
int ring_buff_get(struct ring_buff *p_ring_buff)
{
    int data = 0;

    if (p_ring_buff == NULL)
    {
        printf("get: p null\n");
        return (-1);
    }

    if (get_ring_buff_emptystate(p_ring_buff) == 1)
    {
        printf("get: buff is empty\n");
        return (-2);
    }

    data = p_ring_buff->array[p_ring_buff->R % LEN];
    p_ring_buff->R++;
    return data;
}

/*销毁*/
int ring_buff_destory(struct ring_buff *p_ring_buff)
{
    if (p_ring_buff == NULL)
    {
        printf("destory: p null\n");
        return (-1);
    }

    myfree(SRAMIN, p_ring_buff);

    return (0);
}

/*使用例程*/
int RingDemo(void)
{
    int i = 0;

    /*定义一个环形缓冲区*/
    pt_ring_buff = fifo_init();

    /*向环形缓冲区中写入数据*/
    for (i = 0; i < 5; i++)
    {
        ring_buff_insert(pt_ring_buff, i);
    }

    /*从环形缓冲区中读出数据*/
    for (i = 0; i < 4; i++)
    {
        printf("buff[%d] = %d \r\n", (pt_ring_buff->R - 1) % LEN, ring_buff_get(pt_ring_buff));
    }

    for (i = 100; i < 132; i++)
    {
        ring_buff_insert(pt_ring_buff, i);
    }

    /*从环形缓冲区中读出数据*/
    for (i = 0; i < 32; i++)
    {
        printf("buff[%d] = %d \r\n", (pt_ring_buff->R - 1) % LEN, ring_buff_get(pt_ring_buff));
    }

    /*销毁一个环形缓冲区*/
    ring_buff_destory(pt_ring_buff);

    return (1);
}
