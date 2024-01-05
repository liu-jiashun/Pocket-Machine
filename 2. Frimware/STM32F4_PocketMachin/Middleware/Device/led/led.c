#include "led.h"

/**
 * @brief     :初始化
 * @attention :
 */
void led_init(void)
{
  /* 引脚初始化，gpio.c 文件中已完成 */
}

/**
 * @brief     :LED 控制
 * @param     led :LED0 ~ LED7
 * @param     state :ON 开，OFF 关
 * @attention :
 */
void led_ctl(uint8_t led, uint8_t state)
{
  switch (led)
  {
  case LED0:
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED1:
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED2:
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED3:
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED4:
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED5:
    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED6:
    HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  case LED7:
    HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, state == ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
    break;
  default:
    break;
  }
}
