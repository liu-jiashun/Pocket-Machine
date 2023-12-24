#include "touchkey.h"
#include <stdio.h>
#include <string.h>

static flex_button_t touch_button[TOUCH_BUTTON_MAX];

/**
 * @brief     :初始化
 * @attention :
 */
void touchkey_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成

  int i;

  memset(&touch_button[0], 0x00, sizeof(touch_button));

  for (i = 0; i < TOUCH_BUTTON_MAX; i++)
  {
    touch_button[i].id = i;                                             // 按键的ID号
    touch_button[i].usr_button_read = touch_btn_read;                   // 按键引脚电平读取函数
    touch_button[i].cb = touch_btn_evt_cb;                              // 事件回调函数
    touch_button[i].pressed_logic_level = 0;                            // 设置按键按下的逻辑电平
    touch_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500); // 设置短按事件触发的起始 tick
    touch_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);  // 设置长按事件触发的起始 tick
    touch_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);   // 设置长按保持事件触发的起始 tick

    flex_button_register(&touch_button[i]); // 按键注册
  }
}

/**
 * @brief     :读取按键电平状态
 * @param     arg :Variable
 * @return    :uint8_t
 * @attention :
 */
static uint8_t touch_btn_read(void *arg)
{
  uint8_t value = 0;

  flex_button_t *btn = (flex_button_t *)arg;

  switch (btn->id)
  {
  case TOUCH_BUTTON_UP:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_UP_GPIO_Port, TOUCH_KEY_UP_Pin);
    break;
  case TOUCH_BUTTON_DOWN:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_DOWN_GPIO_Port, TOUCH_KEY_DOWN_Pin);
    break;
  case TOUCH_BUTTON_LEFT:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_LEFT_GPIO_Port, TOUCH_KEY_LEFT_Pin);
    break;
  case TOUCH_BUTTON_RIGHT:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_RIGHT_GPIO_Port, TOUCH_KEY_RIGHT_Pin);
    break;
  case TOUCH_BUTTON_A:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_A_GPIO_Port, TOUCH_KEY_A_Pin);
    break;
  case TOUCH_BUTTON_B:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_B_GPIO_Port, TOUCH_KEY_B_Pin);
    break;
  case TOUCH_BUTTON_C:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_C_GPIO_Port, TOUCH_KEY_C_Pin);
    break;
  case TOUCH_BUTTON_D:
    value = HAL_GPIO_ReadPin(TOUCH_KEY_D_GPIO_Port, TOUCH_KEY_D_Pin);
    break;
  default:
    assert_param(0);
  }
  return value;
}

// 按键事件回调函数
static void touch_btn_evt_cb(void *arg)
{
  flex_button_t *btn = (flex_button_t *)arg;

  // 非组合按键事件处理
  touch_combination_btn_event(btn);

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
static void touch_combination_btn_event(flex_button_t *btn)
{
  switch (btn->id)
  {
  /* 按键 UP */
  case TOUCH_BUTTON_UP:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:
			printf("TOUCH_BUTTON_UP\n");
      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
      printf("TOUCH_BUTTON_UP x2\n");
      break;
    }
    break;
  }

  /* 按键 DOWN */
  case TOUCH_BUTTON_DOWN:
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

    /* 按键 LEFT */
  case TOUCH_BUTTON_LEFT:
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

    /* 按键 RIGHT */
  case TOUCH_BUTTON_RIGHT:
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

    /* 按键 A */
  case TOUCH_BUTTON_A:
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

    /* 按键 B */
  case TOUCH_BUTTON_B:
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

    /* 按键 C */
  case TOUCH_BUTTON_C:
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

    /* 按键 D */
  case TOUCH_BUTTON_D:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:
      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
      break;
    default:
      break;
    }
    break;
  }
  }
}
