#include "vision.h"
#include "usart.h"

lwrb_t vision_uart_buff;      // 迪文屏接收缓冲区句柄
char vision_recv_byte;        // 接收字节
uint8_t vision_buff_data[64]; // 开辟一块内存用于缓冲区

/**
 * @brief     :视觉识别初始化
 * @attention :
 */
void vision_init(void)
{
  // 串口1初始化，usart.c 文件中完成
}

/**
 * @brief     :向视觉模组发送数据
 * @param     dat :数据
 * @param     len :长度
 * @attention :
 */
void vision_senddata(uint8_t *dat, uint16_t len)
{
  HAL_UART_Transmit_IT(&huart1, dat, len);
}
