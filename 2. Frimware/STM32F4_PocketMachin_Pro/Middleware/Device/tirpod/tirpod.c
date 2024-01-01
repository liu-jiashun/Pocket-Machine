#include "tirpod.h"

void tirpod_init(void)
{
  relay_init();   // 激光笔 继电器初始化
  stepper_init(); // 步进电机初始化
}

/**
 * @brief     :限位检测
 * @param     x_limt :X轴限位状态
 * @param     y_limt :Y轴限位状态
 * @attention :
 */
void tirpod_limit_scan(uint8_t *x_limt, uint8_t *y_limt)
{
  *x_limt = HAL_GPIO_ReadPin(STEPPER1_LIMIT_GPIO_Port, STEPPER1_LIMIT_Pin) ? 1 : 0;

  *y_limt = HAL_GPIO_ReadPin(STEPPER2_LIMIT_GPIO_Port, STEPPER2_LIMIT_Pin) ? 1 : 0;
}
