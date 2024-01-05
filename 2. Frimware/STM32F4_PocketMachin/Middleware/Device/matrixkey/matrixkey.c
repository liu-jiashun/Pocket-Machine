#include "matrixkey.h"
#include <stdio.h>
#include <string.h>
#include ".\voice\voice.h"

static flex_button_t matrix_button[MATRIX_BUTTON_MAX];

/**
 * @brief     :��ʼ��
 * @attention :
 */
void matrixkey_init(void)
{
  /* ���ų�ʼ����gpio.c �ļ�������� */

  int i;

  memset(&matrix_button[0], 0x0, sizeof(matrix_button));

  for (i = 0; i < MATRIX_BUTTON_MAX; i++)
  {
    matrix_button[i].id = i;                                             // ������ID��
    matrix_button[i].usr_button_read = matrix_btn_read;                  // �������ŵ�ƽ��ȡ����
    matrix_button[i].cb = matrix_btn_evt_cb;                             // �¼��ص�����
    matrix_button[i].pressed_logic_level = 1;                            // ���ð������µ��߼���ƽ
    matrix_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500); // ���ö̰��¼���������ʼ tick
    matrix_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);  // ���ó����¼���������ʼ tick
    matrix_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);   // ���ó��������¼���������ʼ tick

    flex_button_register(&matrix_button[i]); // ����ע��
  }
}

/**
 * @brief     :д����
 * @param     dat :��ǰ������
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
 * @brief     :��ȡ��
 * @return    :uint8_t ��ǰ�����е�ƽ״̬
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
 * @brief     :�������ɨ��
 * @return    :uint8_t ����ֵ
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
 * @brief     :��ȡ������ƽ״̬
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
 * @brief     :�����¼��ص�����
 * @param     arg :Variable
 * @attention :
 */
static void matrix_btn_evt_cb(void *arg)
{
  flex_button_t *btn = (flex_button_t *)arg;

  /* ����ϰ����¼����� */
  matrix_combination_btn_event(btn);

  // /* ��ϰ����¼����� */
  // if ((flex_button_event_read(&touch_button[TOUCH_BUTTON_UP]) == FLEX_BTN_PRESS_CLICK) &&
  //     (flex_button_event_read(&touch_button[TOUCH_BUTTON_DOWN]) == FLEX_BTN_PRESS_CLICK))
  // {
  //   // printf("[combination]: button 0 and button 1, LED ON>>>>>>>>>>>>>>>>>>>>>>>\n");
  // }
}

#include ".\tirpod\tirpod.h"

// #define V_END 300       /* ĩ�ٶ� */
// #define V_START 0       /* ���ٶ� */
// #define ACCELTIME 3.5f  /* ����ʱ�� (s) */
// #define DECEELTIME 1.5f /* ����ʱ�� (s) */

// __IO uint16_t g_step_angle = 20;        /* ���õĲ�������*/
// extern __IO uint32_t g_add_pulse_count; /* ��������ۼ�*/
// extern motor_state_typedef g_motor_sta;

// extern uint32_t g_ccr_val; /* �Ƚ�ֵ��ֵ */
// extern uint8_t g_run_flag;
// int angle = 0;

/**
 * @brief     :����ϰ����¼�����
 * @param     btn :
 * @attention :���������¼���ʽ
 * FLEX_BTN_PRESS_DOWN = 0,        // �����¼�
 * FLEX_BTN_PRESS_CLICK,           // �����¼�
 * FLEX_BTN_PRESS_DOUBLE_CLICK,    // ˫���¼�
 * FLEX_BTN_PRESS_REPEAT_CLICK,    // �����¼���ʹ�� flex_button_t �е� click_cnt �϶���������
 * FLEX_BTN_PRESS_SHORT_START,     // �̰���ʼ�¼�
 * FLEX_BTN_PRESS_SHORT_UP,        // �̰�̧���¼�
 * FLEX_BTN_PRESS_LONG_START,      // ������ʼ�¼�
 * FLEX_BTN_PRESS_LONG_UP,         // ����̧���¼�
 * FLEX_BTN_PRESS_LONG_HOLD,       // ���������¼�
 * FLEX_BTN_PRESS_LONG_HOLD_UP,    // �������ֵ�̧���¼�
 * @attention :
 */
static void matrix_combination_btn_event(flex_button_t *btn)
{
  switch (btn->id)
  {
  /* ���� UP */
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
      // printf("Set_Aangle:%d \r\n", g_step_angle);                  /*���õ���תλ�ã��Ƕȣ�*/
      // printf("Add_Aangle:%.2f\r\n", g_add_pulse_count * 0.05625f); /*�ۼ���ת�ĽǶ�*/

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
      /* �������S�ͼӼ��� */
      // if (g_motor_sta == STATE_IDLE)
      // {
      //   printf("stepmotor1_move_rel\n");
      //   g_add_pulse_count = 0;
      //   stepmotor1_move_rel(V_START, V_END, ACCELTIME, DECEELTIME, g_step_angle * SPR); /* һ�μӼ����˶� */
      // }

      break;

    case FLEX_BTN_PRESS_LONG_START:
      break;
    case FLEX_BTN_PRESS_LONG_UP:
      break;
    }
    break;
  }

  /* ���� 7 */
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
      // printf("Set_Aangle:%d \r\n", g_step_angle);                /*���õ���תλ�ã��Ƕȣ�*/
      // printf("Add_Aangle:%.2f \r\n", g_add_pulse_count * 0.225); /*�ۼ���ת�ĽǶ�*/

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      stepper_stop(STEPPER_MOTOR_1);
      ST1_EN(EN_OFF);

      break;
    }

    break;
  }

    /* ���� 8 */
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

    /* ���� 9 */
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

    /* ���� DOWN */
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
      //   printf("��ת�ĽǶ�Ϊ��%d\r\n", angle);
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
      //   printf("��ת�ĽǶ�Ϊ��%d\r\n", angle);
      // }

      break;
    }
    break;
  }

    /* ���� 4 */
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
      //   angle = 0;                                                          /* �Ƕ���0���Ա��´����� */
      //   stepper_set_angle(g_stepper.angle, g_stepper.dir, STEPPER_MOTOR_1); /* ������ת */
      //   printf("������ת\r\n");
      // }

      break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:

      break;
    }
    break;
  }
  }
}
