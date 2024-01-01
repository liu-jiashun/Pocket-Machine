#include "vision.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

lwrb_t vision_uart_buff;      // 视觉识别接收缓冲区句柄
char vision_recv_byte;        // 接收字节
uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

/**
 * @brief     :物品信息表
 * @attention :
 */
vision_item v_object[] = {
    {"添加需要检测的物体...", 0x00, 0x00},
    {"红色正方形", 0x01, 0x01},
    {"绿色正方形", 0x02, 0x01},
    {"蓝色正方形", 0x03, 0x01},
    {"黄色正方形", 0x04, 0x01},
};

/**
 * @brief     :视觉识别初始化
 * @attention :
 */
void vision_init(void)
{
  // 串口1初始化，usart.c 文件中完成

  // vision_requst("黄色正方形");
}

/**
 * @brief     :向视觉模组发送数据
 * @param     dat :数据
 * @param     len :长度
 * @attention :
 */
void vision_senddata(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit_IT(&huart1, data, len);
}

/**
 * @brief     :请求识别一个对象
 * @param     object :Variable
 * @attention :
 */
void vision_requst(char *name)
{
  uint8_t data[8];

  data[0] = 0x5A;
  data[1] = 0x5A;
  data[2] = 0x03;
  data[3] = 0x01;
  for (int i = 0; i < sizeof(v_object) / sizeof(vision_item); i++)
  {
    if (strcmp(v_object[i].name, name) == 0)
    {
      data[4] = v_object[i].color; // 颜色
      data[5] = v_object[i].type;  // 类型
    }
  }
  data[6] = 0x9F;
  data[7] = 0xF9;

  vision_senddata(data, 8);
}
