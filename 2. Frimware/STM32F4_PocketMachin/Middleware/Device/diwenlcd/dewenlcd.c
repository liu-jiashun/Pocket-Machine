#include "diwenlcd.h"

lwrb_t diwenlcd_uart_buff;      // ���������ջ��������
char diwenlcd_recv_byte;        // �����ֽ�
uint8_t diwenlcd_buff_data[64]; // ����һ���ڴ����ڻ�����

/**
 * @brief     :MAX485 ��ʼ��
 * @attention :
 */
void max485_init(void)
{
  /* ���ų�ʼ����gpio.c �ļ�������� */
  /* ����5��ʼ����usart.c �ļ������ */
}

/**
 * @brief     :MAX485 ����һ����������
 * @param     pData :��������
 * @param     Size :�ֽڳ���
 * @return    :HAL_StatusTypeDef ״̬
 * @attention :
 */
HAL_StatusTypeDef max485_send(const uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  MAX485_DR(MAX485_SEND); // send enable
  HAL_Delay(5);
  status = HAL_UART_Transmit(&huart5, pData, Size, 0xFFFF);
  HAL_Delay(5);
  MAX485_DR(MAX485_RECEIVE); // receive enable

  return status;
}

/**
 * @brief     :MAX485 ����һ���������ݣ��ӻ�������ȡ�ֽڣ�
 * @param     pData :���յ�����
 * @param     Size :�����ֽڳ���
 * @return    :HAL_StatusTypeDef
 * @attention :
 */
HAL_StatusTypeDef max485_receive(uint8_t *pData, uint16_t Size)
{
  lwrb_read(&diwenlcd_uart_buff, pData, 1);
  return HAL_OK;
}

/**
 * @brief     :��������ʱ�ص�
 * @param     ms :Variable
 * @attention :
 */
void dgus_delay(uint16_t ms)
{
  HAL_Delay(ms); // ����ָ��޷��أ��رճ�ʱ�˳�
}

/**
 * @brief     :��ȡ�ɴӴ��ж˿ڶ�ȡ���ֽڣ��ַ������ص�����õ�ǰ���������ֽ�����
 * @return    :uint8_t �����ֽ���
 * @attention :
 */
uint8_t _serial_bytes_available()
{
  return lwrb_get_full(&diwenlcd_uart_buff);
}

/**
 * @brief     :���ڽ��ջص�
 * @return    :char �����ֽ�
 * @attention :
 */
char _serial_recv_byte()
{
  char c;
  max485_receive((uint8_t *)&c, 1);
  return c;
}

/**
 * @brief     :���ڷ��ͻص�
 * @param     data :�����ֽ�
 * @param     len :���ݳ���
 * @attention :
 */
void _serial_send_data(char *data, size_t len)
{
  max485_send((uint8_t *)data, len);
}

/**
 * @brief     :���մ���ص�
 * @param     data :��������
 * @param     cmd :ָ��
 * @param     len :����
 * @param     addr :��ַ
 * @param     bytelen :�ֽڳ���
 * @attention :
 */
void _a_recv_handler(char *data, uint8_t cmd, uint8_t len, uint16_t addr, uint8_t bytelen)
{
  if (addr == 0x6060 && (uint16_t) * (uint16_t *)data == 0x01) // ��һҳ
  {
    printf("��һҳ\n");
  }
  else if (addr == 0x6062 && (uint16_t) * (uint16_t *)data == 0x02) // ��һҳ
  {
    printf("��һҳ\n");
  }
}

/**
 * @brief     :��������ʼ��
 * @attention :
 */
void diwenlcd_init(void)
{
  // init the library with our callbacks
  dgus_init(_serial_bytes_available, _serial_recv_byte, _serial_send_data, _a_recv_handler);

  // set the first page
  dgus_set_page(2);

  // clear full diwenlcd display
  dgus_set_var(0x6000, 0);
  dgus_set_var(0x6002, 0);
  dgus_set_var(0x6004, 0);
  dgus_set_var(0x6006, 0);
  dgus_set_var(0x6008, 0);
  dgus_set_var(0x6020, 0);
  dgus_set_var(0x600A, 0);
  dgus_set_var(0x600C, 0);
  dgus_set_text_padded(0x0081, "        ", 10);
  dgus_set_text_padded(0x0082, "          ", 12);
}

