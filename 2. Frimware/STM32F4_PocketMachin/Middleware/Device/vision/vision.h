#ifndef __VISION_H__
#define __VISION_H__

#include "main.h"
#include ".\lwrb\lwrb.h"
#include ".\list\list.h"

extern lwrb_t vision_uart_buff;      // 迪文屏接收缓冲区句柄
extern char vision_recv_byte;        // 接收字节
extern uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

/**
 * @brief     :视觉识别对象描述信息
 * @attention :
 */
typedef struct VISION_OBJECT
{
  char name[32];    // 物品名称
  uint8_t color;    // 物品颜色
  uint8_t type;     // 物品类型
  uint16_t x_coord; // 物品 x 坐标
  uint16_t y_coord; // 物品 y 坐标
} vision_obj_typdef;

void vision_init(void);
void vision_requst_obj(vision_obj_typdef *object);  // 请求识别的对象
void vision_receive_obj(vision_obj_typdef *object); // 识别到的对象

#endif
