#include "power.h"

/**
 * @brief     :初始化
 * @attention :
 */
void power_init(void)
{
	// 引脚初始化，gpio.c 文件中已完成
}

/**
 * @brief     :电源开关控制
 * @param     state :ON 打开，OFF 关闭
 * @attention :
 */
void power_ctl(uint8_t state)
{
	HAL_GPIO_WritePin(POWER_GPIO_Port, POWER_Pin, state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
	// if (state == ON)
	// 	HAL_GPIO_WritePin(POWER_GPIO_Port, POWER_Pin, GPIO_PIN_SET);
	// else if (state == OFF)
	// 	HAL_GPIO_WritePin(POWER_GPIO_Port, POWER_Pin, GPIO_PIN_RESET);
}
