#include "relay.h"

/**
 * @brief     :��ʼ��
 * @attention :
 */
void relay_init(void)
{
	// ���ų�ʼ����gpio.c �ļ��������
}

/**
 * @brief     :�̵������ؿ���
 * @param     state :ON �򿪣�OFF �ر�
 * @attention :
 */
void relay_ctl(uint8_t state)
{
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
