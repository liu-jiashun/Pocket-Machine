#ifndef __VISION_H__
#define __VISION_H__

#include "main.h"
#include ".\lwrb\lwrb.h"
#include ".\list\list.h"

extern lwrb_t vision_uart_buff;      // 迪文屏接收缓冲区句柄
extern char vision_recv_byte;        // 接收字节
extern uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

#define VISION_HEAD 0x5AA5 // 帧头
#define VISION_TAIL 0x9FF9 // 帧尾

/* 物品信息表最大个数 */
#define VISION_OBJECT_MAX 32

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
  char name[32];      // 物品名称
  uint8_t color;      // 物品颜色
  uint8_t type;       // 物品类型
  vision_coord coord; // 物品位置坐标值
  void *desc;         // 物品其他特征
} vision_obj_typdef;

void vision_init(void);
vision_obj_typdef *vidion_object_write(vision_obj_typdef *object);   // 写入预置对象
void vision_requst_obj(vision_obj_typdef *object, const char *name); // 请求识别某个对象
void vision_receive_obj(vision_obj_typdef *object);                  // 识别到的对象

#endif
