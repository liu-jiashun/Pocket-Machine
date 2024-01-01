#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__

#include "main.h"
#include ".\FlexibleButton\flexible_button.h"

typedef enum
{
  TOUCH_BUTTON_UP = 0,
  TOUCH_BUTTON_DOWN,
  TOUCH_BUTTON_LEFT,
  TOUCH_BUTTON_RIGHT,
  TOUCH_BUTTON_A,
  TOUCH_BUTTON_B,
  TOUCH_BUTTON_C,
  TOUCH_BUTTON_D,

  TOUCH_BUTTON_MAX
} touch_button_t;

void touchkey_init(void);

static uint8_t touch_btn_read(void *arg);
static void touch_btn_evt_cb(void *arg);
static void touch_combination_btn_event(flex_button_t *btn);

#endif
