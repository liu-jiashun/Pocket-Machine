#ifndef __VISION_H__
#define __VISION_H__

#include "main.h"
#include ".\lwrb\lwrb.h"
#include ".\list\list.h"

extern lwrb_t vision_uart_buff;      // 迪文屏接收缓冲区句柄
extern char vision_recv_byte;        // 接收字节
extern uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

extern list_t *vision_coord_list; // 物品坐标存储链表

#define VISION_HEAD0 0x5A // 帧头
#define VISION_HEAD1 0xA5
#define VISION_TAIL0 0x9F // 帧尾
#define VISION_TAIL1 0xF9

/**
 * @brief     :物品特征
 * @attention :
 */
typedef struct VISION_ITEM
{
  char name[32]; // 物品名称
  uint8_t color; // 物品颜色
  uint8_t type;  // 物品类型
} vision_item;

/**
 * @brief     :物品位置坐标值
 * @attention :
 */
typedef struct VISION_COORD
{
  uint16_t x_coord; // 物品 x 坐标
  uint16_t y_coord; // 物品 y 坐标
} vision_coord;

/**
 * @brief     :视觉识别对象描述信息
 * @attention :
 */
typedef struct VISION_OBJECT
{
  vision_item *item;   // 物品特征
  vision_coord *coord; // 物品位置坐标值
  void *desc;          // 物品其他特征
} vision_obj_typdef;

void vision_init(void);
void vision_requst(char *name); // 请求识别的物品
int8_t vision_recv_data(void);  // 接收识别到的数据

#endif
