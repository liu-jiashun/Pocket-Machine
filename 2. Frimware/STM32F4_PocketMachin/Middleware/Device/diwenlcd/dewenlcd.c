#include "diwenlcd.h"
#include "usart.h"
#include ".\MALLOC\malloc.h"

#define DIWENLCD_UART_USE_DMA // 串口发送接收使用DMA方式

/**
 * @brief     :MAX485 初始化
 * @attention :
 */
void max485_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成
  // 串口初始化，usart.c 文件中完成
}

/**
 * @brief     :MAX485 发送一定量的数据
 * @param     pData :发送数据
 * @param     Size :字节长度
 * @return    :HAL_StatusTypeDef 状态
 * @attention :
 */
HAL_StatusTypeDef max485_send(const uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  MAX485_DR(MAX485_SEND); // send enable

#ifndef DIWENLCD_UART_USE_DMA
  status = HAL_UART_Transmit(&huart5, pData, Size, HAL_MAX_DELAY);
#else
  status = HAL_UART_Transmit_DMA(&huart5, pData, Size);
#endif

  MAX485_DR(MAX485_RECEIVE); // receive enable

  return status;
}
/**
 * @brief     :MAX485 接收一定量的数据
 * @param     pData :接收的数据
 * @param     Size :接收字节长度
 * @return    :HAL_StatusTypeDef
 * @attention :
 */
HAL_StatusTypeDef max485_receive(uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  MAX485_DR(MAX485_RECEIVE); // receive enable

#ifndef DIWENLCD_UART_USE_DMA
  status = HAL_UART_Receive(&huart5, pData, Size, HAL_MAX_DELAY);
#else
  HAL_UART_Receive_DMA(&huart5, pData, Size);
#endif

  return status;
}

/**
 * @brief     :迪文屏延时回调
 * @param     ms :Variable
 * @attention :
 */
void dgus_delay(uint16_t ms)
{
  HAL_Delay(ms);
}

/**
 * @brief     :可用字节数回调函数
 * @return    :uint8_t 可用字节数（default 32）
 * @attention :
 */
uint8_t _serial_bytes_available()
{
  uint8_t Size = 32;
  return Size;
}
/**
 * @brief     :串口接收回调
 * @return    :char 接收字节
 * @attention :
 */
char _serial_recv_byte()
{
  char c;
  max485_receive((void *)&c, 1);
  return c;
}
/**
 * @brief     :串口发送回调
 * @param     data :发送字节
 * @param     len :数据长度
 * @attention :
 */
void _serial_send_data(char *data, size_t len)
{
  max485_send((void *)&data, len);
}
/**
 * @brief     :接收处理回调
 * @param     data :接收数据
 * @param     cmd :指令
 * @param     len :长度
 * @param     addr :地址
 * @param     bytelen :字节长度
 * @attention :
 */
void _a_recv_handler(char *data, uint8_t cmd, uint8_t len, uint16_t addr, uint8_t bytelen)
{
  if (addr == 0x6060 && data[len] == 0x01) // 上一页
  {
  }
  else if (addr == 0x6062 && data[len] == 0x02) // 下一页
  {
  }
}

/**
 * @brief     :迪文屏初始化
 * @attention :
 */
void diwenlcd_init(void)
{
  // init the library with our callbacks
  dgus_init(_serial_bytes_available, _serial_recv_byte, _serial_send_data, _a_recv_handler);

  // set the first page
  // dgus_get_page(2);

  // clear full diwenlcd display
  dgus_set_var(0x6000, 0);
  dgus_set_var(0x6002, 0);
  dgus_set_var(0x6004, 0);
  dgus_set_var(0x6006, 0);
  dgus_set_var(0x6008, 0);
  dgus_set_var(0x6020, 0);
  dgus_set_var(0x600A, 0);
  dgus_set_var(0x600C, 0);
  dgus_set_text_padded(0x0081, "        ", 8);
  dgus_set_text_padded(0x0082, "          ", 10);
}

/**
 * @brief     :迪文屏时间显示
 * @param     times :时间（s）
 * @attention :
 */
void diwenlcd_time_show(uint32_t times)
{
  dgus_set_var(0x6000, times);
}

/**
 * @brief     :向迪文屏写入字符串
 * @param     str :字符串
 * @param     len :长度
 * @attention :
 */
