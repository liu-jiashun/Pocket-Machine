#include "voice.h"
#include "usart.h"

/**
 * @brief     :BY8301模块 初始化
 * @attention :
 */
void BY8301_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成
  // 串口4初始化，usart.c 文件中完成
}

/**
 * @brief     :语音串口发送
 * @param     str :要发送的内容
 * @attention :
 */
void BY8301_SendData(uint8_t *str)
{
  HAL_UART_Transmit_IT(&huart4, str, 8);
}

/**
 * @brief  指定序号播报
 * @param  NOx: mp3文件序号
 * @retval None
 */
void BY8301_Say(uint8_t NOx)
{
  uint8_t arr[8] = {0};
  arr[0] = 0x7E;
  arr[1] = 0x05;
  arr[2] = 0x41;
  arr[3] = 0x00;
  arr[4] = NOx;
  arr[5] = arr[1] ^ arr[2] ^ arr[3] ^ arr[4];
  arr[6] = 0xEF;
  BY8301_SendData(arr);
}

/**
 * @brief     :设置音量
 * @param     volume : 0 ~ 30
 * @attention :
 */
void BY8301_setVolume(uint8_t volume)
{
  if (volume > 30)
    volume = 30;
  uint8_t arr[7];
  arr[0] = 0x7E;
  arr[1] = 0x04;
  arr[2] = 0x31;
  arr[3] = volume;
  arr[4] = arr[1] ^ arr[2] ^ arr[3];
  arr[5] = 0xEF;
  BY8301_SendData(arr);
}

/**
 * @brief     :设置播放暂停
 * @param     playPause :0:pause 1:play
 * @attention :
 */
void BY8301_playPause(uint8_t playPause)
{
  uint8_t arr[6];
  arr[0] = 0x7E;
  arr[1] = 0x03;
  if (playPause)
  {
    arr[2] = 0x01;
  }
  else
  {
    arr[2] = 0x02;
  }
  arr[3] = arr[1] ^ arr[2];
  arr[4] = 0xEF;
  BY8301_SendData(arr);
}

/**
 * @brief     :Busy 引脚状态
 * @return    :uint8_t 返回Busy引脚电平状态 1:busy  0:idle
 * @attention :
 */
uint8_t BY8301_Busy(void)
{
  return HAL_GPIO_ReadPin(VOICE_BUSY_GPIO_Port, VOICE_BUSY_Pin);
}

/**
 * @brief     :语音 初始化
 * @attention :
 */
void voice_init(void)
{
  BY8301_setVolume(15); // 设置音量 %50
}

/**
 * @brief     :播放指定语音文件
 * @param     voice :语音文件（0 ~ 255）
 * @attention :
 */
void voice_say(uint8_t voice)
{
  BY8301_Say(voice);
}
