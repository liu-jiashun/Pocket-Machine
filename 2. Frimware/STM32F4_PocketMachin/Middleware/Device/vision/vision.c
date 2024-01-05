#include "vision.h"
#include "usart.h"
#include "_Debug.h"
#include <string.h>
#include <stdio.h>

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

lwrb_t vision_uart_buff;       // 视觉识别接收缓冲区句柄
char vision_recv_byte;         // 接收字节
uint8_t vision_buff_data[128]; // 开辟一块内存用于缓冲区

/* 物品坐标存储链表 */
list_t *vision_coord_list;

/* Internal recv buffers */
uint8_t recvlen;
uint8_t recvnum;
uint8_t recv_cnt;
uint8_t _recv_state;
uint8_t recvbuf[VSISION_MAX_COORDS * 4];

/**
 * @brief     :视觉识别初始化
 * @attention :
 */
void vision_init(void)
{
  // 串口1初始化，usart.c 文件中完成

  vision_coord_list = list_new(); // 初始化新建一个链表
}

/**
 * @brief     :向视觉模组串口发送数据
 * @param     dat :数据
 * @param     len :长度
 * @attention :
 */
void vision_send(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit(&huart1, data, len, 0xFFFF);
}

/**
 * @brief     :发送数据
 * @param     data :数据
 * @param     len :数据长度
 * @param     cmd :指令
 * @attention :
 */
void vision_send_data(uint8_t *data, uint8_t len, uint8_t cmd)
{
  uint8_t sendbuff[16];

  /* 帧头 */
  sendbuff[0] = VISION_HEAD0;
  sendbuff[1] = VISION_HEAD1;

  /* 数据长度 (指令 + 数据) */
  sendbuff[2] = len + 1;

  /* 指令 */
  sendbuff[3] = cmd;

  /*  数据 */
  for (int i = 0; i < len; i++)
    sendbuff[4 + i] = data[i];

  /* 帧尾 */
  sendbuff[4 + len] = VISION_TAIL0;
  sendbuff[5 + len] = VISION_TAIL1;

  /* 发送一帧数据 */
  vision_send(sendbuff, len + 6);
}

/**
 * @brief     :请求识别一个对象（默认固定长度指令）
 * @param     name :对象名
 * @attention :
 */
void vision_requst(char *name)
{
  uint8_t data[2];

  /* 物品信息数据（通过表格的方式查找） */
  for (int i = 0; i < sizeof(v_object) / sizeof(vision_item); i++)
  {
    if (strcmp(v_object[i].name, name) == 0)
    {
      data[0] = v_object[i].color; // 颜色
      data[1] = v_object[i].type;  // 类型
    }
  }

  /* 发送数据 */
  vision_send_data(data, sizeof(data), 0x01);
}

/**
 * @brief     :数据包处理函数
 * @param     data :图像坐标数据
 * @param     len :数据长度
 * @param     num :图像个数
 * @return    :int
 * @attention :
 */
int _vision_partket(char *data, uint8_t len, uint8_t num)
{
  vision_coord coord_val[VSISION_MAX_COORDS];

  /* 先释放所有列表和节点 (接收到新的数据，只存储当前视图下的数据) */
  list_destroy(vision_coord_list);

  /* 初始化列表 (接收到有效数据) */
  vision_coord_list = list_new();

  /* 计算获得的 num 个图像的坐标值 */
  for (int i = 0; i < num; i++)
  {
    /* X轴坐标 */
    coord_val[i].x_coord = data[i * 4 + 0] << 8; // 高位
    coord_val[i].x_coord |= data[i * 4 + 1];     // 低位

    /* Y轴坐标 */
    coord_val[i].y_coord = data[i * 4 + 2] << 8;
    coord_val[i].y_coord |= data[i * 4 + 3];

    /* 把坐标数据存储到链表中 */
    list_rpush(vision_coord_list, list_node_new((vision_coord *)&coord_val[i]));
  }

#ifdef VISION_DEBUG
  /* 打印完整的数据帧 */
  printf("%X  %X  %X  %X  |", VISION_HEAD0, VISION_HEAD1, len, num);
  printf("  ");
  for (int i = 0; i < len - 1; i++)
    printf("%X  ", data[i]);
  printf("%X  %X  \r\n", VISION_TAIL0, VISION_TAIL1);
#endif

  return 0;
}

/**
 * @brief     :视觉识别模组 接收图形坐标数据任务
 * @return    :int8_t
 * @attention :
 */
int8_t vision_recv_data(void)
{
  uint8_t dat;

  while (lwrb_get_full(&vision_uart_buff)) // 当前缓冲区不为空
  {
    /* 读取缓冲区一个字节 */
    lwrb_read(&vision_uart_buff, &dat, 1);

    switch (_recv_state)
    {
    case 0:
      memset(recvbuf, 0, sizeof(recvbuf)); // 清空缓冲区
      recvlen = 0;                         // 数据长度
      recvnum = 0;                         // 图像个数
      /* match first header byte */
      if (dat != VISION_HEAD0)
      {
        recv_cnt = 0;
        return -1;
      }
      _recv_state = 1;
      break;
    case 1:
      /* match second header byte or 0 for an OK message */
      if (dat != VISION_HEAD1 && dat != 0)
      {
        recv_cnt = 0;
        _recv_state = 0;
        return -1;
      }
      _recv_state = 2;
      break;

    case 2:
      recvlen = dat; // 数据长度
      _recv_state = 3;
      break;

    case 3:
      recvnum = dat; // 图形个数
      _recv_state = 4;
      break;

    case 4:
      recvbuf[recv_cnt] = dat;
      recv_cnt++;
      if (recv_cnt >= recvlen - 1)
      {
        recv_cnt = 0;
        _recv_state = 5;
      }
      break;

    case 5:
      if (dat != VISION_TAIL0) // 检测到帧尾（接收错误）
      {
        _recv_state = 0;
        return -1;
      }
      _recv_state = 6;
      break;
    case 6:
      if (dat != VISION_TAIL1) // 接收到最后一个帧尾（接收错误）
      {
        _recv_state = 0;
        return -1;
      }
      _vision_partket((char *)recvbuf, recvlen, recvnum); // 处理接收到的一帧完整数据
      _recv_state = 0;
      break;
    }
  }
  return 0;
}

/**
 * @brief     :获取当视图下的物品坐标
 * @param     coords :坐标对象
 * @return    :int8_t
 * @attention :
 */
int8_t vision_get_coords(vision_coord *coords)
{
#ifdef VISION_DEBUG
  /* 链表存储数据打印 */
  vision_coord *coord;
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(vision_coord_list, LIST_HEAD);
  while ((node = list_iterator_next(it)) != NULL)
  {
    coord = node->val;
    printf("链表存储: x轴坐标:%d, Y轴坐标:%d\r\n", coord->x_coord, coord->y_coord);
  }
#endif

  return 0;
}
