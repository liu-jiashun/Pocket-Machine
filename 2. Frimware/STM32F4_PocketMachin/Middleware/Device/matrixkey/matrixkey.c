#include "matrixkey.h"
#include <string.h>

static flex_button_t matrixkey_button[MATRIX_BUTTON_MAX];

void matrixkey_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成

  int i;

  memset(&matrixkey_button[0], 0x0, sizeof(matrixkey_button));

  for (i = 0; i < MATRIX_BUTTON_MAX; i++)
  {
    matrixkey_button[i].id = i;                                             // 按键的ID号
    matrixkey_button[i].usr_button_read = matrix_btn_read;                  // 按键引脚电平读取函数
    matrixkey_button[i].cb = matrix_btn_evt_cb;                             // 事件回调函数
    matrixkey_button[i].pressed_logic_level = 1;                            // 设置按键按下的逻辑电平
    matrixkey_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500); // 设置短按事件触发的起始 tick
    matrixkey_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);  // 设置长按事件触发的起始 tick
    matrixkey_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);   // 设置长按保持事件触发的起始 tick

    flex_button_register(&matrixkey_button[i]); // 按键注册
  }
}

// 读列
uint8_t keyboard_read_col(void)
{
  uint8_t temp = 0;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C1_GPIO_Port, MATRIX_KEY_C1_Pin) << 0;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C1_GPIO_Port, MATRIX_KEY_C1_Pin) << 1;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C1_GPIO_Port, MATRIX_KEY_C1_Pin) << 2;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C1_GPIO_Port, MATRIX_KEY_C1_Pin) << 3;
  return temp;
}

