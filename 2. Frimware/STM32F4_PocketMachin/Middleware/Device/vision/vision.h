#ifndef __VISION_H__
#define __VISION_H__

#include "main.h"
#include ".\lwrb\lwrb.h"
#include ".\list\list.h"

extern lwrb_t vision_uart_buff;      // ���������ջ��������
extern char vision_recv_byte;        // �����ֽ�
extern uint8_t vision_buff_data[64]; // ����һ���ڴ����ڻ�����

extern list_t *vision_coord_list; // ��Ʒ����洢����

#define VISION_HEAD0 0x5A // ֡ͷ
#define VISION_HEAD1 0xA5
#define VISION_TAIL0 0x9F // ֡β
#define VISION_TAIL1 0xF9

/**
 * @brief     :��Ʒ����
 * @attention :
 */
typedef struct VISION_ITEM
{
  char name[32]; // ��Ʒ����
  uint8_t color; // ��Ʒ��ɫ
  uint8_t type;  // ��Ʒ����
} vision_item;

/**
 * @brief     :��Ʒλ������ֵ
 * @attention :
 */
typedef struct VISION_COORD
{
  uint16_t x_coord; // ��Ʒ x ����
  uint16_t y_coord; // ��Ʒ y ����
} vision_coord;

/**
 * @brief     :�Ӿ�ʶ�����������Ϣ
 * @attention :
 */
typedef struct VISION_OBJECT
{
  vision_item *item;   // ��Ʒ����
  vision_coord *coord; // ��Ʒλ������ֵ
  void *desc;          // ��Ʒ��������
} vision_obj_typdef;

void vision_init(void);
void vision_requst(char *name); // ����ʶ�����Ʒ
int8_t vision_recv_data(void);  // ����ʶ�𵽵�����

#endif