void diwenlcd_write(char *str, int len)
{
  // static last_line_addr;
  // uint8_t line_count;                     // 列计数
  // uint16_t row_count;                     // 行字节计数
  // uint16_t addr = 0x5000;                 // 文本框基地址
  // char *buff = mymalloc(SRAMIN, 2048);    // 申请一段内存，作为迪文屏终端显存
  // strncpy(buff, str, len); // 复制一行的数据到申请的内存
  // while (len--)
  // {
  //   row_count++;
  //   if (buff[len] == '\n' || row_count >= DIWENLCD_MAX_SIZES) // 换行
  //   {
  //     line_count++;
  //     row_count = 0;
  //     addr += 0x20; // 文本框地址递增
  //   }
  //   dgus_set_text_padded(addr, buff, DIWENLCD_MAX_SIZES); // 迪文屏文本框显示全部数据
  // }
  static uint8_t line_count;
  static uint16_t addr = 0x5000;                                          // 文本框基地址
  char *buff = mymalloc(SRAMIN, DIWENLCD_MAX_LINES * DIWENLCD_MAX_SIZES); // 申请一段内存，作为迪文屏终端显存
  strncpy(buff, str, len);                                                // 复制数据到申请的内存

  if (line_count < DIWENLCD_MAX_LINES) // 未超过最大行数
  {
    for (int i = 0; i < DIWENLCD_MAX_LINES; i++)
    {
      dgus_set_text_padded(addr, buff, DIWENLCD_MAX_SIZES); // 迪文屏文本框显示全部数据
      addr = addr + 0x20;
      if (len / DIWENLCD_MAX_SIZES < i)
        break;
    }
  }
  else // 超过最大行数
  {
  }

  line_count ++;

  myfree(SRAMIN, buff); // 释放内存
}
/**
 * @brief     :迪文屏打印函数
 * @param     fmt :Variable
 * @param     ... :Variable
 * @return    :int
 * @attention :
 */
int diwenlcd_printf(const char *fmt, ...)
{
  char *buffer = mymalloc(SRAMIN, 256); // 申请一段内存

  va_list args;
  int n;

  va_start(args, fmt);
  n = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  diwenlcd_write(buffer, n);

  myfree(SRAMIN, buffer); // 释放内存

  return n;
}

/**
 * @brief     :迪文屏显示不同模式下检测物品的计数
 * @param     mode :工作模式
 * @param     count :计数值
 * @attention :
 */
void diwenlcd_mode_count(diwenlcd_mode mode, uint16_t count)
{
  switch (mode)
  {
  case DIWENLCD_MANUAL_MODE:
    dgus_set_var(0x6002, count); // 手动模式
    break;
  case DIWENLCD_STATIC_MODE:
    dgus_set_var(0x6004, count); // 静态模式
    break;
  case DIWENLCD_SLOW_MODE:
    dgus_set_var(0x6006, count); // 慢速模式
    break;
  case DIWENLCD_FAST_MODE:
    dgus_set_var(0x6008, count); // 快速模式
    break;
  default:
    break;
  }
}

/**
 * @brief     :迪文屏显示不同工作模式
 * @param     mode :工作模式
 * @attention :
 */
void diwenlcd_mode_show(diwenlcd_mode mode)
{
  switch (mode)
  {
  case DIWENLCD_MANUAL_MODE:
    dgus_set_text_padded(0x0081, "手动模式", 8); // 手动模式
    break;
  case DIWENLCD_STATIC_MODE:
    dgus_set_text_padded(0x0081, "静态模式", 8); // 静态模式
    break;
  case DIWENLCD_SLOW_MODE:
    dgus_set_text_padded(0x0081, "慢速模式", 8); // 慢速模式
    break;
  case DIWENLCD_FAST_MODE:
    dgus_set_text_padded(0x0081, "快速模式", 8); // 快速模式
    break;
  default:
    break;
  }
}

/**
 * @brief     :迪文屏显示物品类型描述
 * @param     str :物品描述信息字符串
 * @attention :
 */
void diwenlcd_objects_type(char *str)
{
  uint8_t len = strlen(str);
  dgus_set_text_padded(0x0082, str, len); // 快速模式
}

/**
 * @brief     :迪文屏显示手动输入坐标
 * @param     x_coord :X轴坐标
 * @param     y_coord :Y轴坐标
 * @attention :
 */
void diwenlcd_coord_show(uint16_t x_coord, uint16_t y_coord)
{
  dgus_set_var(0x600A, x_coord);
  dgus_set_var(0x600C, y_coord);
}