// 写行
void keyboard_write_row(uint8_t dat)
{
  HAL_GPIO_WritePin(MATRIX_KEY_R1_GPIO_Port, MATRIX_KEY_R1_Pin, ((dat >> 0) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R2_GPIO_Port, MATRIX_KEY_R2_Pin, ((dat >> 1) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R3_GPIO_Port, MATRIX_KEY_R3_Pin, ((dat >> 2) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R4_GPIO_Port, MATRIX_KEY_R4_Pin, ((dat >> 3) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R5_GPIO_Port, MATRIX_KEY_R5_Pin, ((dat >> 4) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// 键盘扫描
uint8_t matrickey_scan(void)
{
  uint8_t value = 0;

  keyboard_write_row(0x00);
  if (keyboard_read_col() != 0x0f)
  {
    if (keyboard_read_col() != 0x0f)
    {
      for (int i = 0; i < 5; i++)
      {
        keyboard_write_row(~(0x01 << i));
        switch (keyboard_read_col())
        {
        case 0x0e:
          value = 1 + 4 * i;
          break;
        case 0x0d:
          value = 2 + 4 * i;
          break;
        case 0x0b:
          value = 3 + 4 * i;
          break;
        case 0x07:
          value = 4 + 4 * i;
          break;
        }
      }
    }
  }

  return value;
}

static uint8_t matrix_btn_read(void *arg)
{
  uint8_t value = 0;

  switch (matrickey_scan())
  {
  case MATRIX_KEY_UP:
    value = matrickey_scan() == MATRIX_KEY_UP ? 1 : 0;
    break;
  case MATRIX_KEY_7:
    value = matrickey_scan() == MATRIX_KEY_7 ? 1 : 0;
    break;
  case MATRIX_KEY_8:
    value = matrickey_scan() == MATRIX_KEY_8 ? 1 : 0;
    break;
  case MATRIX_KEY_9:
    value = matrickey_scan() == MATRIX_KEY_9 ? 1 : 0;
    break;

  case MATRIX_KEY_DOWN:
    value = matrickey_scan() == MATRIX_KEY_DOWN ? 1 : 0;
    break;
  case MATRIX_KEY_4:
    value = matrickey_scan() == MATRIX_KEY_4 ? 1 : 0;
    break;
  case MATRIX_KEY_5:
    value = matrickey_scan() == MATRIX_KEY_5 ? 1 : 0;
    break;
  case MATRIX_KEY_6:
    value = matrickey_scan() == MATRIX_KEY_6 ? 1 : 0;
    break;

  case MATRIX_KEY_LEFT:
    value = matrickey_scan() == MATRIX_KEY_LEFT ? 1 : 0;
    break;
  case MATRIX_KEY_1:
    value = matrickey_scan() == MATRIX_KEY_1 ? 1 : 0;
    break;
  case MATRIX_KEY_2:
    value = matrickey_scan() == MATRIX_KEY_2 ? 1 : 0;
    break;
  case MATRIX_KEY_3:
    value = matrickey_scan() == MATRIX_KEY_3 ? 1 : 0;
    break;

  case MATRIX_KEY_RIGHT:
    value = matrickey_scan() == MATRIX_KEY_RIGHT ? 1 : 0;
    break;
  case MATRIX_KEY_D:
    value = matrickey_scan() == MATRIX_KEY_D ? 1 : 0;
    break;
  case MATRIX_KEY_0:
    value = matrickey_scan() == MATRIX_KEY_0 ? 1 : 0;
    break;
  case MATRIX_KEY_E:
    value = matrickey_scan() == MATRIX_KEY_E ? 1 : 0;
    break;

  case MATRIX_KEY_F1:
    value = matrickey_scan() == MATRIX_KEY_F1 ? 1 : 0;
    break;
  case MATRIX_KEY_F2:
    value = matrickey_scan() == MATRIX_KEY_F2 ? 1 : 0;
    break;
  case MATRIX_KEY_F3:
    value = matrickey_scan() == MATRIX_KEY_F3 ? 1 : 0;
    break;
  case MATRIX_KEY_F4:
    value = matrickey_scan() == MATRIX_KEY_F4 ? 1 : 0;
    break;

  default:
    assert_param(0);
  }
  return value;
}

// 按键事件回调函数
static void matrix_btn_evt_cb(void *arg)
{
  flex_button_t *btn = (flex_button_t *)arg;

  // 非组合按键事件处理
  matrix_combination_btn_event(btn);

  // // 组合按键事件处理
  // if ((flex_button_event_read(&touch_button[TOUCH_BUTTON_UP]) == FLEX_BTN_PRESS_CLICK) &&
  //     (flex_button_event_read(&touch_button[TOUCH_BUTTON_DOWN]) == FLEX_BTN_PRESS_CLICK))
  // {
  //    printf("[combination]: button 0 and button 1, LED ON>>>>>>>>>>>>>>>>>>>>>>>\n");
  // }
}

/**
 * @brief     :非组合按键事件处理
 * @param     btn :
 * @attention :按键触发事件形式
 * FLEX_BTN_PRESS_DOWN = 0,        // 按下事件
 * FLEX_BTN_PRESS_CLICK,           // 单击事件
 * FLEX_BTN_PRESS_DOUBLE_CLICK,    // 双击事件
 * FLEX_BTN_PRESS_REPEAT_CLICK,    // 连击事件，使用 flex_button_t 中的 click_cnt 断定连击次数
 * FLEX_BTN_PRESS_SHORT_START,     // 短按开始事件
 * FLEX_BTN_PRESS_SHORT_UP,        // 短按抬起事件
 * FLEX_BTN_PRESS_LONG_START,      // 长按开始事件
 * FLEX_BTN_PRESS_LONG_UP,         // 长按抬起事件
 * FLEX_BTN_PRESS_LONG_HOLD,       // 长按保持事件
 * FLEX_BTN_PRESS_LONG_HOLD_UP,    // 长按保持的抬起事件
 * @attention :
 */
static void matrix_combination_btn_event(flex_button_t *btn)
{
  switch (btn->id)
  {
  /* 按键 UP */
  case MATRIX_KEY_UP:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:
      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
      break;
    }
    break;
  }
  }
}
