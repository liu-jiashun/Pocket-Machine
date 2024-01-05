#include "matrixkey.h"
#include <stdio.h>
#include <string.h>
#include ".\voice\voice.h"

static flex_button_t matrix_button[MATRIX_BUTTON_MAX];

/**
 * @brief     :初始化
 * @attention :
 */
void matrixkey_init(void)
{
  /* 引脚初始化，gpio.c 文件中已完成 */

  int i;

  memset(&matrix_button[0], 0x0, sizeof(matrix_button));

  for (i = 0; i < MATRIX_BUTTON_MAX; i++)
  {
    matrix_button[i].id = i;                                             // 按键的ID号
    matrix_button[i].usr_button_read = matrix_btn_read;                  // 按键引脚电平读取函数
    matrix_button[i].cb = matrix_btn_evt_cb;                             // 事件回调函数
    matrix_button[i].pressed_logic_level = 1;                            // 设置按键按下的逻辑电平
    matrix_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500); // 设置短按事件触发的起始 tick
    matrix_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);  // 设置长按事件触发的起始 tick
    matrix_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);   // 设置长按保持事件触发的起始 tick

    flex_button_register(&matrix_button[i]); // 按键注册
  }
}

/**
 * @brief     :写入行
 * @param     dat :当前行拉高
 * @attention :
 */
void keyboard_write_row(uint16_t dat)
{
  HAL_GPIO_WritePin(MATRIX_KEY_R1_GPIO_Port, MATRIX_KEY_R1_Pin, ((dat >> 0) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R2_GPIO_Port, MATRIX_KEY_R2_Pin, ((dat >> 1) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R3_GPIO_Port, MATRIX_KEY_R3_Pin, ((dat >> 2) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R4_GPIO_Port, MATRIX_KEY_R4_Pin, ((dat >> 3) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MATRIX_KEY_R5_GPIO_Port, MATRIX_KEY_R5_Pin, ((dat >> 4) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief     :读取列
 * @return    :uint8_t 当前所在列电平状态
 * @attention :
 */
uint8_t keyboard_read_col(void)
{
  uint8_t temp = 0;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C1_GPIO_Port, MATRIX_KEY_C1_Pin) << 0;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C2_GPIO_Port, MATRIX_KEY_C2_Pin) << 1;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C3_GPIO_Port, MATRIX_KEY_C3_Pin) << 2;
  temp |= HAL_GPIO_ReadPin(MATRIX_KEY_C4_GPIO_Port, MATRIX_KEY_C4_Pin) << 3;
  return temp;
}

/**
 * @brief     :矩阵键盘扫描
 * @return    :uint8_t 键盘值
 * @attention :
 */
uint8_t keyboard_scan(void)
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
        case 0xe:
          value = 1 + 4 * i;
          break;
        case 0xd:
          value = 2 + 4 * i;
          break;
        case 0xb:
          value = 3 + 4 * i;
          break;
        case 0x7:
          value = 4 + 4 * i;
          break;
        }
      }
    }
  }
  return value;
}

/**
 * @brief     :读取按键电平状态
 * @param     arg :Variable
 * @return    :uint8_t
 * @attention :
 */
static uint8_t matrix_btn_read(void *arg)
{
  uint8_t value = 0;

  flex_button_t *btn = (flex_button_t *)arg;

  switch (btn->id)
  {
  case MATRIX_KEY_UP:
    value = keyboard_scan() == MATRIX_KEY_UP ? 1 : 0;
    break;
  case MATRIX_KEY_7:
    value = keyboard_scan() == MATRIX_KEY_7 ? 1 : 0;
    break;
  case MATRIX_KEY_8:
    value = keyboard_scan() == MATRIX_KEY_8 ? 1 : 0;
    break;
  case MATRIX_KEY_9:
    value = keyboard_scan() == MATRIX_KEY_9 ? 1 : 0;
    break;

  case MATRIX_KEY_DOWN:
    value = keyboard_scan() == MATRIX_KEY_DOWN ? 1 : 0;
    break;
  case MATRIX_KEY_4:
    value = keyboard_scan() == MATRIX_KEY_4 ? 1 : 0;
    break;
  case MATRIX_KEY_5:
    value = keyboard_scan() == MATRIX_KEY_5 ? 1 : 0;
    break;
  case MATRIX_KEY_6:
    value = keyboard_scan() == MATRIX_KEY_6 ? 1 : 0;
    break;

  case MATRIX_KEY_LEFT:
    value = keyboard_scan() == MATRIX_KEY_LEFT ? 1 : 0;
    break;
  case MATRIX_KEY_1:
    value = keyboard_scan() == MATRIX_KEY_1 ? 1 : 0;
    break;
  case MATRIX_KEY_2:
    value = keyboard_scan() == MATRIX_KEY_2 ? 1 : 0;
    break;
  case MATRIX_KEY_3:
    value = keyboard_scan() == MATRIX_KEY_3 ? 1 : 0;
    break;

  case MATRIX_KEY_RIGHT:
    value = keyboard_scan() == MATRIX_KEY_RIGHT ? 1 : 0;
    break;
  case MATRIX_KEY_D:
    value = keyboard_scan() == MATRIX_KEY_D ? 1 : 0;
    break;
  case MATRIX_KEY_0:
    value = keyboard_scan() == MATRIX_KEY_0 ? 1 : 0;
    break;
  case MATRIX_KEY_E:
    value = keyboard_scan() == MATRIX_KEY_E ? 1 : 0;
    break;

  case MATRIX_KEY_F1:
    value = keyboard_scan() == MATRIX_KEY_F1 ? 1 : 0;
    break;
  case MATRIX_KEY_F2:
    value = keyboard_scan() == MATRIX_KEY_F2 ? 1 : 0;
    break;
  case MATRIX_KEY_F3:
    value = keyboard_scan() == MATRIX_KEY_F3 ? 1 : 0;
    break;
  case MATRIX_KEY_F4:
    value = keyboard_scan() == MATRIX_KEY_F4 ? 1 : 0;
    break;

  default:
    assert_param(0);
  }
  return value;
}

