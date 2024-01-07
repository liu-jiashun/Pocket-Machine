#include "tftlcd.h"

/**
 * @brief     :TFTLCD ��ʼ�����ڴ�����ʾ����ʼ����
 * @attention :
 */
void tftlcd_init(void)
{
  /* ���ų�ʼ����gpio.c �ļ�������� */
  /* SPI1 ��ʼ����spi.c �ļ������ */

  ST7789_Init(); // ST7789 ��ʼ��
}

/**
 * @brief     :��TFTLCDд���ַ���
 * @param     str :�ַ���
 * @param     len :����
 * @return    :int
 * @attention :
 */
int tftlcd_write(char *str, int len)
{
  /* ���� */
  static uint16_t x = 0, y = 0;

  /* ѭ����ӡ */
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
 * @brief     :TFTLCD��ӡ����
 * @param     fmt :��ʽ���ַ�
 * @param     ... :����
 * @return    :int
 * @attention :
 */
int tftlcd_printf(const char *fmt, ...)
{
  char *buffer = mymalloc(SRAMIN, 256); // ����һ���ڴ�

  va_list args;
  int n;

  va_start(args, fmt);
  n = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  tftlcd_write(buffer, n);

  myfree(SRAMIN, buffer); // �ͷ��ڴ�

  return n;
}