/**
 * @brief     :������ʱ����ʾ
 * @param     times :ʱ�䣨s��
 * @attention :
 */
void diwenlcd_time_show(uint32_t times)
{
  dgus_set_var(0x6000, times);
}

/**
 * @brief     :�������д���ַ���
 * @param     str :�ַ���
 * @param     len :����
 * @attention :
 */
void diwenlcd_write(char *str, int len)
{
  static uint8_t line_count;
  static uint16_t addr = 0x5000;                                          // �ı������ַ
  char *buff = mymalloc(SRAMIN, DIWENLCD_MAX_LINES * DIWENLCD_MAX_SIZES); // ����һ���ڴ棬��Ϊ�������ն��Դ�
  strncpy(buff, str, len);                                                // �������ݵ�������ڴ�

  if (line_count < DIWENLCD_MAX_LINES) // δ�����������
  {
    for (int i = 0; i < DIWENLCD_MAX_LINES; i++)
    {
      dgus_set_text_padded(addr, buff, DIWENLCD_MAX_SIZES); // �������ı�����ʾȫ������
      addr = addr + 0x20;
      if (len / DIWENLCD_MAX_SIZES < i)
        break;
    }
  }
  else // �����������
  {
  }

  line_count++;

  myfree(SRAMIN, buff); // �ͷ��ڴ�
}
/**
 * @brief     :��������ӡ����
 * @param     fmt :��ʽ���ַ�
 * @param     ... :����
 * @return    :int
 * @attention :
 */
int diwenlcd_printf(const char *fmt, ...)
{
  char *buffer = mymalloc(SRAMIN, 256); // ����һ���ڴ�

  va_list args;
  int n;

  va_start(args, fmt);
  n = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  diwenlcd_write(buffer, n);

  myfree(SRAMIN, buffer); // �ͷ��ڴ�

  return n;
}

/**
 * @brief     :��������ʾ��ͬģʽ�¼����Ʒ�ļ���
 * @param     mode :����ģʽ
 * @param     count :����ֵ
 * @attention :
 */
void diwenlcd_mode_count(diwenlcd_mode mode, uint16_t count)
{
  switch (mode)
  {
  case DIWENLCD_MANUAL_MODE:
    dgus_set_var(0x6002, count); // �ֶ�ģʽ
    break;
  case DIWENLCD_STATIC_MODE:
    dgus_set_var(0x6004, count); // ��̬ģʽ
    break;
  case DIWENLCD_SLOW_MODE:
    dgus_set_var(0x6006, count); // ����ģʽ
    break;
  case DIWENLCD_FAST_MODE:
    dgus_set_var(0x6008, count); // ����ģʽ
    break;
  default:
    break;
  }
}

/**
 * @brief     :��������ʾ��ͬ����ģʽ
 * @param     mode :����ģʽ
 * @attention :
 */
void diwenlcd_mode_show(diwenlcd_mode mode)
{
  switch (mode)
  {
  case DIWENLCD_MANUAL_MODE:
    dgus_set_text_padded(0x0081, "�ֶ�ģʽ", 10); // �ֶ�ģʽ
    break;
  case DIWENLCD_STATIC_MODE:
    dgus_set_text_padded(0x0081, "��̬ģʽ", 10); // ��̬ģʽ
    break;
  case DIWENLCD_SLOW_MODE:
    dgus_set_text_padded(0x0081, "����ģʽ", 10); // ����ģʽ
    break;
  case DIWENLCD_FAST_MODE:
    dgus_set_text_padded(0x0081, "����ģʽ", 10); // ����ģʽ
    break;
  default:
    break;
  }
}

/**
 * @brief     :��������ʾ��Ʒ��������
 * @param     str :��Ʒ������Ϣ�ַ���
 * @attention :
 */
void diwenlcd_objects_type(char *str)
{
  uint8_t len = strlen(str);
  dgus_set_text_padded(0x0082, str, len + 2); // ����ģʽ
}

/**
 * @brief     :��������ʾ�ֶ���������
 * @param     x_coord :X������
 * @param     y_coord :Y������
 * @attention :
 */
void diwenlcd_coord_show(uint16_t x_coord, uint16_t y_coord)
{
  dgus_set_var(0x600A, x_coord);
  dgus_set_var(0x600C, y_coord);
}
