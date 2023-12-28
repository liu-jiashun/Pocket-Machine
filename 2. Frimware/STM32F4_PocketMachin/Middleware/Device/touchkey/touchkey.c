#include "touchkey.h"
#include <stdio.h>
#include <string.h>

static flex_button_t touch_button[TOUCH_BUTTON_MAX];

/**
 * @brief     :��ʼ��
 * @attention :
 */
void touchkey_init(void)
{
  // ���ų�ʼ����gpio.c �ļ��������

  int i;

  memset(&touch_button[0], 0x00, sizeof(touch_button));

  for (i = 0; i < TOUCH_BUTTON_MAX; i++)
  {
    touch_button[i].id = i;                                             // ������ID��
    touch_button[i].usr_button_read = touch_btn_read;                   // �������ŵ�ƽ��ȡ����
    touch_button[i].cb = touch_btn_evt_cb;                              // �¼��ص�����
    touch_button[i].pressed_logic_level = 0;                            // ���ð������µ��߼���ƽ
    touch_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500); // ���ö̰��¼���������ʼ tick
    touch_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);  // ���ó����¼���������ʼ tick
    touch_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);   // ���ó��������¼���������ʼ tick

    flex_button_register(&touch_button[i]); // ����ע��
  }
}

/**
 * @brief     :��ȡ������ƽ״̬
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

// �����¼��ص�����
static void touch_btn_evt_cb(void *arg)
{
  flex_button_t *btn = (flex_button_t *)arg;

  // ����ϰ����¼�����
  touch_combination_btn_event(btn);

  // // ��ϰ����¼�����
  // if ((flex_button_event_read(&touch_button[TOUCH_BUTTON_UP]) == FLEX_BTN_PRESS_CLICK) &&
  //     (flex_button_event_read(&touch_button[TOUCH_BUTTON_DOWN]) == FLEX_BTN_PRESS_CLICK))
  // {
  //    printf("[combination]: button 0 and button 1, LED ON>>>>>>>>>>>>>>>>>>>>>>>\n");
  // }
}

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
static void touch_combination_btn_event(flex_button_t *btn)
{
  switch (btn->id)
  {
  /* ���� UP */
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

  /* ���� DOWN */
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

    /* ���� LEFT */
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

    /* ���� RIGHT */
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

    /* ���� A */
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

    /* ���� B */
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

    /* ���� C */
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

    /* ���� D */
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
