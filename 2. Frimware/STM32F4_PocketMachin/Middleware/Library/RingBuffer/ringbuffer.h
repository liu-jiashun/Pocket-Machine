#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

#define LEN 1024

/*环形队列结构体*/
typedef struct ring_buff
{
    unsigned char array[LEN];
    int W;
    int R;
} *ring;

extern ring pt_ring_buff;

/*环形队列初始化*/
struct ring_buff *fifo_init(void);

/*判断环形队列是否已经满了*/
int get_ring_buff_fullstate(struct ring_buff *p_ring_buff);

/*判断环形队列为空*/
int get_ring_buff_emptystate(struct ring_buff *p_ring_buff);

/*插入数据*/
int ring_buff_insert(struct ring_buff *p_ring_buff, int data);

/*读取环形队列数据*/
int ring_buff_get(struct ring_buff *p_ring_buff);

/*销毁*/
int ring_buff_destory(struct ring_buff *p_ring_buff);

/*使用例程*/
int RingDemo(void);

#endif /* __RING_H_ */
