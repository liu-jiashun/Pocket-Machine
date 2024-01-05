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
 * @brief     :�������д���ַ���
 * @param     str :�ַ���
 * @param     len :����
 * @attention :
 */
void tftlcd_write(char *str, int len)
{
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
