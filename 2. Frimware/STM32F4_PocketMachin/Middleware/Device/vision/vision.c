#include "vision.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "_Debug.h"

/**
 * @brief     :物品信息表
 * @attention :
 */
const vision_item v_object[] = {
    {"请在下方添加需要检测的物体...", 0x00, 0x00},
    /* 物体信息   指令1  指令2 */
    {"红色正方形", 0x01, 0x01},
    {"绿色正方形", 0x02, 0x01},
    {"蓝色正方形", 0x03, 0x01},
    {"黄色正方形", 0x04, 0x01},
};

lwrb_t vision_uart_buff;      // 视觉识别接收缓冲区句柄
char vision_recv_byte;        // 接收字节
uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

vision_coord v_coords = {0, 0}; // 物品坐标（默认原点坐标:（0,0））
list_t *vision_coord_list;      // 物品坐标存储链表

/* Internal recv buffers */
uint8_t recvlen = 0;
uint8_t recvnum = 0;
uint8_t recv_cnt = 0;
uint8_t recv_coord = 0;
uint8_t _recv_state = 0;

/**
 * @brief     :视觉识别初始化
 * @attention :
 */
void vision_init(void)
{
  // 串口1初始化，usart.c 文件中完成

  vision_coord_list = list_new();                                                                       // 新建一个链表
  list_node_t *v_coord0 = list_rpush(vision_coord_list, list_node_new((vision_obj_typdef *)&v_coords)); // 添加默认对象0（原点坐标）
}

/**
 * @brief     :向视觉模组发送数据
 * @param     dat :数据
 * @param     len :长度
 * @attention :
 */
void vision_senddata(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit(&huart1, data, len, 0xFFFF);
}

/**
 * @brief     :请求识别一个对象（默认固定长度指令）
 * @param     object :Variable
 * @attention :
 */
void vision_requst(char *name)
{
  uint8_t data[8];

  data[0] = VISION_HEAD0; // 帧头
  data[1] = VISION_HEAD1;
  data[2] = 0x03; // 数据长度（不包括帧头帧尾）
  data[3] = 0x01; // 命令

  /* 物品信息数据（通过表格的方式查找） */
  for (int i = 0; i < sizeof(v_object) / sizeof(vision_item); i++)
  {
    if (strcmp(v_object[i].name, name) == 0)
    {
      data[4] = v_object[i].color; // 颜色
      data[5] = v_object[i].type;  // 类型
    }
  }
  data[6] = VISION_TAIL0; // 帧尾
  data[7] = VISION_TAIL1;

  vision_senddata(data, 8);
}

/**
 * @brief     :视觉识别模组 接收图形坐标数据任务
 * @return    :int8_t
 * @attention :
 */
int8_t vision_recv_data(void)
{
  uint8_t dat = 0;

  if (!lwrb_get_full(&vision_uart_buff)) // 检测当前接收字节是否为空
    return -1;

  while (lwrb_get_full(&vision_uart_buff))
  {
    lwrb_read(&vision_uart_buff, &dat, 1); // 读取缓冲区一个字节

    switch (_recv_state)
    {
    case 0:
      v_coords.x_coord = 0;
      v_coords.y_coord = 0;
      recvlen = 0;
      recvnum = 0;
      // match first header byte
      if (dat != VISION_HEAD0)
      {
        recv_cnt = 0;
        return -1;
      }
      printf("%x ", dat);
      _recv_state = 1;
      break;
    case 1:
      // match second header byte or 0 for an OK message
      if (dat != VISION_HEAD1 && dat != 0)
      {
        recv_cnt = 0;
        _recv_state = 0;
        return -1;
      }
      printf("%x ", dat);
      _recv_state = 2;
      break;

    case 2:
      recvlen = dat; // 数据长度
      printf("%x ", dat);
      _recv_state = 3;
      break;

    case 3:
      recvnum = dat; // 图形个数
      printf("%x ", dat);
      _recv_state = 4;
      break;

    case 4:
      recv_cnt++;
      if (recv_cnt > recvlen - 1)
      {
        _recv_state = 5;
      }
      else
      {
        switch (recv_coord)
        {
        case 0:
          v_coords.x_coord |= dat << 8; // X 坐标高八位
          recv_coord = 1;
          break;
        case 1:
          v_coords.x_coord |= dat; // X 坐标低八位
          recv_coord = 2;
          break;
        case 2:
          v_coords.y_coord |= dat << 8; // Y 坐标高八位
          recv_coord = 3;
          break;
        case 3:
          v_coords.y_coord |= dat; // Y 坐标低八位
          recv_coord = 0;
          break;
        default:
          break;
        }
      }
      break;

    case 5:
      if (dat != VISION_TAIL0) // 检测到帧尾
      {
        recv_cnt = 0;
        return -1;
      }
      printf("%x ", dat);
      _recv_state = 6;
      break;
    case 6:
      if (dat != VISION_TAIL1) // 接收到最后一个帧尾，完成一帧数据的接收
      {
        recv_cnt = 0;
        _recv_state = 0;
        return -1;
      }
      printf("%x ", dat);
      _recv_state = 7;
      break;

    case 7:
    {
#ifdef __Debug
      // 打印识别到的图像坐标
      static uint8_t v_number;
      vision_coord *coords;
      list_node_t *node;
      list_iterator_t *it = list_iterator_new(vision_coord_list, LIST_HEAD);
      while ((node == list_iterator_next(it)))
      {
        v_number++;
        coords = node->val;
        printf("检测到图像%d, x轴坐标:%d, Y轴坐标:%d\r\n", v_number, coords->x_coord, coords->y_coord);
      }
#endif
      list_rpush(vision_coord_list, list_node_new((vision_obj_typdef *)&v_coords)); // 把完整的坐标数据添加到链表中
      _recv_state = 0;
      break;
    }

    default:
      break;
    }
  }
  return -1;
}
