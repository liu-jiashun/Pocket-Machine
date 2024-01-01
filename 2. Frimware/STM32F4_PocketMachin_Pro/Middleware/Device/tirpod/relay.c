#include "relay.h"

/**
 * @brief     :初始化
 * @attention :
 */
void relay_init(void)
{
	// 引脚初始化，gpio.c 文件中已完成
}

/**
 * @brief     :继电器开关控制
 * @param     state :ON 打开，OFF 关闭
 * @attention :
 */
void relay_ctl(uint8_t state)
{
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
