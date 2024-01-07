#include "tftlcd.h"

/**
 * @brief     :TFTLCD 初始化（口袋机显示屏初始化）
 * @attention :
 */
void tftlcd_init(void)
{
  /* 引脚初始化，gpio.c 文件中已完成 */
  /* SPI1 初始化，spi.c 文件中完成 */

  ST7789_Init(); // ST7789 初始化
}

/**
 * @brief     :向TFTLCD写入字符串
 * @param     str :字符串
 * @param     len :长度
 * @return    :int
 * @attention :
 */
int tftlcd_write(char *str, int len)
{
  /* 坐标 */
  static uint16_t x = 0, y = 0;

  /* 循环打印 */
  for (int i = 0; i < len; i++)
  {
    if (str[i] == '\n')
    {
      x = 0;
      return str[i];
    }
    if (x > ST7789_WIDTH - 7)
    {
      x = 0;
      y += 10;
    }
    if (y > ST7789_HEIGHT - 10)
    {
      return str[i];
    }
    ST7789_WriteChar(x, y, str[i], Font_7x10, WHITE, BLACK);
    x += 7;
  }

  return 0;
}
/**
 * @brief     :TFTLCD打印函数
 * @param     fmt :格式化字符
 * @param     ... :参数
 * @return    :int
 * @attention :
 */
int tftlcd_printf(const char *fmt, ...)
{
  char *buffer = mymalloc(SRAMIN, 256); // 申请一段内存

  va_list args;
  int n;

  va_start(args, fmt);
  n = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  tftlcd_write(buffer, n);

  myfree(SRAMIN, buffer); // 释放内存

  return n;
}
