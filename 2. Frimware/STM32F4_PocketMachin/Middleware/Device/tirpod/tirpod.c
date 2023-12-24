#include "tirpod.h"

void tirpod_init(void)
{
  relay_init();     // 激光笔 继电器初始化
  stepmotor_init(); // 步进电机初始化
}

/**
 * @brief     :限位检测
 * @param     x_limt :X轴限位状态
 * @param     y_limt :Y轴限位状态
 * @attention :
 */
void tirpod_limit_scan(uint8_t *x_limt, uint8_t *y_limt)
{
  *x_limt = HAL_GPIO_ReadPin(TIRPOD_X_LIMIT_GPIO_Port, TIRPOD_X_LIMIT_Pin) ? 1 : 0;

  *y_limt = HAL_GPIO_ReadPin(TIRPOD_Y_LIMIT_GPIO_Port, TIRPOD_Y_LIMIT_Pin) ? 1 : 0;
}
