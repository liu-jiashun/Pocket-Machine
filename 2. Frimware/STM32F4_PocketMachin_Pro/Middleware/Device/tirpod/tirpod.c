#include "tirpod.h"

void tirpod_init(void)
{
  relay_init();   // ����� �̵�����ʼ��
  stepper_init(); // ���������ʼ��
}

/**
 * @brief     :��λ���
 * @param     x_limt :X����λ״̬
 * @param     y_limt :Y����λ״̬
 * @attention :
 */
void tirpod_limit_scan(uint8_t *x_limt, uint8_t *y_limt)
{
  *x_limt = HAL_GPIO_ReadPin(STEPPER1_LIMIT_GPIO_Port, STEPPER1_LIMIT_Pin) ? 1 : 0;

  *y_limt = HAL_GPIO_ReadPin(STEPPER2_LIMIT_GPIO_Port, STEPPER2_LIMIT_Pin) ? 1 : 0;
}