/**
 * @brief     :按键事件回调函数
 * @param     arg :Variable
 * @attention :
 */
static void matrix_btn_evt_cb(void *arg)
{
  flex_button_t *btn = (flex_button_t *)arg;

  /* 非组合按键事件处理 */
  matrix_combination_btn_event(btn);

  // /* 组合按键事件处理 */
  // if ((flex_button_event_read(&touch_button[TOUCH_BUTTON_UP]) == FLEX_BTN_PRESS_CLICK) &&
  //     (flex_button_event_read(&touch_button[TOUCH_BUTTON_DOWN]) == FLEX_BTN_PRESS_CLICK))
  // {
  //   // printf("[combination]: button 0 and button 1, LED ON>>>>>>>>>>>>>>>>>>>>>>>\n");
  // }
}

#include ".\tirpod\tirpod.h"

// #define V_END 300       /* 末速度 */
// #define V_START 0       /* 初速度 */
// #define ACCELTIME 3.5f  /* 加速时间 (s) */
// #define DECEELTIME 1.5f /* 减速时间 (s) */

// __IO uint16_t g_step_angle = 20;        /* 设置的步进步数*/
// extern __IO uint32_t g_add_pulse_count; /* 脉冲个数累计*/
// extern motor_state_typedef g_motor_sta;

// extern uint32_t g_ccr_val; /* 比较值变值 */
// extern uint8_t g_run_flag;
// int angle = 0;

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
      // g_step_angle = g_step_angle + 1;
      // if (g_step_angle >= 50)
      //   g_step_angle = 1;
      // printf("Set_Aangle:%d \r\n", g_step_angle);                  /*设置的旋转位置（角度）*/
      // printf("Add_Aangle:%.2f\r\n", g_add_pulse_count * 0.05625f); /*累计旋转的角度*/

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
      /* 开启电机S型加减速 */
      // if (g_motor_sta == STATE_IDLE)
      // {
      //   printf("stepmotor1_move_rel\n");
      //   g_add_pulse_count = 0;
      //   stepmotor1_move_rel(V_START, V_END, ACCELTIME, DECEELTIME, g_step_angle * SPR); /* 一次加减速运动 */
      // }

      break;

    case FLEX_BTN_PRESS_LONG_START:
      break;
    case FLEX_BTN_PRESS_LONG_UP:
      break;
    }
    break;
  }

  /* 按键 7 */
  case MATRIX_KEY_7:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:

      // g_step_angle = g_step_angle - 1;
      // if (g_step_angle <= 1)
      //   g_step_angle = 50;
      // printf("Set_Aangle:%d \r\n", g_step_angle);                /*设置的旋转位置（角度）*/
      // printf("Add_Aangle:%.2f \r\n", g_add_pulse_count * 0.225); /*累计旋转的角度*/

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      stepper_stop(STEPPER_MOTOR_1);
      ST1_EN(EN_OFF);

      break;
    }

    break;
  }

    /* 按键 8 */
  case MATRIX_KEY_8:
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

    /* 按键 9 */
  case MATRIX_KEY_9:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:

      // g_ccr_val -= 20;
      // if (g_ccr_val <= 20)
      // {
      //   g_ccr_val = 20;
      // }

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      // g_ccr_val += 20;
      // if (g_ccr_val >= 1000)
      // {
      //   g_ccr_val = 1000;
      // }

      break;
    }
    break;
  }

    /* 按键 DOWN */
  case MATRIX_KEY_DOWN:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:

      // if (g_run_flag == 0)
      // {
      //   angle += 90;
      //   if (angle >= 0)
      //   {
      //     g_stepper.angle = angle;
      //     g_stepper.dir = CW;
      //   }
      //   else
      //   {
      //     g_stepper.angle = -angle;
      //     g_stepper.dir = CCW;
      //   }
      //   printf("旋转的角度为：%d\r\n", angle);
      // }

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      // if (g_run_flag == 0)
      // {
      //   angle -= 90;
      //   if (angle >= 0)
      //   {
      //     g_stepper.angle = angle;
      //     g_stepper.dir = CW;
      //   }
      //   else
      //   {
      //     g_stepper.angle = -angle;
      //     g_stepper.dir = CCW;
      //   }
      //   printf("旋转的角度为：%d\r\n", angle);
      // }

      break;
    }
    break;
  }

    /* 按键 4 */
  case MATRIX_KEY_4:
  {
    switch (btn->event)
    {
    case FLEX_BTN_PRESS_DOWN:
      break;
    case FLEX_BTN_PRESS_CLICK:

      // if (g_run_flag == 0)
      // {
      //   ST1_EN(EN_ON);
      //   angle = 0;                                                          /* 角度清0，以便下次设置 */
      //   stepper_set_angle(g_stepper.angle, g_stepper.dir, STEPPER_MOTOR_1); /* 开启旋转 */
      //   printf("开启旋转\r\n");
      // }

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      break;
    }
    break;
  }
  }
}
