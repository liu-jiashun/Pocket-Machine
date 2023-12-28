#include "beep.h"

/**
 * @brief     :��ʼ��
 * @attention :
 */
void beep_init(void)
{
  // ���ų�ʼ����gpio.c �ļ��������
}

/**
 * @brief     :����������
 * @param     state :ON ����OFF ��
 * @attention :
 */
void beep_ctl(uint8_t state)
{
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
