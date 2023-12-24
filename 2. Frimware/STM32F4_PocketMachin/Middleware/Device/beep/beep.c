#include "beep.h"

/**
 * @brief     :初始化
 * @attention :
 */
void beep_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成
}

/**
 * @brief     :蜂鸣器控制
 * @param     state :ON 开，OFF 关
 * @attention :
 */
void beep_ctl(uint8_t state)
{
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
