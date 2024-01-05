#include "vision.h"
#include "usart.h"
#include "_Debug.h"
#include <string.h>
#include <stdio.h>

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

lwrb_t vision_uart_buff;       // �Ӿ�ʶ����ջ��������
char vision_recv_byte;         // �����ֽ�
uint8_t vision_buff_data[128]; // ����һ���ڴ����ڻ�����

/* ��Ʒ����洢���� */
list_t *vision_coord_list;

/* Internal recv buffers */
uint8_t recvlen;
uint8_t recvnum;
uint8_t recv_cnt;
uint8_t _recv_state;
uint8_t recvbuf[VSISION_MAX_COORDS * 4];

/**
 * @brief     :�Ӿ�ʶ���ʼ��
 * @attention :
 */
void vision_init(void)
{
  // ����1��ʼ����usart.c �ļ������

  vision_coord_list = list_new(); // ��ʼ���½�һ������
}

/**
 * @brief     :���Ӿ�ģ�鴮�ڷ�������
 * @param     dat :����
 * @param     len :����
 * @attention :
 */
void vision_send(uint8_t *data, uint8_t len)
{
  HAL_UART_Transmit(&huart1, data, len, 0xFFFF);
}

/**
 * @brief     :��������
 * @param     data :����
 * @param     len :���ݳ���
 * @param     cmd :ָ��
 * @attention :
 */
void vision_send_data(uint8_t *data, uint8_t len, uint8_t cmd)
{
  uint8_t sendbuff[16];

  /* ֡ͷ */
  sendbuff[0] = VISION_HEAD0;
  sendbuff[1] = VISION_HEAD1;

  /* ���ݳ��� (ָ�� + ����) */
  sendbuff[2] = len + 1;

  /* ָ�� */
  sendbuff[3] = cmd;

  /*  ���� */
  for (int i = 0; i < len; i++)
    sendbuff[4 + i] = data[i];

  /* ֡β */
  sendbuff[4 + len] = VISION_TAIL0;
  sendbuff[5 + len] = VISION_TAIL1;

  /* ����һ֡���� */
  vision_send(sendbuff, len + 6);
}

/**
 * @brief     :����ʶ��һ������Ĭ�Ϲ̶�����ָ�
 * @param     name :������
 * @attention :
 */
void vision_requst(char *name)
{
  uint8_t data[2];

  /* ��Ʒ��Ϣ���ݣ�ͨ�����ķ�ʽ���ң� */
  for (int i = 0; i < sizeof(v_object) / sizeof(vision_item); i++)
  {
    if (strcmp(v_object[i].name, name) == 0)
    {
      data[0] = v_object[i].color; // ��ɫ
      data[1] = v_object[i].type;  // ����
    }
  }

  /* �������� */
  vision_send_data(data, sizeof(data), 0x01);
}

/**
 * @brief     :���ݰ�������
 * @param     data :ͼ����������
 * @param     len :���ݳ���
 * @param     num :ͼ�����
 * @return    :int
 * @attention :
 */
int _vision_partket(char *data, uint8_t len, uint8_t num)
{
  vision_coord coord_val[VSISION_MAX_COORDS];

  /* ���ͷ������б�ͽڵ� (���յ��µ����ݣ�ֻ�洢��ǰ��ͼ�µ�����) */
  list_destroy(vision_coord_list);

  /* ��ʼ���б� (���յ���Ч����) */
  vision_coord_list = list_new();

  /* �����õ� num ��ͼ�������ֵ */
  for (int i = 0; i < num; i++)
  {
    /* X������ */
    coord_val[i].x_coord = data[i * 4 + 0] << 8; // ��λ
    coord_val[i].x_coord |= data[i * 4 + 1];     // ��λ

    /* Y������ */
    coord_val[i].y_coord = data[i * 4 + 2] << 8;
    coord_val[i].y_coord |= data[i * 4 + 3];

    /* ���������ݴ洢�������� */
    list_rpush(vision_coord_list, list_node_new((vision_coord *)&coord_val[i]));
  }

#ifdef VISION_DEBUG
  /* ��ӡ����������֡ */
  printf("%X  %X  %X  %X  |", VISION_HEAD0, VISION_HEAD1, len, num);
  printf("  ");
  for (int i = 0; i < len - 1; i++)
    printf("%X  ", data[i]);
  printf("%X  %X  \r\n", VISION_TAIL0, VISION_TAIL1);
#endif

  return 0;
}

/**
 * @brief     :�Ӿ�ʶ��ģ�� ����ͼ��������������
 * @return    :int8_t
 * @attention :
 */
int8_t vision_recv_data(void)
{
  uint8_t dat;

  while (lwrb_get_full(&vision_uart_buff)) // ��ǰ��������Ϊ��
  {
    /* ��ȡ������һ���ֽ� */
    lwrb_read(&vision_uart_buff, &dat, 1);

    switch (_recv_state)
    {
    case 0:
      memset(recvbuf, 0, sizeof(recvbuf)); // ��ջ�����
      recvlen = 0;                         // ���ݳ���
      recvnum = 0;                         // ͼ�����
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
      recvlen = dat; // ���ݳ���
      _recv_state = 3;
      break;

    case 3:
      recvnum = dat; // ͼ�θ���
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
      if (dat != VISION_TAIL0) // ��⵽֡β�����մ���
      {
        _recv_state = 0;
        return -1;
      }
      _recv_state = 6;
      break;
    case 6:
      if (dat != VISION_TAIL1) // ���յ����һ��֡β�����մ���
      {
        _recv_state = 0;
        return -1;
      }
      _vision_partket((char *)recvbuf, recvlen, recvnum); // ������յ���һ֡��������
      _recv_state = 0;
      break;
    }
  }
  return 0;
}

/**
 * @brief     :��ȡ����ͼ�µ���Ʒ����
 * @param     coords :�������
 * @return    :int8_t
 * @attention :
 */
int8_t vision_get_coords(vision_coord *coords)
{
#ifdef VISION_DEBUG
  /* ����洢���ݴ�ӡ */
  vision_coord *coord;
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(vision_coord_list, LIST_HEAD);
  while ((node = list_iterator_next(it)) != NULL)
  {
    coord = node->val;
    printf("����洢: x������:%d, Y������:%d\r\n", coord->x_coord, coord->y_coord);
  }
#endif

  return 0;
}
