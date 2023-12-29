#include "vision.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

lwrb_t vision_uart_buff;      // �Ӿ�ʶ����ջ��������
char vision_recv_byte;        // �����ֽ�
uint8_t vision_buff_data[64]; // ����һ���ڴ����ڻ�����

/**
 * @brief     :��Ʒ��Ϣ��
 * @attention :
 */
vision_item v_object[] = {
    {"�����Ҫ��������...", 0x00, 0x00},
    {"��ɫ������", 0x01, 0x01},
    {"��ɫ������", 0x02, 0x01},
    {"��ɫ������", 0x03, 0x01},
    {"��ɫ������", 0x04, 0x01},
};

/**
 * @brief     :�Ӿ�ʶ���ʼ��
 * @attention :
 */
void vision_init(void)
{
  // ����1��ʼ����usart.c �ļ������

  // vision_requst("��ɫ������");
}

/**
 * @brief     :���Ӿ�ģ�鷢������
 * @param     dat :����
 * @param     len :����
 * @attention :
 */
void vision_senddata(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit_IT(&huart1, data, len);
}

/**
 * @brief     :����ʶ��һ������
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
      data[4] = v_object[i].color; // ��ɫ
      data[5] = v_object[i].type;  // ����
    }
  }
  data[6] = 0x9F;
  data[7] = 0xF9;

  vision_senddata(data, 8);
}
