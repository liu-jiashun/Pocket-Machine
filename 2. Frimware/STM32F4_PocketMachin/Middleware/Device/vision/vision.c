#include "vision.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "_Debug.h"

/**
 * @brief     :��Ʒ��Ϣ��
 * @attention :
 */
const vision_item v_object[] = {
    {"�����·������Ҫ��������...", 0x00, 0x00},
    /* ������Ϣ   ָ��1  ָ��2 */
    {"��ɫ������", 0x01, 0x01},
    {"��ɫ������", 0x02, 0x01},
    {"��ɫ������", 0x03, 0x01},
    {"��ɫ������", 0x04, 0x01},
};

lwrb_t vision_uart_buff;      // �Ӿ�ʶ����ջ��������
char vision_recv_byte;        // �����ֽ�
uint8_t vision_buff_data[64]; // ����һ���ڴ����ڻ�����

vision_coord v_coords = {0, 0}; // ��Ʒ���꣨Ĭ��ԭ������:��0,0����
list_t *vision_coord_list;      // ��Ʒ����洢����

/* Internal recv buffers */
uint8_t recvlen = 0;
uint8_t recvnum = 0;
uint8_t recv_cnt = 0;
uint8_t recv_coord = 0;
uint8_t _recv_state = 0;

/**
 * @brief     :�Ӿ�ʶ���ʼ��
 * @attention :
 */
void vision_init(void)
{
  // ����1��ʼ����usart.c �ļ������

  vision_coord_list = list_new();                                                                       // �½�һ������
  list_node_t *v_coord0 = list_rpush(vision_coord_list, list_node_new((vision_obj_typdef *)&v_coords)); // ���Ĭ�϶���0��ԭ�����꣩
}

/**
 * @brief     :���Ӿ�ģ�鷢������
 * @param     dat :����
 * @param     len :����
 * @attention :
 */
void vision_senddata(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit(&huart1, data, len, 0xFFFF);
}

/**
 * @brief     :����ʶ��һ������Ĭ�Ϲ̶�����ָ�
 * @param     object :Variable
 * @attention :
 */
void vision_requst(char *name)
{
  uint8_t data[8];

  data[0] = VISION_HEAD0; // ֡ͷ
  data[1] = VISION_HEAD1;
  data[2] = 0x03; // ���ݳ��ȣ�������֡ͷ֡β��
  data[3] = 0x01; // ����

  /* ��Ʒ��Ϣ���ݣ�ͨ�����ķ�ʽ���ң� */
  for (int i = 0; i < sizeof(v_object) / sizeof(vision_item); i++)
  {
    if (strcmp(v_object[i].name, name) == 0)
    {
      data[4] = v_object[i].color; // ��ɫ
      data[5] = v_object[i].type;  // ����
    }
  }
  data[6] = VISION_TAIL0; // ֡β
  data[7] = VISION_TAIL1;

  vision_senddata(data, 8);
}

/**
 * @brief     :�Ӿ�ʶ��ģ�� ����ͼ��������������
 * @return    :int8_t
 * @attention :
 */
int8_t vision_recv_data(void)
{
  uint8_t dat = 0;

  if (!lwrb_get_full(&vision_uart_buff)) // ��⵱ǰ�����ֽ��Ƿ�Ϊ��
    return -1;

  while (lwrb_get_full(&vision_uart_buff))
  {
    lwrb_read(&vision_uart_buff, &dat, 1); // ��ȡ������һ���ֽ�

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
      recvlen = dat; // ���ݳ���
      printf("%x ", dat);
      _recv_state = 3;
      break;

    case 3:
      recvnum = dat; // ͼ�θ���
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
          v_coords.x_coord |= dat << 8; // X ����߰�λ
          recv_coord = 1;
          break;
        case 1:
          v_coords.x_coord |= dat; // X ����Ͱ�λ
          recv_coord = 2;
          break;
        case 2:
          v_coords.y_coord |= dat << 8; // Y ����߰�λ
          recv_coord = 3;
          break;
        case 3:
          v_coords.y_coord |= dat; // Y ����Ͱ�λ
          recv_coord = 0;
          break;
        default:
          break;
        }
      }
      break;

    case 5:
      if (dat != VISION_TAIL0) // ��⵽֡β
      {
        recv_cnt = 0;
        return -1;
      }
      printf("%x ", dat);
      _recv_state = 6;
      break;
    case 6:
      if (dat != VISION_TAIL1) // ���յ����һ��֡β�����һ֡���ݵĽ���
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
      // ��ӡʶ�𵽵�ͼ������
      static uint8_t v_number;
      vision_coord *coords;
      list_node_t *node;
      list_iterator_t *it = list_iterator_new(vision_coord_list, LIST_HEAD);
      while ((node == list_iterator_next(it)))
      {
        v_number++;
        coords = node->val;
        printf("��⵽ͼ��%d, x������:%d, Y������:%d\r\n", v_number, coords->x_coord, coords->y_coord);
      }
#endif
      list_rpush(vision_coord_list, list_node_new((vision_obj_typdef *)&v_coords)); // ������������������ӵ�������
      _recv_state = 0;
      break;
    }

    default:
      break;
    }
  }
  return -1;
}
