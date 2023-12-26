#include "vision.h"
#include "usart.h"
#include <string.h>

lwrb_t vision_uart_buff;      // 迪文屏接收缓冲区句柄
char vision_recv_byte;        // 接收字节
uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

/**
 * @brief     :物品信息表
 * @attention :
 */
vision_obj_typdef *vision_obj;                // 对象访问指针
list_t vision_object_list[VISION_OBJECT_MAX]; // 视觉识别对象数据表

/**
 * @brief     :视觉识别初始化
 * @attention :
 */
void vision_init(void)
{
  // 串口1初始化，usart.c 文件中完成
}

/**
 * @brief     :写入需要识别的对象信息
 * @param     object :Variable
 * @return    :vision_obj_typdef* 返回当前对象指针
 * @attention :
 */
vision_obj_typdef *vidion_object_write(vision_obj_typdef *object)
{
  list_node_t *node = list_rpush(vision_object_list, list_node_new((vision_obj_typdef *)&object));
  return node->val;
}

/**
 * @brief     :向视觉模组发送数据
 * @param     dat :数据
 * @param     len :长度
 * @attention :
 */
void vision_senddata(uint8_t cmd, uint8_t *dat, uint8_t len)
{
  uint8_t i = 0;
  uint8_t data[32];

  data[0] = (VISION_HEAD & 0xFF00) >> 8; // 帧头
  data[1] = VISION_HEAD & 0X00FF;

  data[2] = cmd; // 命令

  while (len--)
    data[3 + i] = dat[i]; // 数据

  data[len + i + 1] = (VISION_TAIL & 0xFF00) >> 8; // 帧尾
  data[len + i + 2] = (VISION_TAIL & 0xFF00) >> 8;

  HAL_UART_Transmit_IT(&huart1, data, len);
}

/**
 * @brief     :请求识别一个对象
 * @param     object :Variable
 * @attention :
 */
void vision_requst_obj(vision_obj_typdef *object, const char *name)
{
  uint8_t data[8];

  data[0] = object->color; // 颜色
  data[1] = object->type;  // 类型

  vision_senddata(0x01, data, 8);
}
